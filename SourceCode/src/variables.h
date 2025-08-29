#ifndef _VARIABLES_H
#define _VARIABLES_H

#include <Arduino.h>

extern String Version;

extern bool Heater_Enabled;

extern uint16_t HeaterTargetTemp;
extern uint16_t HeaterTargetTempMax;
extern uint16_t HeaterHeatingTime;

extern uint8_t Brightness;

extern uint8_t Heating_Mode;

extern uint8_t Timer_Minutes;
extern uint8_t Timer_Seconds;

extern bool Buzzer_Enabled;
extern bool CoolingFan_Enabled;

extern uint8_t Heater_Status;
extern uint16_t Heater_Temp;
extern float Heater_DutyCycle;

extern float Heater_KP;
extern float Heater_KI;
extern float Heater_KD;

#endif