#include <event_pid.h>

void ui_event_PIDSettingBack( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_RELEASED) {
      _ui_screen_change( &ui_SystemSettingScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_SystemSettingScreen_screen_init);
        lvgl_group_to_setting();
        WritePID();
        Heater_PID_Update_Tunings(Heater_KP, Heater_KI, Heater_KD);
}
}

void ui_event_HeaterKP( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_VALUE_CHANGED) {
    Heater_PID_Update();
}
}

void ui_event_HeaterKI( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_VALUE_CHANGED) {
    Heater_PID_Update();
}
}

void ui_event_HeaterKD( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_VALUE_CHANGED) {
    Heater_PID_Update();
}
}

void ui_event_KPDown( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_PRESSED) {
    lv_spinbox_decrement(ui_HeaterKP);
}
}

void ui_event_KPUp( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_PRESSED) {
    lv_spinbox_increment(ui_HeaterKP);
}
}

void ui_event_KIDown( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_PRESSED) {
    lv_spinbox_decrement(ui_HeaterKI);
}
}

void ui_event_KIUp( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_PRESSED) {
    lv_spinbox_increment(ui_HeaterKI);
}
}

void ui_event_KDDown( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_PRESSED) {
    lv_spinbox_decrement(ui_HeaterKD);
}
}

void ui_event_KDUp( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_PRESSED) {
    lv_spinbox_increment(ui_HeaterKD);
}
}

void Heater_PID_Update()
{
    Heater_KP = lv_spinbox_get_value(ui_HeaterKP) / 1000.0f;
    Heater_KI = lv_spinbox_get_value(ui_HeaterKI) / 1000.0f;
    Heater_KD = lv_spinbox_get_value(ui_HeaterKD) / 1000.0f;
}

void Heater_PID_Init()
{
    uint32_t HeaterKp = (uint32_t)(Heater_KP * 1000.0f + 0.5f);
    uint32_t HeaterKi = (uint32_t)(Heater_KI * 1000.0f + 0.5f);
    uint32_t HeaterKd = (uint32_t)(Heater_KD * 1000.0f + 0.5f);

    lv_spinbox_set_value(ui_HeaterKP, HeaterKp);
    lv_spinbox_set_value(ui_HeaterKI, HeaterKi);
    lv_spinbox_set_value(ui_HeaterKD, HeaterKd);
}
