#include <display.h>

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */
FT6336U touch_6336;

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp_drv );
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
  // 更新触摸数据
  touch_6336.scan();
  
  // 检查是否有触摸点
  if (touch_6336.touchPoint.touch_count > 0)
  {
    // 找到第一个活动的触摸点
    for (uint8_t i = 0; i < 2; i++)
    {
      if (touch_6336.touchPoint.tp[i].status != release)
      {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_6336.touchPoint.tp[i].x;
        data->point.y = touch_6336.touchPoint.tp[i].y;
        return;
      }
    }
  }
  
  // 没有触摸点活动
  data->state = LV_INDEV_STATE_REL;
}

void backlight_init()
{
  // 第一个 PWM 引脚设置 (例如 GPIO 6)
  gpio_set_function(Screen_BKLT, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(Screen_BKLT);
  uint channel = pwm_gpio_to_channel(Screen_BKLT);
  
  // 为第一个引脚设置时钟分频和计数范围（决定频率）
  pwm_set_clkdiv(slice_num, 250.0);  // 分频器
  pwm_set_wrap(slice_num, 1000);     // 最大计数值 (分辨率)
  
  // 确定目标亮度值
  uint16_t target_brightness;
  if(Brightness == 0)
  {
    target_brightness = 500;  // 默认亮度
  }
  else
  {
    target_brightness = Brightness * 10;  // 设定亮度
  }
  
  pwm_set_enabled(slice_num, true);
  
  // 渐进增加亮度，从0开始到目标值
  const uint16_t step = 10;  // 每次增加的步长
  const uint32_t delay_ms = 1;  // 每步之间的延时（毫秒）
  
  for(uint16_t current_level = 0; current_level <= target_brightness; current_level += step)
  {
    // 确保不超过目标值
    if(current_level > target_brightness)
    {
      current_level = target_brightness;
    }
    
    pwm_set_chan_level(slice_num, channel, current_level);
    delay(delay_ms);  // 延时
    
    // 如果已达到目标值，退出循环
    if(current_level == target_brightness)
    {
      break;
    }
  }
}

void backlight_refresh()
{
  uint slice_num = pwm_gpio_to_slice_num(Screen_BKLT);
  uint channel = pwm_gpio_to_channel(Screen_BKLT);
  pwm_set_chan_level(slice_num, channel, Brightness * 10);  // 占空比
}

void display_init()
{
  tft.init(); /* TFT init */
  tft.initDMA();
  tft.setRotation(3); /* Landscape orientation, flipped */
  lv_init();
  /*Set the touchscreen calibration data,
   the actual data for your display can be acquired using
   the Generic -> Touch_calibrate example from the TFT_eSPI library*/
  touch_6336.begin(Wire);
  lv_disp_draw_buf_init(&draw_buf, buf_1, buf_2, screenWidth * screenHeight / 5);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ui_init();
  lv_label_set_text(ui_FWVersion, Version.c_str()); // 设置固件版本信息
  int display_time_start = millis();
  while (display_time_start + 200 > millis())
  {
    lv_task_handler(); // 处理 LVGL 任务
  }
  backlight_init();
  MainScreen_init();
  SystemSettingScreen_init();
  PIDSettingScreen_init();
  Heater_PID_Init();
  UserSettingScreen_init();
}

void lvgl_task_handler()
{
  lv_task_handler();
  handle_encoder_parameters_edit();
}

void ChangeDisplayType(bool Type)
{
  if (Type) {
      tft.invertDisplay(true); // true:IPS屏幕
  } else {
      tft.invertDisplay(false); // false:TFT屏幕
  }
}

void MainScreen_init()
{
  lv_label_set_text_fmt(ui_HeaterTargetTemp, "%.3d℃", HeaterTargetTemp);

  if(Buzzer_Enabled == true) {
      lv_obj_clear_state(ui_Buzzer, LV_STATE_CHECKED);
      lv_img_set_src(ui_BuzzerStatus, &ui_img_1699618864);
  } else {
      lv_obj_add_state(ui_Buzzer, LV_STATE_CHECKED);
      lv_img_set_src(ui_BuzzerStatus, &ui_img_185202102);
  }

  if(CoolingFan_Enabled == true) {
      lv_obj_clear_state(ui_CoolingFan, LV_STATE_CHECKED);
      lv_img_set_src(ui_CoolingStatus, &ui_img_2103744591);
  } else {
      lv_obj_add_state(ui_CoolingFan, LV_STATE_CHECKED);
      lv_img_set_src(ui_CoolingStatus, &ui_img_1708415670);
      Cooling_FAN_Set_PWM(0); // 关闭冷却风扇
  }

  if (HeaterHeatingTime == 0)
  {
    lv_img_set_src(ui_SleepStatus, &ui_img_minus_png);
  } else {
    lv_img_set_src(ui_SleepStatus, &ui_img_295618317);
  }
}

void SystemSettingScreen_init() 
{    
    // 为所有控件添加滚动跟随焦点的标志
    lv_obj_add_flag(ui_PIDSetting, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_flag(ui_ReflowSetting, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_flag(ui_HeaterMaxTemp, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_flag(ui_HeatingTime, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_flag(ui_SystemBrightness, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_flag(ui_SettingSave, LV_OBJ_FLAG_SCROLL_ON_FOCUS); 

    lv_label_set_text_fmt(ui_TextHeaterMaxTemp, "%d℃", HeaterTargetTempMax);
    lv_slider_set_value(ui_HeaterMaxTemp, HeaterTargetTempMax, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextHeatingTime, "%dMin", HeaterHeatingTime);
    lv_slider_set_value(ui_HeatingTime, HeaterHeatingTime, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextBrightness, "%d%%", Brightness);
    lv_slider_set_value(ui_SystemBrightness, Brightness, LV_ANIM_OFF);
}

void PIDSettingScreen_init()
{    
    // 为所有控件添加滚动跟随焦点的标志
    lv_obj_add_flag(ui_HeaterKP, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_flag(ui_HeaterKI, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_add_flag(ui_HeaterKD, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
}

void UserSettingScreen_init()
{
    lv_label_set_text_fmt(ui_TextTemp1, "%d℃", Temp1);
    lv_slider_set_value(ui_Temp1, Temp1, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTemp2, "%d℃", Temp2);
    lv_slider_set_value(ui_Temp2, Temp2, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTemp3, "%d℃", Temp3);
    lv_slider_set_value(ui_Temp3, Temp3, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTemp4, "%d℃", Temp4);
    lv_slider_set_value(ui_Temp4, Temp4, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTemp5, "%d℃", Temp5);
    lv_slider_set_value(ui_Temp5, Temp5, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTime1, "%dS", Time1);
    lv_slider_set_value(ui_Time1, Time1, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTime2, "%dS", Time2);
    lv_slider_set_value(ui_Time2, Time2, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTime3, "%dS", Time3);
    lv_slider_set_value(ui_Time3, Time3, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTime4, "%dS", Time4);
    lv_slider_set_value(ui_Time4, Time4, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui_TextTime5, "%dS", Time5);
    lv_slider_set_value(ui_Time5, Time5, LV_ANIM_OFF);

    user_chart_init();
}