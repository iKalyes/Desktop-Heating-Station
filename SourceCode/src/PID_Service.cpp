#include <PID_Service.h>
#include <math.h>  // fabsf

// ---------- 高惯性优化：积分分离、死区、输出限速与滤波 ----------
// 配置参数（可按需调参）
const float integral_sep_band = 8.0f;      // from 5.0f：更晚启用积分，抑制超调
const float deadband = 1.5f;               // from 1.0f：增大稳态容忍，减小抖动
const float max_duty_step_per_call = 3.0f; // from 5.0f：更慢的占空比斜率
const float duty_alpha = 0.4f;             // from 0.3f：更强平滑

// 设定点爬坡与动态限功率/预测提前收功参数
const float sp_ramp_rate_cps = 1.2f;       // from 1.5f：更慢爬坡，降低储能过量
const float pred_horizon_s = 10.0f;        // from 8.0f：看得更远
const float pred_margin_c = 0.8f;          // from 0.5f：更保守的提前收功

static float prev_applied_duty = 0.0f;
static float duty_hold = 0.0f;
static int last_temp = 0;
static unsigned long last_temp_time = 0;
static float last_Ki_eff = -1.0f;

// 设定点爬坡状态
static float sp_ramp = 0.0f;
static int   last_set_target = -32768;
static bool  sp_initialized = false;

// 新增：加热计时状态
static bool timer_heating_prev = false;
static unsigned long timer_last_ms = 0;
static unsigned int  timer_acc_ms  = 0;

// 新增：曲线模式与时间基准
static unsigned long heating_start_ms = 0;        // 加热开始时刻（ms）
static int last_mode = -1;                        // 上次生效的模式
static float last_mode_target = 0.0f;             // 上次模式目标（用于跟踪变化）

//
//积分分离带 integral_sep_band：5℃（3~8℃范围内调试）
//死区 deadband：1℃（0.5~1.5℃范围内调试）
//每周期最大变化 max_duty_step_per_call：5%/200ms（即约25%/s，3~8%/步可调）
//低通滤波系数 duty_alpha：0.3（0.2~0.5可调）
//
//首先把 Ki 降低一些，再根据响应慢/超调程度微调 integral_sep_band 和 deadband。
//若温度仍有小幅抖动，增大 duty_alpha 或减小 max_duty_step_per_call。
//若升温太慢，可适当增大 max_duty_step_per_call，但注意配合 deadband 以避免来回抖动。
//
//设定点爬坡速率 sp_ramp_rate_cps：1.2~2.0 ℃/s（默认1.5）
//预测地平线 pred_horizon_s：6~10 s（默认8）
//预测裕量 pred_margin_c：0.3~1.0 ℃（默认0.5）
//动态限功率分段：≤5℃:25%，≤10℃:40%，≤15℃:70%，≤20℃:85%，>20℃:100%（可按超调情况微调）
//PID初值可尝试收敛：Kp 60<del>90，Ki 0.6</del>1.2，Kd 15~30（先按代码策略改完再调参）
//
//先降 Kp 到 60<del>90，Ki 到 0.6</del>1.2，Kd 提到 15~30；观察超调是否明显减小。
//若仍有超调，增大 pred_horizon_s 到 9<del>10 或将 pred_margin_c 提到 0.7</del>1.0；同时把 ≤10℃ 的动态上限降到 30%。
//若升温过慢，把 sp_ramp_rate_cps 提到 1.8~2.0，并将 ≤15℃ 上限提高到 80%。
//稳态小抖动时，增大 duty_alpha 到 0.4 或把 max_duty_step_per_call 降到 3~4。
//

QuickPID Heater_PID(&Heater_temp_float, &Heater_DutyCycle, &Heater_target_temp_float,
                    Heater_KP, Heater_KI, Heater_KD,
                    QuickPID::pMode::pOnErrorMeas,
                    QuickPID::dMode::dOnMeas,
                    QuickPID::iAwMode::iAwClamp,
                    QuickPID::Action::direct);

