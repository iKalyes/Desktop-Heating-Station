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

// 新增: 屏幕类型选择回调与状态
static volatile bool s_disp_type_selected = false;

static void on_select_ips(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    lv_obj_t* modal = (lv_obj_t*)lv_event_get_user_data(e);
    // 设置并保存选择
    Display_Type = true;
    Display_Type_Configured = true;
    WriteDisplayType();
    ChangeDisplayType(Display_Type);
    if (modal) lv_obj_del(modal);
    s_disp_type_selected = true;
}

static void on_select_tft(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    lv_obj_t* modal = (lv_obj_t*)lv_event_get_user_data(e);
    // 设置并保存选择
    Display_Type = false;
    Display_Type_Configured = true;
    WriteDisplayType();
    ChangeDisplayType(Display_Type);
    if (modal) lv_obj_del(modal);
    s_disp_type_selected = true;
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

    // 新增: 如果未配置屏幕类型，则弹窗选择；已配置则直接应用
    if (!Display_Type_Configured) {
        lv_obj_t* modal = lv_obj_create(lv_scr_act());
        // 缩小窗口尺寸
        lv_obj_set_size(modal, 300, 160);
        lv_obj_center(modal);
        lv_obj_set_style_radius(modal, 8, LV_PART_MAIN);
        lv_obj_set_style_pad_all(modal, 12, LV_PART_MAIN);
        lv_obj_clear_flag(modal, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        // 背景保持半透明深色，便于对比
        lv_obj_set_style_bg_color(modal, lv_color_hex(0x202020), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(modal, LV_OPA_90, LV_PART_MAIN);

        // 头部容器：放置图标与标题（横向排列，居中）
        lv_obj_t* header = lv_obj_create(modal);
        lv_obj_set_size(header, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(header, LV_OPA_0, LV_PART_MAIN);
        lv_obj_set_style_border_opa(header, LV_OPA_0, LV_PART_MAIN);
        lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(header, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 6);

        // 图标（黄色）
        lv_obj_t* icon = lv_img_create(header);
        lv_img_set_src(icon, &ui_img_921610142);
        lv_obj_set_style_img_recolor_opa(icon, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_img_recolor(icon, lv_color_hex(0xFFFF00), LV_PART_MAIN);

        // 标题（黄色）
        lv_obj_t* label = lv_label_create(header);
        lv_label_set_text(label, "请选择屏幕类型");
        lv_obj_set_style_text_font(label, &ui_font_ASCII32, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFF00), LV_PART_MAIN);

        // IPS 按钮
        lv_obj_t* btn_ips = lv_btn_create(modal);
        lv_obj_set_size(btn_ips, 120, 48);
        lv_obj_align(btn_ips, LV_ALIGN_BOTTOM_LEFT, 10, -10);
        lv_obj_add_event_cb(btn_ips, on_select_ips, LV_EVENT_CLICKED, modal);
        lv_obj_t* lbl_ips = lv_label_create(btn_ips);
        lv_label_set_text(lbl_ips, "IPS屏幕");
        lv_obj_set_style_text_font(lbl_ips, &ui_font_ASCII24, LV_PART_MAIN);
        lv_obj_center(lbl_ips);

        // TFT 按钮
        lv_obj_t* btn_tft = lv_btn_create(modal);
        lv_obj_set_size(btn_tft, 120, 48);
        lv_obj_align(btn_tft, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
        lv_obj_add_event_cb(btn_tft, on_select_tft, LV_EVENT_CLICKED, modal);
        lv_obj_t* lbl_tft = lv_label_create(btn_tft);
        lv_label_set_text(lbl_tft, "TFT屏幕");
        lv_obj_set_style_text_font(lbl_tft, &ui_font_ASCII24, LV_PART_MAIN);
        lv_obj_center(lbl_tft);

        // 等待用户选择，期间持续处理 LVGL 任务
        s_disp_type_selected = false;
        while (!s_disp_type_selected) {
            lvgl_friendly_delay(20);
        }
    } else {
        // 已配置则直接应用
        ChangeDisplayType(Display_Type);
    }

    if (ui_InitializeStatusBar) lv_bar_set_value(ui_InitializeStatusBar, 8, LV_ANIM_ON);
    if (ui_InitializeStatus) lv_textarea_add_text(ui_InitializeStatus, "显示屏初始化成功.\n");
    if (ui_InitializeStatusBar) lv_bar_set_value(ui_InitializeStatusBar, 16, LV_ANIM_ON);
    if (ui_InitializeStatus) lv_textarea_add_text(ui_InitializeStatus, "GPIO初始化成功.\n");
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
    if (ui_InitializeStatus) lv_textarea_add_text(ui_InitializeStatus, "控件组初始化成功.\n");
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
        lv_textarea_add_text(ui_InitializeStatus, "初始化完成正在加载主屏幕.");
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