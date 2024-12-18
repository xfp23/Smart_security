#include "System.h"

Nimble_Server ble; // 蓝牙
volatile System_Class_t System;
Timer_Class timer;          // 定时器对象
DHT11_Class dht(DHT11_PIN); // 温湿度传感器
Beep_Class Beep;            // 蜂鸣器
MQ_2_Class MQ;              // 烟雾传感器

bool timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    static uint8_t system1ms_count = 0;
    static uint16_t system10ms_count = 0;
    static uint16_t system100ms_count = 0;
    static uint16_t system500ms_count = 0;
    static uint16_t system1000ms_count = 0;
    if (++system1ms_count > 1)
    {
        system1ms_count = 0;
        System.timer.system_1ms = ON;
    }
    if (++system10ms_count > 10)
    {
        system10ms_count = 0;
        System.timer.system_10ms = ON;
    }
    if (++system100ms_count > 100)
    {
        system100ms_count = 0;
        System.timer.system_100ms = ON;
    }
    if (++system500ms_count > 500)
    {
        system500ms_count = 0;
        System.timer.system_500ms = ON;
    }
    if (++system1000ms_count > 1000)
    {
        system1000ms_count = 0;
        System.timer.system_1000ms = ON;
    }
    return true;
}

/**
 * @brief 按键1按下
 *
 */
static void IRAM_ATTR Switch_1Handle(void *arg)
{
    if (System.flag.Power == OFF)
    {
        System.flag.Power = ON;
    }
    else if (System.flag.Power == ON)
    {
        System.flag.Power = OFF;
    }
    System.flag.Beep = SW;
}

static void gpio_init()
{
    gpio_config_t gpio_conf = {
        .pin_bit_mask = 1ULL << SW1_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    gpio_install_isr_service(0); // 使用默认标志
    gpio_isr_handler_add(SW1_PIN, Switch_1Handle, (void *)SW1_PIN);
    gpio_config(&gpio_conf);
    gpio_conf = {
        .pin_bit_mask = 1ULL << BODY_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_conf);
}
void System_Init(void)
{
    printf("SYSTEM Init\n");
    memset((void *)&System.flag, 0, sizeof(System_flag_t));
    printf("SYSTEM Init1\n");
    gpio_init();
    ble.begin("Smart_security");
    printf("SYSTEM Init2\n");
    Beep.begin(BEEP_PIN);
    printf("SYSTEM Init3\n");
    MQ.begin(ADC2, ADC_CHANNEL_9);
    printf("SYSTEM Init4\n");
    timer.RegisterAlarm(timer_callback);
    printf("SYSTEM Init5\n");
    timer.begin();
}

/**
 * @brief 检测人体
 *
 */
// void DealWith_Body()
// {
//     System.sensor.Body = digitalRead(BODY_PIN);
// }

/**
 * @brief 处理蓝牙通信
 *
 */
void DealWith_ble()
{
    char *data = NULL;
   
    if (ble.bleSta() == true && (data = ble.get_BleData()) != NULL)
    {
         printf("Ble : %s" ,data);
        System.flag.Beep = BLE;
        if (!strcmp(data, "ON"))
        {
            System.flag.Power = ON;
        }

        if (!strcmp(data, "OFF"))
        {
            System.flag.Power = OFF;
        }
        if (!strcmp(data, "beep"))
        {
             Beep.turn_on(2, 150);
        }

        if (!strcmp(data, "scrON"))
        {
            System.flag.Screen = ON;
        }

        if (!strcmp(data, "scrOFF"))
        {
            System.flag.Screen = OFF;
        }
    }
}

/**
 * @brief 处理传感器报警逻辑
 *
 */
void DealWith_error()
{
    if (System.sensor.Smoke > 50.00 && System.sensor.Smoke < 100.00) // 烟雾报警
    {
        System.flag.Beep = MQ_2;
    }
    if (System.sensor.Body == ON)
    {
        System.flag.Beep = BODY;
        System.sensor.Body = OFF;
    }
    if (System.sensor.Hum > 50 && System.sensor.Hum < 100)
    {
        System.flag.Beep = DHT11_HUM;
    }
    if (System.sensor.Temp > 40 && System.sensor.Temp < 100)
    {
        System.flag.Beep = DHT11_TEMP;
    }
}

void Delawith_Beep()
{
    switch (System.flag.Beep)
    {
    case BEEP_OFF:
        Beep.turn_off();
        break;
    case MQ_2:
        Beep.turn_on(10, 100);
        break;
    case DHT11_TEMP:
        Beep.turn_on(3, 1000);
        break;
    case DHT11_HUM:
        Beep.turn_on(2, 500);
        break;
    case BLE:
        Beep.turn_on(1, 50);
        break;
    case SW:
        Beep.turn_on(1, 50);
    default:
        Beep.turn_off();
        break;
    }
    System.flag.Beep = BEEP_OFF;
}