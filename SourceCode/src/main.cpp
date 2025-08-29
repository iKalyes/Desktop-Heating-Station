#include <main.h>

void setup()
{
  Serial.begin(11520); /* prepare for possible serial debug */

  ReadFlash();

  System_Init();

}

void loop()
{
  lvgl_task_handler();
}

void setup1()
{
  
}

void loop1()
{
  Heater_PID_Compute();
}