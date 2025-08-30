#include <flash.h>

void WriteFlash() {
    EEPROM.begin(64);

    EEPROM.put(0, HeaterTargetTempMax);
    EEPROM.put(2, HeaterHeatingTime);

    EEPROM.write(4, Brightness);

    EEPROM.write(5, Buzzer_Enabled ? 1 : 0);
    EEPROM.write(6, CoolingFan_Enabled ? 1 : 0);

    EEPROM.put(22, Temp1);
    EEPROM.put(24, Temp2);
    EEPROM.put(26, Temp3);
    EEPROM.put(28, Temp4);
    EEPROM.put(30, Temp5);

    EEPROM.put(32, Time1);
    EEPROM.put(34, Time2);
    EEPROM.put(36, Time3);
    EEPROM.put(38, Time4);
    EEPROM.put(40, Time5);

    EEPROM.end();
}

void WriteHeater()
{
    EEPROM.begin(64);

    EEPROM.put(8, HeaterTargetTemp);

    EEPROM.end();
}

void WriteDisplayType()
{
    EEPROM.begin(64);

    EEPROM.write(42, Display_Type ? 1 : 0); // true: IPS屏幕；false: TFT屏幕
    EEPROM.write(43, Display_Type_Configured ? 1 : 0); // true: 已配置；false: 未配置

    EEPROM.end();
}

void WritePID() {
    EEPROM.begin(64);

    uint32_t HeaterKp = (uint32_t)(Heater_KP * 1000.0f + 0.5f);
    uint32_t HeaterKi = (uint32_t)(Heater_KI * 1000.0f + 0.5f);
    uint32_t HeaterKd = (uint32_t)(Heater_KD * 1000.0f + 0.5f);

    EEPROM.put(10, HeaterKp);
    EEPROM.put(14, HeaterKi);
    EEPROM.put(18, HeaterKd);

    EEPROM.end();
}

//—————————————————————————————————————————//
// Read from flash memory

void ReadFlash() {
    EEPROM.begin(64);

    EEPROM.get(0, HeaterTargetTempMax);
    if(HeaterTargetTempMax > 350 || HeaterTargetTempMax < 100) {
        HeaterTargetTempMax = 200; // Ensure maximum value
    }
    EEPROM.get(2, HeaterHeatingTime);
    if(HeaterHeatingTime < 0 || HeaterHeatingTime > 60) {
        HeaterHeatingTime = 0; // Default value
    }
    Brightness = EEPROM.read(4);
    if(Brightness < 10 || Brightness > 100) {
        Brightness = 50; // Default value
    }
    EEPROM.get(8, HeaterTargetTemp);
    if(HeaterTargetTemp < 0 || HeaterTargetTemp > HeaterTargetTempMax) {
        HeaterTargetTemp = 100; // Default value
    }
    Buzzer_Enabled = EEPROM.read(5);
    if(Buzzer_Enabled > 1) {
        Buzzer_Enabled = true; // Default value
    } else {
        Buzzer_Enabled = (Buzzer_Enabled == 1);
    }

    CoolingFan_Enabled = EEPROM.read(6);
    if(CoolingFan_Enabled > 1) {
        CoolingFan_Enabled = true; // Default value
    } else {
        CoolingFan_Enabled = (CoolingFan_Enabled == 1);
    }
    
    uint32_t HeaterKp, HeaterKi, HeaterKd;
    
    EEPROM.get(10, HeaterKp);
    if (HeaterKp == 0)
    {
        HeaterKp = 70000; // Default value
    }
    EEPROM.get(14, HeaterKi);
    if (HeaterKi == 0)
    {
        HeaterKi = 600; // Default value
    }
    EEPROM.get(18, HeaterKd);
    if (HeaterKd == 0)
    {
        HeaterKd = 25000; // Default value
    }

    Heater_KP = (float)HeaterKp / 1000.0f;
    Heater_KI = (float)HeaterKi / 1000.0f;
    Heater_KD = (float)HeaterKd / 1000.0f;

    EEPROM.get(22, Temp1);
    if(Temp1 < 100 || Temp1 > 350) {
        Temp1 = 200; // Default value
    }
    EEPROM.get(24, Temp2);
    if(Temp2 < 100 || Temp2 > 350) {
        Temp2 = 200; // Default value
    }
    EEPROM.get(26, Temp3);
    if(Temp3 < 100 || Temp3 > 350) {
        Temp3 = 200; // Default value
    }
    EEPROM.get(28, Temp4);
    if(Temp4 < 100 || Temp4 > 350) {
        Temp4 = 200; // Default value
    }
    EEPROM.get(30, Temp5);
    if(Temp5 < 100 || Temp5 > 350) {
        Temp5 = 200; // Default value
    }

    EEPROM.get(32, Time1);
    if(Time1 < 0 || Time1 > 300) {
        Time1 = 180; // Default value
    }
    EEPROM.get(34, Time2);
    if(Time2 < 0 || Time2 > 300) {
        Time2 = 180; // Default value
    }
    EEPROM.get(36, Time3);
    if(Time3 < 0 || Time3 > 300) {
        Time3 = 180; // Default value
    }
    EEPROM.get(38, Time4);
    if(Time4 < 0 || Time4 > 300) {
        Time4 = 180; // Default value
    }
    EEPROM.get(40, Time5);
    if(Time5 < 0 || Time5 > 300) {
        Time5 = 180; // Default value
    }

    EEPROM.get(42, Display_Type);
    EEPROM.get(43, Display_Type_Configured);

    EEPROM.end();
}

void ClearFlash(){
    EEPROM.begin(64);
    for (int i = 0; i < 64; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.end();
}