void Heater_PID_Compute_Init()
{
    Heater_PID.SetOutputLimits(0, 100);
    Heater_PID.SetSampleTimeUs(200000); // 200ms
    Heater_PID.SetTunings(Heater_KP, Heater_KI, Heater_KD);
    // 启用抗积分饱和（高惯性系统强烈建议）
    Heater_PID.SetAntiWindupMode(QuickPID::iAwMode::iAwClamp);
    Heater_PID.SetMode(QuickPID::Control::automatic);
}

void Heater_PID_Update_Tunings(float Kp, float Ki, float Kd)
{
    Heater_PID.SetTunings(Kp, Ki, Kd);
}

// 模式目标获取：未提供曲线时回退到 HeaterTargetTemp
static inline float GetModeTargetTemp(unsigned long now_ms)
{
    switch (Heating_Mode)
    {
        case 1: { // RSS曲线（内置）
            const unsigned long elapsed = (heating_start_ms == 0) ? 0UL : (now_ms - heating_start_ms);
            return RSS_Curve(elapsed);
        }
        case 2: { // RTS曲线（内置）
            const unsigned long elapsed = (heating_start_ms == 0) ? 0UL : (now_ms - heating_start_ms);
            return RTS_Curve(elapsed);
        }
        case 3: { // 自定曲线
            const unsigned long elapsed = (heating_start_ms == 0) ? 0UL : (now_ms - heating_start_ms);
            return CUSTOM_Curve(elapsed);
        }
        case 0:
        default:
            return (float)HeaterTargetTemp;
    }
}

