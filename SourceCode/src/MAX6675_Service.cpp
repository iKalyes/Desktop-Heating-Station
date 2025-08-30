#include <MAX6675_Service.h>

MAX6675 sensorheater(MAX6675_SO_PIN, MAX6675_CS_PIN, MAX6675_CLK_PIN, &SPI1, 1000000);
lv_timer_t* MAX6675_Display_Timer;
static lv_chart_series_t * temp_series = NULL;

// 新增：主屏对象与工具函数（仅当主屏激活时才刷新图表）
extern lv_obj_t *ui_MainScreen;
static inline bool is_main_screen_active() {
    return lv_scr_act() == ui_MainScreen;
}

uint16_t Clock_Seconds = 0;

void MAX6675_Init()
{
    sensorheater.begin();
    
    // 初始化图表系列
    temp_series = lv_chart_add_series(ui_HeaterChart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    
    MAX6675_Display_Timer = lv_timer_create(MAX6675_Task, 200, NULL);
}

void MAX6675_Task(lv_timer_t *timer)
{
    static int max_temp_range = 100;
    static bool heating_started = false;
    static uint16_t temp_point_count = 0;
    
    Heater_Status = MAX6675_Read_Heater_Status();
    Heater_Temp = (int)MAX6675_Read_Heater_Temperature();

    if (Heater_Status == 0)
    {
        // 正常显示温度
        lv_label_set_text_fmt(ui_HeaterTemp, "%.3d", Heater_Temp);
        lv_label_set_text_fmt(ui_HeaterCurrentTemp, "%.3d℃", Heater_Temp);
        lv_label_set_text_fmt(ui_Clock, "%02d:%02d", Timer_Minutes, Timer_Seconds);
        Clock_Seconds = Timer_Seconds + Timer_Minutes * 60;
        lv_label_set_text_fmt(ui_Second, "%05dS", Clock_Seconds);
        lv_bar_set_value(ui_BarHeaterDuty, Heater_DutyCycle, LV_ANIM_ON);

        // 更新图表数据（仅当主屏激活时才执行）
        if (temp_series && is_main_screen_active()) {
            if (!Heater_Enabled) {
                // 重置加热状态
                if (heating_started) {
                    heating_started = false;
                    temp_point_count = 0;
                    lv_chart_set_point_count(ui_HeaterChart, 128);
                }
                
                // 未启用：动态范围调整，最小值固定为0，最大值可双向调整
                static int recent_max_temp = 0;
                static unsigned long last_temp_update = 0;
                unsigned long current_time = millis();
                
                if (Heater_Temp > recent_max_temp) {
                    recent_max_temp = Heater_Temp;
                    last_temp_update = current_time;
                }

                // 每5秒检查一次是否需要降低最大值
                if (current_time - last_temp_update > 5000) {
                    recent_max_temp = Heater_Temp;
                    last_temp_update = current_time;
                }
                
                int new_max_range = ((recent_max_temp / 10) + 1) * 10 + 20; // 向上取整并留余量
                if (new_max_range < 50) new_max_range = 50; // 最小显示范围
                if (new_max_range > 400) new_max_range = 400;
                
                if (new_max_range != max_temp_range) {
                    max_temp_range = new_max_range;
                    lv_chart_set_range(ui_HeaterChart, LV_CHART_AXIS_PRIMARY_Y, 0, max_temp_range);
                }
                
                // 流动显示模式
                lv_chart_set_next_value(ui_HeaterChart, temp_series, Heater_Temp);
            } else {
                // 已启用：动态范围调整，只能增大不能缩小
                static int heating_max_range = 100;  // 加热过程中的最大范围
                
                if (!heating_started) {
                    // 清除之前的数据，开始新的加热过程记录
                    lv_chart_remove_series(ui_HeaterChart, temp_series);
                    temp_series = lv_chart_add_series(ui_HeaterChart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
                    temp_point_count = 0;
                    // 设置初始点数
                    lv_chart_set_point_count(ui_HeaterChart, 200);
                    // 重置加热过程的最大范围为当前范围
                    heating_max_range = max_temp_range;
                    heating_started = true;
                }
                
                // 动态调整Y轴范围 - 只能增大
                int new_max_range = ((Heater_Temp / 10) + 1) * 10 + 20; // 向上取整并留余量
                if (new_max_range > 400) new_max_range = 400;
                
                // 只有当新范围大于当前加热过程的最大范围时才更新
                if (new_max_range > heating_max_range) {
                    heating_max_range = new_max_range;
                    max_temp_range = heating_max_range;
                    lv_chart_set_range(ui_HeaterChart, LV_CHART_AXIS_PRIMARY_Y, 0, max_temp_range);
                }
                
                // 温度数据累积显示 - 支持动态扩展
                temp_point_count++;
                
                // 当超过当前点数时，扩展图表点数
                uint16_t current_point_count = lv_chart_get_point_count(ui_HeaterChart);
                if (temp_point_count > current_point_count) {
                    // 每次扩展100个点
                    lv_chart_set_point_count(ui_HeaterChart, current_point_count + 100);
                }
                
                // 设置温度数据点
                lv_coord_t * temp_array = lv_chart_get_y_array(ui_HeaterChart, temp_series);
                if (temp_array && temp_point_count <= lv_chart_get_point_count(ui_HeaterChart)) {
                    temp_array[temp_point_count - 1] = Heater_Temp;
                    lv_chart_refresh(ui_HeaterChart);
                }
            }
        }
    }
    else
    {
        // 重置状态
        heating_started = false;
        temp_point_count = 0;
        
        // 错误时显示ERR文本
        lv_label_set_text(ui_HeaterTemp, "ERR");
        lv_label_set_text(ui_HeaterCurrentTemp, "ERROR℃");
        lv_label_set_text(ui_Clock, "00:00");
        lv_label_set_text(ui_Second, "00000S");
        lv_bar_set_value(ui_BarHeaterDuty, 0, LV_ANIM_ON);
        
        // 完全清空图表显示内容（仅当主屏激活时才执行）
        if (temp_series && is_main_screen_active()) {
            lv_chart_set_point_count(ui_HeaterChart, 0);
            lv_chart_remove_series(ui_HeaterChart, temp_series);
            
            // 重新创建空的系列
            temp_series = lv_chart_add_series(ui_HeaterChart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
            
            // 强制刷新图表显示
            lv_obj_invalidate(ui_HeaterChart);
        }
    }
} 

uint8_t MAX6675_Read_Heater_Status()
{
    return sensorheater.read();
}

float MAX6675_Read_Heater_Temperature()
{
    return sensorheater.getTemperature();
}