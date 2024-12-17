#ifndef TIMER_H
#define TIMER_H

#include "esp_timer.h"
#include "driver/gptimer_types.h"
#include "driver/gptimer.h"

typedef bool (*Timer_Alarm)(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);
class Timer_Class
{

public:
    Timer_Class();
    ~Timer_Class();
    void begin(uint32_t hz = 1000000,uint8_t intr = 2,uint32_t alarm  = 10000,uint32_t reload = 0);
    void RegisterAlarm(Timer_Alarm fun);
    void start(); // 启动定时器
    void stop(); // 停止定时器

private:
    Timer_Alarm Timer_Alarm_Fun = NULL;
    gptimer_handle_t gptimer_handle = NULL; // 定时器句柄
    gptimer_event_callbacks_t *cbs;
};

#endif