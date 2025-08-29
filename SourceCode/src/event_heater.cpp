#include <event_heater.h>

char tempDisplay[4] = "---";  // 初始化为"---"，仅存储数字部分
uint8_t inputPos = 0;        // 输入位置指针

// 更新显示函数，添加摄氏度单位
void updateDisplay() {
    tempDisplay[3] = '\0';
    // 添加摄氏度单位
    lv_label_set_text_fmt(ui_HeaterSetTemp, "%s℃", tempDisplay);
}

// 数字输入处理函数 - 修改为支持循环输入
void handleNumberInput(char num) {
    // 如果已经输入了3位数字，则重新从第一位开始输入
    if (inputPos >= 3) {
        // 重置所有位置为'-'
        for (int i = 0; i < 3; i++) {
            tempDisplay[i] = '-';
        }
        // 重置输入位置指针
        inputPos = 0;
    }
    
    // 输入新数字
    tempDisplay[inputPos] = num;
    
    // 确保后面的位置保持为'-'
    for (int i = inputPos + 1; i < 3; i++) {
        tempDisplay[i] = '-';
    }
    
    inputPos++;
    updateDisplay();
}

void ui_event_Confirm(lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_RELEASED) {
        // 检查是否输入了完整的三位数字
        bool isComplete = true;
        for (int i = 0; i < 3; i++) {
            if (tempDisplay[i] == '-') {
                isComplete = false;
                break;
            }
        }
        
        // 如果输入不完整，清空当前输入值，重新开始输入
        if (!isComplete) {
            // 重置所有位置为'-'
            for (int i = 0; i < 3; i++) {
                tempDisplay[i] = '-';
            }
            // 重置输入位置指针
            inputPos = 0;
            updateDisplay();
            return;
        }
        
        // 添加输入验证
        if (!tempDisplay || strlen(tempDisplay) > 3) {
            return;
        }
        
        // 使用更安全的转换方式
        char *endptr;
        long temp_val = strtol(tempDisplay, &endptr, 10);
        // 验证转换结果
        if (endptr == tempDisplay || *endptr != '\0') {
            return;
        }
        
        // 保存旧的温度值，用于判断是否需要保存
        uint16_t old_temp = HeaterTargetTemp;
        bool value_changed = false;

        HeaterTargetTemp = (int)temp_val;

        if (HeaterTargetTemp == 0) {
            lv_label_set_text(ui_HeaterTargetTemp, "000℃");
            value_changed = (old_temp != HeaterTargetTemp);
        }
        else if(HeaterTargetTemp > HeaterTargetTempMax) {
            HeaterTargetTemp = HeaterTargetTempMax;
            lv_label_set_text_fmt(ui_HeaterTargetTemp, "%.3d℃", HeaterTargetTemp);
            lv_label_set_text_fmt(ui_HeaterCurrentTemp, "%.3d℃", HeaterTargetTemp);
            lv_label_set_text(ui_HeaterSetTemp, "---℃");
            value_changed = (old_temp != HeaterTargetTemp);
        }
        else if(HeaterTargetTemp < 0) {
            HeaterTargetTemp = 0;
            lv_label_set_text_fmt(ui_HeaterTargetTemp, "%.3d℃", HeaterTargetTemp);
            lv_label_set_text_fmt(ui_HeaterCurrentTemp, "%.3d℃", HeaterTargetTemp);
            lv_label_set_text(ui_HeaterSetTemp, "---℃");
            value_changed = (old_temp != HeaterTargetTemp);
        }
        else {
            lv_label_set_text_fmt(ui_HeaterTargetTemp, "%.3d℃", HeaterTargetTemp);
            value_changed = (old_temp != HeaterTargetTemp);
        }
        
        // 只有当数值实际发生变化时才保存到EEPROM
        if (value_changed) {
            WriteHeater(); // 保存烙铁温度到EEPROM
        }
        
        // 确认后重置输入状态到初始状态
        for (int i = 0; i < 3; i++) {
            tempDisplay[i] = '-';
        }
        inputPos = 0;
        updateDisplay();
        
        // 切换回主屏幕
        _ui_screen_change(&ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_MainScreen_screen_init);
        _ui_label_set_property(ui_HeatingMode, _UI_LABEL_PROPERTY_TEXT, "恒温模式");
        Heating_Mode = 0; // 切换到恒温模式
        lvgl_group_to_main();
    }
}

void ui_event_Delete( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        for(int i = 0; i < 3; i++) {
            tempDisplay[i] = '-';
        }
        inputPos = 0;
        HeaterTargetTemp = 0;
        updateDisplay();
    }
}

void ui_event_Num0( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('0');
    }
}

void ui_event_Num1( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('1');
    }
}

void ui_event_Num2( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('2');
    }
}

void ui_event_Num3( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('3');
    }
}

void ui_event_Num4( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('4');
    }
}

void ui_event_Num5( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('5');
    }
}

void ui_event_Num6( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('6');
    }
}

void ui_event_Num7( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('7');
    }
}

void ui_event_Num8( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('8');
    }
}

void ui_event_Num9( lv_event_t * e) {
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_PRESSED) {
        handleNumberInput('9');
    }
}

void ui_event_HeaterTempSetBack(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_RELEASED) {
        // 重置所有位置为'-'
        for (int i = 0; i < 3; i++) {
            tempDisplay[i] = '-';
        }
        // 重置输入位置指针
        inputPos = 0;
        // 更新显示
        updateDisplay();
        
        // 切换回主屏幕
        _ui_screen_change(&ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_MainScreen_screen_init);
        lvgl_group_to_main();
    }
}

void ui_event_RSS(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_RELEASED)
    {
        _ui_screen_change(&ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_MainScreen_screen_init);
        _ui_label_set_property(ui_HeatingMode, _UI_LABEL_PROPERTY_TEXT, "RSS曲线模式");
        Heating_Mode = 1; // 切换到RSS曲线模式
        lv_label_set_text(ui_HeaterTargetTemp, "NONE");
        lvgl_group_to_main();
    }
}

void ui_event_RTS(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_RELEASED)
    {
        _ui_screen_change(&ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_MainScreen_screen_init);
        _ui_label_set_property(ui_HeatingMode, _UI_LABEL_PROPERTY_TEXT, "RTS曲线模式");
        Heating_Mode = 2; // 切换到RTS曲线模式
        lv_label_set_text(ui_HeaterTargetTemp, "NONE");
        lvgl_group_to_main();
    }
}

void ui_event_USER(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_RELEASED)
    {
        _ui_screen_change(&ui_MainScreen, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_MainScreen_screen_init);
        _ui_label_set_property(ui_HeatingMode, _UI_LABEL_PROPERTY_TEXT, "自定曲线模式");
        Heating_Mode = 3; // 切换到自定曲线模式
        lv_label_set_text(ui_HeaterTargetTemp, "NONE");
        lvgl_group_to_main();
    }
}