#ifndef _EVENT_USER_H
#define _EVENT_USER_H

#include <Arduino.h>
#include <lvgl.h>
#include <ui/ui.h>
#include <variables.h>
#include <lvgl_group.h>

void user_chart_init();
void user_temp_sliders_refresh_max(); // 新增：运行中可调用以重设Temp滑块最大值

#endif // _EVENT_USER_H