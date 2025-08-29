#include <variables.h> 

String Version = "Ver: 1.0 - 250829 Release"; // 固件版本号

bool Heater_Enabled = false;

uint16_t HeaterTargetTemp = 100;
uint16_t HeaterTargetTempMax = 200; //Max 350
uint16_t HeaterHeatingTime = 0; // Minutes 0: False

uint8_t Brightness;

uint8_t Heating_Mode = 0; // 0: 恒温模式 1: RSS曲线模式 2: RTS曲线模式 3: 自定曲线模式

uint8_t Timer_Minutes = 0;
uint8_t Timer_Seconds = 0;

bool Buzzer_Enabled = true;
bool CoolingFan_Enabled = true;

uint8_t Heater_Status;
uint16_t Heater_Temp;
float Heater_DutyCycle;

float Heater_KP = 70.0f;
float Heater_KI = 0.6f;
float Heater_KD = 25.0f;