void Heater_PID_Compute()
{    
    if(Heater_Enabled == true)
    {
        static unsigned long last_execution_time = 0;
        static bool error_buzzer_started = false;  // 异常蜂鸣器状态跟踪
        const unsigned long current_time = millis();
        
        // 200ms非阻塞延迟检查
        if (current_time - last_execution_time < 200)
        {
            return;
        }
        last_execution_time = current_time;

        // 新增：加热计时（在任何可能return前更新）
        if (!timer_heating_prev) {
            // 刚开始加热：清零计时并初始化
            Timer_Minutes = 0;
            Timer_Seconds = 0;
            timer_acc_ms = 0;
            timer_last_ms = current_time;
            timer_heating_prev = true;

            // 新增：时间基准与模式复位
            heating_start_ms = current_time;
            sp_initialized = false;         // 让爬坡从当前温度重新开始
            last_mode = Heating_Mode;
            last_mode_target = (float)HeaterTargetTemp;
            // 可选：复位到温提示标志（避免跨次加热误触发）
            // buzzer_state.temperature_reached_played = false;
        }
        // 累加本周期时间并换算为分钟:秒
        unsigned long dt_ms = current_time - timer_last_ms;
        timer_last_ms = current_time;
        timer_acc_ms += (unsigned int)dt_ms;
        while (timer_acc_ms >= 1000) {
            timer_acc_ms -= 1000;
            ++Timer_Seconds;
            if (Timer_Seconds >= 60) {
                Timer_Seconds = 0;
                ++Timer_Minutes;
            }
        }

        // 新增：到时自动停止加热（仅恒温模式生效；HeaterHeatingTime 单位：分钟；0 表示不自动停止）
        if (Heating_Mode == 0 && HeaterHeatingTime > 0 && Timer_Minutes >= HeaterHeatingTime) {
            // 立即停PWM与占空比
            Heater_DutyCycle = 0.0f;
            Heater_Set_PWM(0);
            // 关闭蜂鸣器（若在鸣叫中）
            if (buzzer_state.short_active || buzzer_state.long_active) {
                Buzzer_OFF();
                buzzer_state.short_active = false;
                buzzer_state.long_active  = false;
            }
            // 关闭加热与UI开关
            Heater_Enabled = false;
            lv_obj_clear_state(ui_HeaterSwitch, LV_STATE_CHECKED);
            // 立刻清零计时并复位内部计时状态
            Timer_Minutes = 0;
            Timer_Seconds = 0;
            timer_heating_prev = false;
            timer_acc_ms = 0;
            timer_last_ms = 0;
            heating_start_ms = 0;       // 复位时间基准
            sp_initialized = false;     // 复位爬坡
            last_mode = -1;
            return;
        }

        // 蜂鸣器管理
        if (buzzer_state.short_active || buzzer_state.long_active)
        {
            if (!Buzzer_Enabled)
            {
                // 蜂鸣器被禁用，立即停止
                Buzzer_OFF();
                buzzer_state.short_active = buzzer_state.long_active = false;
            }
            else
            {
                // 检查蜂鸣器超时（短鸣叫优先级更高，时间更短）
                const unsigned long elapsed = current_time - buzzer_state.start_time;
                if (buzzer_state.short_active && elapsed >= 50)
                {
                    Buzzer_OFF();
                    buzzer_state.short_active = false;
                }
                else if (buzzer_state.long_active && elapsed >= 1000)
                {
                    Buzzer_OFF();
                    buzzer_state.long_active = false;
                }
            }
        }

        if (Heater_Status != 0)
        {
            // 传感器异常 - 立即处理
            Heater_DutyCycle = 0.0f;
            Heater_Set_PWM(0);

            // 触发应急蜂鸣器
            if (Buzzer_Enabled && !buzzer_state.long_active && !buzzer_state.short_active && !error_buzzer_started)
            {
                Buzzer_ON();
                buzzer_state.start_time = current_time;
                buzzer_state.long_active = true;
                error_buzzer_started = true;
            }

            // 检查蜂鸣器是否完成长鸣，然后执行关闭操作
            if (error_buzzer_started && !buzzer_state.long_active)
            {
                Heater_Enabled = false;
                lv_obj_clear_state(ui_HeaterSwitch, LV_STATE_CHECKED);
                Buzzer_OFF();
                error_buzzer_started = false;  // 重置状态
                return;
            }
            else if (error_buzzer_started)
            {
                return;  // 等待蜂鸣器完成长鸣
            }
        }

        // 安全检查 - 使用常量避免重复计算
        const int safety_threshold = HeaterTargetTempMax + 10;
        if (Heater_Temp > safety_threshold)
        {
            Heater_DutyCycle = 0.0f;
            Heater_Set_PWM(0);

            // 触发应急蜂鸣器
            if (Buzzer_Enabled && !buzzer_state.long_active && !buzzer_state.short_active && !error_buzzer_started)
            {
                Buzzer_ON();
                buzzer_state.start_time = current_time;
                buzzer_state.long_active = true;
                error_buzzer_started = true;
            }

            // 检查蜂鸣器是否完成长鸣，然后执行关闭操作
            if (error_buzzer_started && !buzzer_state.long_active)
            {
                Heater_Enabled = false;
                lv_obj_clear_state(ui_HeaterSwitch, LV_STATE_CHECKED);
                Buzzer_OFF();
                error_buzzer_started = false;  // 重置状态
                return;
            }
            else if (error_buzzer_started)
            {
                return;  // 等待蜂鸣器完成长鸣
            }
        }

        // 如果没有异常，重置异常蜂鸣器状态
        error_buzzer_started = false;

        // 新增：根据模式获取目标温度（用于误差、爬坡与动态限功率）
        const float mode_target = GetModeTargetTemp(current_time);

        // 温度到达检查（仅在恒温模式对最终目标值判断，曲线模式不提示）
        if (Heating_Mode == 0 &&
            !buzzer_state.temperature_reached_played &&
            !buzzer_state.short_active && !buzzer_state.long_active &&
            abs(Heater_Temp - (int)HeaterTargetTemp) <= 3)
        {
            if (Buzzer_Enabled)
            {
                Buzzer_ON();
                buzzer_state.start_time = current_time;
                buzzer_state.short_active = true;
            }
            buzzer_state.temperature_reached_played = true;
        }

        // 当前误差与温度斜率（℃/ms）
        const float error_now = mode_target - (float)Heater_Temp;
        float temp_slope = 0.0f;
        if (last_temp_time != 0)
        {
            const unsigned long dt = current_time - last_temp_time;
            if (dt > 0) temp_slope = ((float)Heater_Temp - (float)last_temp) / (float)dt;
        }
        last_temp = Heater_Temp;
        last_temp_time = current_time;

        // 积分分离：大误差时关闭积分
        const float Ki_eff = (fabsf(error_now) > integral_sep_band) ? 0.0f : Heater_KI;
        if (Ki_eff != last_Ki_eff)
        {
            Heater_PID.SetTunings(Heater_KP, Ki_eff, Heater_KD);
            last_Ki_eff = Ki_eff;
        }

        // ---------- 新增：设定点爬坡与动态限功率/预测提前收功 ----------
        // 模式变化时重新初始化爬坡起点
        if (!sp_initialized || last_mode != Heating_Mode) {
            sp_ramp = (float)Heater_Temp;
            sp_initialized = true;
            last_mode = Heating_Mode;
            last_mode_target = mode_target;
        } else {
            last_mode_target = mode_target;
        }

        // 设定点按速率爬坡（采样周期200ms -> 0.2s），面向当前“模式目标”
        const float dt_s = 0.2f;
        const float target_f = mode_target;
        if (sp_ramp < target_f) {
            float inc = sp_ramp_rate_cps * dt_s;
            sp_ramp = (sp_ramp + inc > target_f) ? target_f : (sp_ramp + inc);
        } else if (sp_ramp > target_f) {
            float dec = sp_ramp_rate_cps * dt_s;
            sp_ramp = (sp_ramp - dec < target_f) ? target_f : (sp_ramp - dec);
        }

        // 动态输出上限
        float dynamic_max = 100.0f;
        const float eabs_to_sp = fabsf(sp_ramp - (float)Heater_Temp);
        if      (eabs_to_sp <=  5.0f) dynamic_max = 30.0f;
        else if (eabs_to_sp <= 10.0f) dynamic_max = 40.0f;
        else if (eabs_to_sp <= 15.0f) dynamic_max = 80.0f;
        else if (eabs_to_sp <= 20.0f) dynamic_max = 90.0f;
        else if (eabs_to_sp <= 25.0f) dynamic_max = 100.0f;

        // 预测性提前收功
        const float slope_cps = temp_slope * 1000.0f; // ℃/ms -> ℃/s
        const float t_pred = (float)Heater_Temp + slope_cps * pred_horizon_s;
        if (slope_cps > 0.0f && t_pred >= sp_ramp + pred_margin_c) {
            dynamic_max = 0.0f;
        }

        Heater_PID.SetOutputLimits(0.0f, dynamic_max);
        // ---------- 新增结束 ----------

        // PID计算（使用爬坡后的设定点）
        Heater_temp_float = (float)Heater_Temp;
        Heater_target_temp_float = sp_ramp;
        Heater_PID.Compute();

        // 死区与保持
        const bool in_deadband_and_stable =
            (fabsf(error_now) <= deadband) && (fabsf(temp_slope) < 0.0004f); // from 0.0005f，更严格的斜率
        if (in_deadband_and_stable)
        {
            Heater_DutyCycle = duty_hold;
        }
        else
        {
            duty_hold = Heater_DutyCycle;
        }

        // 输出限速与低通滤波
        float desired = Heater_DutyCycle;
        float delta = desired - prev_applied_duty;
        if (delta >  max_duty_step_per_call) delta =  max_duty_step_per_call;
        if (delta < -max_duty_step_per_call) delta = -max_duty_step_per_call;
        float ramped = prev_applied_duty + delta;

        // 低通滤波（进一步平滑）
        float applied = prev_applied_duty + duty_alpha * (ramped - prev_applied_duty);
        if (applied < 0.0f)   applied = 0.0f;
        if (applied > 100.0f) applied = 100.0f;
        prev_applied_duty = applied;
        Heater_DutyCycle = applied;
        // ---------- 高惯性优化结束 ----------

        // 功率控制决策（按平滑后的占空比输出）
        if (Heater_DutyCycle > 0.0f)
        {
            Heater_Set_PWM(Heater_DutyCycle);
        }
        else
        {
            Heater_Set_PWM(0);
        }
    }
    else
    {
        // 关闭加热：立即清零计时并复位状态
        Timer_Minutes = 0;
        Timer_Seconds = 0;
        timer_heating_prev = false;
        timer_acc_ms = 0;
        timer_last_ms = 0;

        // 新增：复位模式/时间基准/爬坡
        heating_start_ms = 0;
        sp_initialized = false;
        last_mode = -1;

        Heater_DutyCycle = 0.0f;
        Heater_Set_PWM(0);
    }
}
