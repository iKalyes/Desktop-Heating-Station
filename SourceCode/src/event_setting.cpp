#include <event_setting.h>

void ui_event_SystemSettingBack( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_RELEASED) {
      _ui_screen_change( &ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_MainScreen_screen_init);
      lvgl_group_to_main();
}
}

void ui_event_PIDSetting( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_RELEASED) {
      _ui_screen_change( &ui_PIDSettingScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_PIDSettingScreen_screen_init);
      lvgl_group_to_pid_setting();
}
}

void ui_event_HeaterMaxTemp(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);

if ( event_code == LV_EVENT_VALUE_CHANGED) {
    _ui_slider_set_text_value(ui_TextHeaterMaxTemp, target, "", "℃");
    HeaterTargetTempMax = lv_slider_get_value(target);
}
}

void ui_event_HeatingTime(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);

if ( event_code == LV_EVENT_VALUE_CHANGED) {
    _ui_slider_set_text_value(ui_TextHeatingTime, target, "", "Min");
    HeaterHeatingTime = lv_slider_get_value(target);
}
}

void ui_event_SystemBrightness( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);lv_obj_t * target = lv_event_get_target(e);

if ( event_code == LV_EVENT_VALUE_CHANGED) {
      _ui_slider_set_text_value( ui_TextBrightness, target, "", "%");
        Brightness = lv_slider_get_value( target );
        backlight_refresh();
}
}

void ui_event_SettingSave( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

if ( event_code == LV_EVENT_PRESSED) {
      SaveConfig( e );
      WriteFlash();
}
}
