#ifndef _PID_SERVICE_H
#define _PID_SERVICE_H

#include <Arduino.h>
#include <QuickPID.h>
#include <GPIO_Service.h>
#include <MAX6675_Service.h>
#include <variables.h>
#include <lvgl.h>
#include "ui/ui.h"

// 创建float类型的中间变量用于PID计算
static float Heater_temp_float = 0.0f;
static float Heater_target_temp_float = 0.0f;

// 蜂鸣器控制变量
static struct {
    bool temperature_reached_played;    // 是否已鸣叫过到达温度
    bool short_active;                  // 短鸣叫是否激活
    bool long_active;                   // 长鸣叫是否激活
    unsigned long start_time;           // 蜂鸣器开始时间
} buzzer_state = {false, false, false, 0};

// 内置曲线：分段线性插值工具
static inline float _interp1(float t, const float* ts, const float* ys, size_t n)
{
    if (n == 0) return (float)HeaterTargetTemp;
    if (t <= ts[0]) return ys[0];
    if (t >= ts[n - 1]) return ys[n - 1];
    for (size_t i = 0; i + 1 < n; ++i) {
        if (t >= ts[i] && t <= ts[i + 1]) {
            const float dt = ts[i + 1] - ts[i];
            const float k = (dt > 0.0f) ? (t - ts[i]) / dt : 0.0f;
            return ys[i] + k * (ys[i + 1] - ys[i]);
        }
    }
    return ys[n - 1];
}

// RSS 无铅回流曲线（较保守，爬升慢、浸泡长、峰值230℃，TAL约60s）
static inline float RSS_Curve(unsigned long elapsed_ms)
{
    const float t = elapsed_ms / 1000.0f; // s
    // 时间点(s)与目标温度(℃)
    static const float ts[]  = {   0,   90,  150,  190,  250,  290,  9999 };
    static const float ys[]  = {  50,  150,  180,  230,  230,  150,    50 };
    return _interp1(t, ts, ys, sizeof(ts)/sizeof(ts[0]));
}

// RTS 无铅回流曲线（更快，峰值240℃，TAL约45~60s）
static inline float RTS_Curve(unsigned long elapsed_ms)
{
    const float t = elapsed_ms / 1000.0f; // s
    static const float ts[]  = {   0,   60,   90,  120,  170,  200,  9999 };
    static const float ys[]  = {  50,  150,  170,  230,  230,  150,    50 };
    return _interp1(t, ts, ys, sizeof(ts)/sizeof(ts[0]));
}

// 自定曲线模式
static inline float CUSTOM_Curve(unsigned long elapsed_ms)
{
    const float t = elapsed_ms / 1000.0f; // s
    // 时间点(s)与目标温度(℃)
    static const float ts[]  = {   0, (float)Time1, (float)Time2, (float)Time3, (float)Time4, (float)Time5, 9999};
    static const float ys[]  = {  50, (float)Temp1, (float)Temp2, (float)Temp3, (float)Temp4, (float)Temp5,   50};
    return _interp1(t, ts, ys, sizeof(ts)/sizeof(ts[0]));
}

void Heater_PID_Compute_Init();
void Heater_PID_Update_Tunings(float Kp, float Ki, float Kd);
void Heater_PID_Compute();

#endif