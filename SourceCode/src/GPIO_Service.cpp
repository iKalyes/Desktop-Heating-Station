#include <GPIO_Service.h>

void Heater_GPIO_Init()
{
    // 设置加热器PWM引脚 - 50Hz PWM
    gpio_set_function(Heater_PWM, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(Heater_PWM);
    uint channel = pwm_gpio_to_channel(Heater_PWM);
    pwm_set_clkdiv(slice_num, 230.0);  // 分频器
    pwm_set_wrap(slice_num, 20000);     // 最大计数值 (分辨率)
    pwm_set_chan_level(slice_num, channel, 0); // 初始占空比为0
    pwm_set_enabled(slice_num, true);
}

void Heater_Set_PWM(float value)
{
    // 设置加热器PWM占空比
    uint slice_num = pwm_gpio_to_slice_num(Heater_PWM);
    uint channel = pwm_gpio_to_channel(Heater_PWM);
    if (value > 100) {
        value = 100; // 限制最大值
    }
    pwm_set_chan_level(slice_num, channel, value * 200); // 设置占空比
}

//——————————————————————————————————————————————————————————————————————//

void Cooling_FAN_GPIO_Init()
{
    // 设置冷却风扇PWM引脚 - 10kHz PWM
    gpio_set_function(Cooling_FAN_PWM, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(Cooling_FAN_PWM);
    uint channel = pwm_gpio_to_channel(Cooling_FAN_PWM);
    pwm_set_clkdiv(slice_num, 230.0);  // 分频器
    pwm_set_wrap(slice_num, 100);     // 最大计数值 (分辨率)
    pwm_set_chan_level(slice_num, channel, 0); // 初始占空比为0
    pwm_set_enabled(slice_num, true);
}

void Cooling_FAN_Set_PWM(float value)
{
    // 设置冷却风扇PWM占空比
    uint slice_num = pwm_gpio_to_slice_num(Cooling_FAN_PWM);
    uint channel = pwm_gpio_to_channel(Cooling_FAN_PWM);
    if (value > 100) {
        value = 100; // 限制最大值
    }
    pwm_set_chan_level(slice_num, channel, value); // 设置占空比
}

//——————————————————————————————————————————————————————————————————————//

void Buzzer_GPIO_Init()
{
    // 设置蜂鸣器PWM引脚 - 2700Hz PWM
    gpio_set_function(Buzzer_PWM, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(Buzzer_PWM);
    uint channel = pwm_gpio_to_channel(Buzzer_PWM);
    pwm_set_clkdiv(slice_num, 31.53f);  // 分频器
    pwm_set_wrap(slice_num, 2700);     // 最大计数值 (分辨率)
    pwm_set_chan_level(slice_num, channel, 0); // 初始占空比为0
    pwm_set_enabled(slice_num, true);
}

void Buzzer_Set_PWM(float value)
{
    // 设置蜂鸣器PWM占空比
    uint slice_num = pwm_gpio_to_slice_num(Buzzer_PWM);
    uint channel = pwm_gpio_to_channel(Buzzer_PWM);
    if (value > 100) {
        value = 100; // 限制最大值
    }
    // 将0-100映射到0-2700的计数值
    pwm_set_chan_level(slice_num, channel, (uint16_t)((value / 100.0) * 2700)); // 设置占空比
}

void Buzzer_ON()
{
    // 打开蜂鸣器
    Buzzer_Set_PWM(70.0f); // 设置占空比为70%
}

void Buzzer_OFF()
{
    // 关闭蜂鸣器
    Buzzer_Set_PWM(0.0f); // 设置占空比为0%
}