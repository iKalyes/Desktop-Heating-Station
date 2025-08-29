#ifndef _MAX6675_SERVICE_H
#define _MAX6675_SERVICE_H

#include <Arduino.h>
#include <MAX6675.h>
#include <lvgl.h>
#include <ui/ui.h>
#include <variables.h>

#define MAX6675_SO_PIN 24
#define MAX6675_CS_PIN 25
#define MAX6675_CLK_PIN 26

void MAX6675_Init();
void MAX6675_Task(lv_timer_t *timer);

uint8_t MAX6675_Read_Heater_Status();
float MAX6675_Read_Heater_Temperature();

#endif // _MAX6675_SERVICE_H