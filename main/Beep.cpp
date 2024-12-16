#include "Beep.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief 初始化蜂鸣器输出
 *
 * @param freq
 */
void Beep_Class::begin(uint16_t freq, ledc_channel_t channel, gpio_num_t gpio)
{
    Beep_run = PWM_RUN;
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = freq,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .gpio_num = gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,

        .duty = 0,
        .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void Beep_Class::begin(gpio_num_t gpio)
{
    Beep_pin = gpio;
    Beep_run = DIG_RUN;
    gpio_config_t gpio_conf = {
        .pin_bit_mask = 1ULL << gpio,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,  
    };
    gpio_config(&gpio_conf);
}

/**
 * @brief 打开蜂鸣器 用于PWM驱动
 *
 * @param count 设置蜂鸣器响几下
 * @param time  响的持续时间 ms
 * @param peercent 占空比百分比 0~100%
 */
void Beep_Class::turn_on(uint8_t count, uint16_t time, float percent)
{
    uint16_t duty = percent * 8196;
    if (percent < 0)
    {
        duty = 0;
    }
    else if (percent > 8196)
    {
        duty = 8196;
    }

    for (int i = 0; i <= count; i++)
    {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, Beep_channel, duty * 8191);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, Beep_channel);
        vTaskDelay(pdMS_TO_TICKS(time));
        ledc_set_duty(LEDC_LOW_SPEED_MODE, Beep_channel, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, Beep_channel);
        vTaskDelay(pdMS_TO_TICKS(time));
    }
}

/**
 * @brief 打开蜂鸣器 用于数字信号驱动
 *
 * @param count 响几下
 * @param ms 持续时间
 */
void Beep_Class::turn_on(uint8_t count, uint16_t ms)
{
    for(int i =0; i < count;i++)
    {
        gpio_set_level(Beep_pin,1);
        vTaskDelay(pdMS_TO_TICKS(ms));
        gpio_set_level(Beep_pin,0);
        vTaskDelay(pdMS_TO_TICKS(ms));
    }
}

void Beep_Class::turn_off()
{
    switch (Beep_run)
    {
    case PWM_RUN:
        ledc_set_duty(LEDC_LOW_SPEED_MODE, Beep_channel, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, Beep_channel);
        break;
    case DIG_RUN:
        gpio_set_level(Beep_pin, 0);
        break;
    }
}