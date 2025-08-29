#include <System_Init.h>

// 定义延时时间 (毫秒)
#define INIT_STEP_DELAY_MS 250

// LVGL 友好的延时函数
void lvgl_friendly_delay(uint32_t ms) {
    if (ms == 0) {
        lv_task_handler(); // 即使延时为0，也至少处理一次任务
        return;
    }
    uint32_t start = millis();
    while (millis() - start < ms) {
        lv_task_handler();
    }
    lv_task_handler(); // 确保在延时结束后再次处理一次
}

// 初始化失败处理函数
void handle_init_failure(const char* error_msg) {
    // 设置进度条为红色错误状态
    if (ui_InitializeStatusBar) {
        lv_obj_set_style_bg_color(ui_InitializeStatusBar, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    }
    
    // 显示错误信息
    if (ui_InitializeStatus) {
        lv_textarea_add_text(ui_InitializeStatus, error_msg);
    }
    
    // 错误提示音
    if(Buzzer_Enabled == true)
    {
        for (int i = 0; i < 3; i++) {
            Buzzer_ON();
            lvgl_friendly_delay(150);
            Buzzer_OFF();
            lvgl_friendly_delay(150);
        }
    }
}

void System_Init()
{
    char status_buffer[128];
    bool init_success = true;

    // 1. display_init
    Heater_GPIO_Init();
    Cooling_FAN_GPIO_Init();
    Buzzer_GPIO_Init();
    display_init();
    if (ui_InitializeStatusBar) lv_bar_set_value(ui_InitializeStatusBar, 8, LV_ANIM_ON);
    if (ui_InitializeStatus) lv_textarea_add_text(ui_InitializeStatus, "显示屏初始化成功. ");
    if (ui_InitializeStatusBar) lv_bar_set_value(ui_InitializeStatusBar, 16, LV_ANIM_ON);
    if (ui_InitializeStatus) lv_textarea_add_text(ui_InitializeStatus, "GPIO 初始化成功.\n");
    if(Buzzer_Enabled == true)
    {
        Buzzer_ON();
        lvgl_friendly_delay(INIT_STEP_DELAY_MS * 2);
        Buzzer_OFF();
    }
    else
    {
        lvgl_friendly_delay(INIT_STEP_DELAY_MS * 2); // 如果蜂鸣器未启用，仍然延时以保持一致性
    }

    // 2. MAX6675_Init
    MAX6675_Init();
    if (ui_InitializeStatusBar)
        lv_bar_set_value(ui_InitializeStatusBar, 24, LV_ANIM_ON);
    if (ui_InitializeStatus)
        lv_textarea_add_text(ui_InitializeStatus, "MAX6675初始化成功.\n");
    lvgl_friendly_delay(INIT_STEP_DELAY_MS);
    uint8_t heater_status = MAX6675_Read_Heater_Status();
    if (ui_InitializeStatusBar)
        lv_bar_set_value(ui_InitializeStatusBar, 32, LV_ANIM_ON);
    bool heater_ok = (heater_status == 0 || heater_status == 4);
    if (ui_InitializeStatus)
    {
        sprintf(status_buffer, "热电偶: %s (状态: %d).\n", heater_ok ? "正常" : "错误", heater_status);
        lv_textarea_add_text(ui_InitializeStatus, status_buffer);
    }

    if (!heater_ok)
    {
        handle_init_failure("错误: 热电偶检测错误!\n");
        return; // 停止初始化
    }
    lvgl_friendly_delay(INIT_STEP_DELAY_MS);

    // 3. TMP102_Init - 关键初始化步骤
    bool tmp102_ok = TMP102_Init();
    if (ui_InitializeStatusBar)
        lv_bar_set_value(ui_InitializeStatusBar, 48, LV_ANIM_ON);
    if (ui_InitializeStatus)
    {
        sprintf(status_buffer, "TMP102初始化: %s.\n", tmp102_ok ? "成功" : "失败");
        lv_textarea_add_text(ui_InitializeStatus, status_buffer);
    }

    if (!tmp102_ok)
    {
        handle_init_failure("错误: TMP102初始化失败!\n");
        return; // 停止初始化
    }
    lvgl_friendly_delay(INIT_STEP_DELAY_MS);

    // 4. encoder_lvgl_init
    encoder_lvgl_init();
    if (ui_InitializeStatusBar) lv_bar_set_value(ui_InitializeStatusBar, 56, LV_ANIM_ON);
    if (ui_InitializeStatus) lv_textarea_add_text(ui_InitializeStatus, "编码器初始化成功.\n");
    lvgl_friendly_delay(INIT_STEP_DELAY_MS);

    // 5. lvgl_group_init
    lvgl_group_init();
    if (ui_InitializeStatusBar) lv_bar_set_value(ui_InitializeStatusBar, 72, LV_ANIM_ON);
    if (ui_InitializeStatus) lv_textarea_add_text(ui_InitializeStatus, "Group初始化成功.\n");
    lvgl_friendly_delay(INIT_STEP_DELAY_MS);

    // 6. Heater_PID_Compute_Init
    Heater_PID_Compute_Init();
    if (ui_InitializeStatusBar) lv_bar_set_value(ui_InitializeStatusBar, 88, LV_ANIM_ON);
    if (ui_InitializeStatus) lv_textarea_add_text(ui_InitializeStatus, "加热器初始化成功.\n");
    lvgl_friendly_delay(INIT_STEP_DELAY_MS);

    // 所有初始化完成
    if (ui_InitializeStatusBar)
        lv_bar_set_value(ui_InitializeStatusBar, 100, LV_ANIM_ON);
    if (ui_InitializeStatus) {
        lv_textarea_add_text(ui_InitializeStatus, "初始化完成, 正在加载主屏幕.");
    }
    // 允许用户看到完成消息，然后再切换屏幕
    if(Buzzer_Enabled == true)
    {
        lvgl_friendly_delay(400);
        Buzzer_ON();
        lvgl_friendly_delay(100);
        Buzzer_OFF();

    }
    else
    {
        lvgl_friendly_delay(500); // 如果蜂鸣器未启用，仍然延时以保持一致性
    }

    // 跳转到主屏幕
    if (ui_MainScreen) {
        // lv_scr_load_anim 的第四个参数是动画开始前的延时，这个延时由 LVGL 内部处理，是无阻塞的。
        // 将其设置为0，因为我们已经在上面用 lvgl_friendly_delay 添加了延时。
        lv_scr_load_anim(ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, true);
    } else {
         if (ui_InitializeStatus) {
             lv_textarea_add_text(ui_InitializeStatus, "错误: 主屏幕为空!\n");
         }
    }
}