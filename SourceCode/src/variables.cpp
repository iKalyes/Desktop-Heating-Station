#include <variables.h> 

String Version = "Ver: 1.0 - 250831 Release"; // 固件版本号

bool Heater_Enabled = false;

bool Display_Type = true; // true: IPS屏幕；false: TFT屏幕
bool Display_Type_Configured = false; // 是否已配置显示类型

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

uint16_t Temp1 = 200;
uint16_t Temp2 = 200;
uint16_t Temp3 = 200;
uint16_t Temp4 = 200;
uint16_t Temp5 = 200;

uint16_t Time1 = 180;
uint16_t Time2 = 180;
uint16_t Time3 = 180;
uint16_t Time4 = 180;
uint16_t Time5 = 180;
