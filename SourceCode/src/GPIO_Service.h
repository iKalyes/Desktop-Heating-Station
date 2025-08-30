#ifndef _GPIO_SERVICE_H
#define _GPIO_SERVICE_H

#include <Arduino.h>
#include <variables.h>
#include <hardware/pwm.h>

#define Heater_PWM 14
#define Cooling_FAN_PWM 20
#define Buzzer_PWM 18

void Heater_GPIO_Init();
void Heater_Set_PWM(float value);

void Cooling_FAN_GPIO_Init();
void Cooling_FAN_Set_PWM(float value);

void Buzzer_GPIO_Init();
void Buzzer_Set_PWM(float value);
void Buzzer_ON();
void Buzzer_OFF();

#endif