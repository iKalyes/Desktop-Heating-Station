#include <lvgl_group.h>

lv_group_t* group;

void lvgl_group_init()
{
    group = lv_group_create();
    lv_group_set_default(group);
    lv_indev_set_group(get_encoder_indev(), group);

    lv_group_add_obj(group, ui_Buzzer);
    lv_group_add_obj(group, ui_CoolingFan);
    lv_group_add_obj(group, ui_Setting);

    lv_group_add_obj(group, ui_HeaterTargetTemp);
    lv_group_add_obj(group, ui_HeaterSwitch);
    lv_group_add_obj(group, ui_HeaterSet);
}

void lvgl_group_to_setting()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_encoder_indev());

    lv_group_add_obj(group, ui_SystemSettingBack);

    lv_group_add_obj(group, ui_PIDSetting);
    lv_group_add_obj(group, ui_ReflowSetting);
    lv_group_add_obj(group, ui_HeaterMaxTemp);
    lv_group_add_obj(group, ui_HeatingTime);

    lv_group_add_obj(group, ui_SystemBrightness);
    lv_group_add_obj(group, ui_SettingSave);
}

void lvgl_group_to_pid_setting()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_encoder_indev());

    lv_group_add_obj(group, ui_PIDSettingBack);

    lv_group_add_obj(group, ui_KPDown);
    lv_group_add_obj(group, ui_HeaterKP);
    lv_group_add_obj(group, ui_KPUp);

    lv_group_add_obj(group, ui_KIDown);
    lv_group_add_obj(group, ui_HeaterKI);
    lv_group_add_obj(group, ui_KIUp);

    lv_group_add_obj(group, ui_KDDown);
    lv_group_add_obj(group, ui_HeaterKD);
    lv_group_add_obj(group, ui_KDUp);
}

void lvgl_group_to_heater()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_encoder_indev());
    
    lv_group_add_obj(group, ui_HeaterTempSetBack);
    lv_group_add_obj(group, ui_RSS);
    lv_group_add_obj(group, ui_RTS);
    lv_group_add_obj(group, ui_USER);
}

void lvgl_group_to_user()
{
    lv_group_remove_all_objs(group);

    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_encoder_indev());

    lv_group_add_obj(group, ui_UserSettingBack);
    lv_group_add_obj(group, ui_Temp1);
    lv_group_add_obj(group, ui_Time1);
    lv_group_add_obj(group, ui_Temp2);
    lv_group_add_obj(group, ui_Time2);
    lv_group_add_obj(group, ui_Temp3);
    lv_group_add_obj(group, ui_Time3);
    lv_group_add_obj(group, ui_Temp4);
    lv_group_add_obj(group, ui_Time4);
    lv_group_add_obj(group, ui_Temp5);
    lv_group_add_obj(group, ui_Time5);
}

void lvgl_group_to_main()
{
    lv_group_remove_all_objs(group);
    
    // 等待输入设备释放后再添加对象到组
    lv_indev_wait_release(get_encoder_indev());

    lv_group_add_obj(group, ui_Buzzer);
    lv_group_add_obj(group, ui_CoolingFan);
    lv_group_add_obj(group, ui_Setting);

    lv_group_add_obj(group, ui_HeaterTargetTemp);
    lv_group_add_obj(group, ui_HeaterSwitch);
    lv_group_add_obj(group, ui_HeaterSet);
}