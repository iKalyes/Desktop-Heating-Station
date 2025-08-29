#include <flash.h>

void WriteFlash() {
    EEPROM.begin(64);

    EEPROM.put(0, HeaterTargetTempMax);
    EEPROM.put(2, HeaterHeatingTime);

    EEPROM.write(4, Brightness);

    EEPROM.write(5, Buzzer_Enabled ? 1 : 0);
    EEPROM.write(6, CoolingFan_Enabled ? 1 : 0);

    EEPROM.end();
}

void WriteHeater()
{
    EEPROM.begin(64);

    EEPROM.put(8, HeaterTargetTemp);

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
        HeaterKp = 125000; // Default value
    }
    EEPROM.get(14, HeaterKi);
    if (HeaterKi == 0)
    {
        HeaterKi = 2000; // Default value
    }
    EEPROM.get(18, HeaterKd);
    if (HeaterKd == 0)
    {
        HeaterKd = 10000; // Default value
    }

    Heater_KP = (float)HeaterKp / 1000.0f;
    Heater_KI = (float)HeaterKi / 1000.0f;
    Heater_KD = (float)HeaterKd / 1000.0f;

    EEPROM.end();
}

void ReadPID() {
    EEPROM.begin(64);

    uint32_t HeaterKp, HeaterKi, HeaterKd;

    EEPROM.get(10, HeaterKp);
    if(HeaterKp == 0) {
        HeaterKp = 70000; // Default value
    }
    EEPROM.get(14, HeaterKi);
    if(HeaterKi == 0) {
        HeaterKi = 600; // Default value
    }
    EEPROM.get(18, HeaterKd);
    if(HeaterKd == 0) {
        HeaterKd = 25000; // Default value
    }

    Heater_KP = (float)HeaterKp / 1000.0f;
    Heater_KI = (float)HeaterKi / 1000.0f;
    Heater_KD = (float)HeaterKd / 1000.0f;

    EEPROM.end();
}

void ClearFlash(){
    EEPROM.begin(32);
    for (int i = 0; i < 32; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.end();
}