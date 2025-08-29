#include <event_main.h>

/**
 * 处理编码器参数编辑功能
 * 基于LVGL编辑状态检测，支持多个参数的编辑
 */
void handle_encoder_parameters_edit()
{
    static int last_position = 0;
    static lv_obj_t *last_edited_obj = NULL;
    static bool was_editing = false; // 跟踪上一次的编辑状态

    // 检查各个可能处于编辑模式的控件
    lv_obj_t *current_edited_obj = NULL;

    // 检查温度编辑状态
    lv_group_t *Heater_group = (lv_group_t *)lv_obj_get_group(ui_HeaterTargetTemp);
    if (Heater_group != NULL &&
        lv_obj_has_state(ui_HeaterTargetTemp, LV_STATE_FOCUSED) &&
        lv_group_get_editing(Heater_group))
    {
        current_edited_obj = ui_HeaterTargetTemp;
    }

    bool is_currently_editing = (current_edited_obj != NULL);

    // 检测从编辑模式退出的情况
    if (was_editing && !is_currently_editing && last_edited_obj != NULL)
    {
        // 刚刚退出编辑模式，执行保存操作
        if (last_edited_obj == ui_HeaterTargetTemp)
        {
            WriteHeater(); // 保存加热器温度
            _ui_label_set_property(ui_HeatingMode, _UI_LABEL_PROPERTY_TEXT, "恒温模式");
            Heating_Mode = 0; // 切换到恒温模式
        }
    }

    // 更新状态
    was_editing = is_currently_editing;

    // 如果没有控件处于编辑状态，保存当前位置并返回
    if (current_edited_obj == NULL)
    {
        last_position = get_encoder_position();
        last_edited_obj = NULL;
        return;
    }

    // 如果编辑对象发生变化，重置编码器位置
    if (current_edited_obj != last_edited_obj)
    {
        last_position = get_encoder_position();
        last_edited_obj = current_edited_obj;
        return;
    }

    // 编码器处理逻辑
    int current_position = get_encoder_position();
    if (current_position == last_position)
        return; // 无变化则退出

    int diff = current_position - last_position;
    last_position = current_position;

    // 根据不同的控件应用不同的编辑逻辑
    switch (current_edited_obj == ui_HeaterTargetTemp ? 1 : 0)
    {

    case 1: // 加热器温度编辑
        handle_parameter_change(&HeaterTargetTemp, diff,
                                0, HeaterTargetTempMax,
                                ui_HeaterTargetTemp, "%03d℃");
        lv_label_set_text_fmt(ui_HeaterCurrentTemp, "%03d℃", HeaterTargetTemp);
        break;
    }
}

/**
 * 处理参数变更的辅助函数
 */
void handle_parameter_change(uint16_t *parameter, int diff,
                             uint16_t min_value, uint16_t max_value,
                             lv_obj_t *ui_object, const char *format)
{
    // 根据diff调整参数值
    if (diff > 0)
    {
        // 增加值，检查是否超过最大值
        if (*parameter < max_value)
        {
            *parameter += diff;
            // 确保不超过最大值
            if (*parameter > max_value)
            {
                *parameter = max_value;
            }
        }
    }
    else if (diff < 0)
    {
        // 减少值，检查是否低于最小值
        if (*parameter > min_value)
        {
            uint16_t abs_diff = (uint16_t)(-diff);
            if (*parameter > min_value + abs_diff)
            {
                *parameter -= abs_diff;
            }
            else
            {
                *parameter = min_value;
            }
        }
    }

    // 更新显示
    char value_str[16];
    snprintf(value_str, sizeof(value_str), format, *parameter);

    // 检查控件是否仍然有效后再更新文本
    if (lv_obj_is_valid(ui_object))
    {
        lv_label_set_text(ui_object, value_str);
    }
}

void ui_event_Setting(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_RELEASED)
    {
        _ui_screen_change(&ui_SystemSettingScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_SystemSettingScreen_screen_init);
        lvgl_group_to_setting();
    }
}

void ui_event_Buzzer( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);

if ( event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      BuzzerOFF( e );
      Buzzer_Enabled = false; // 确保全局变量同步
      
      lv_img_set_src(ui_BuzzerStatus, &ui_img_185202102);
}
if ( event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      BuzzerON( e );
      Buzzer_Enabled = true; // 确保全局变量同步
      lv_img_set_src(ui_BuzzerStatus, &ui_img_1699618864);
}
}

void ui_event_CoolingFan( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);

if ( event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      CoolingOFF( e );
      CoolingFan_Enabled = false; // 确保全局变量同步
      lv_img_set_src(ui_CoolingStatus, &ui_img_1708415670);
}
if ( event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      CoolingON( e );
      CoolingFan_Enabled = true; // 确保全局变量同步
      lv_img_set_src(ui_CoolingStatus, &ui_img_2103744591);
}
}

void ui_event_HeaterSet(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_RELEASED) {
      _ui_screen_change( &ui_HeaterTempSetScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_HeaterTempSetScreen_screen_init);
      lvgl_group_to_heater();
}
}

void ui_event_HeaterSwitch(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);

if ( event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      Heater_Enabled = true; // 确保全局变量同步
}
if ( event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target,LV_STATE_CHECKED)  ) {
      Heater_Enabled = false; // 确保全局变量同步
}
}
