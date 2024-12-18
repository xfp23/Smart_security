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
    MQ.begin(ADC2, ADC_CHANNEL_4);
    printf("SYSTEM Init4\n");
    timer.RegisterAlarm(timer_callback);
    printf("SYSTEM Init5\n");
    timer.begin();
    OLED_Init();
    OLED_Clear();
    // OLED_ShowString(0, 0, (u8 *)"Person  :     ");
    // OLED_ShowString(0, 2, (u8 *)"Temp    :     ");
    // OLED_ShowString(0, 4, (u8 *)"Humidity:    %");
    // OLED_ShowString(0, 6, (u8 *)"Smoke   :    %");
    // OLED_ShowString(80, 0, (u8 *)"N");
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
    char data[8] = {0};

    if (ble.bleSta() == true)
    {
        mempcpy(data, ble.get_BleData(), 8);

        if (strlen(data) > 0 && (data[0] != '\n' || data[0] != ' '))
        {
            System.flag.Beep = BLE;
            if (!strcmp(data, "ON\n"))
            {
                System.flag.Power = ON;
                System.flag.Screen = ON;
            }

            if (!strcmp(data, "OFF\n"))
            {
                System.flag.Power = OFF;
                System.flag.Screen = OFF;
            }
            if (!strcmp(data, "beep\n"))
            {
                Beep.turn_on(2, 150);
            }

            if (!strcmp(data, "scrON\n"))
            {
                System.flag.Screen = ON;
            }

            if (!strcmp(data, "scrOFF\n"))
            {
                System.flag.Screen = OFF;
            }
        }
    }
}

/**
 * @brief 处理传感器报警逻辑
 *
 */
void DealWith_error()
{
    if (System.flag.Power == ON)
    {
        if (System.sensor.Smoke > SMOKE_MIN && System.sensor.Smoke <= SMOKE_MAX) // 烟雾报警
        {
            System.flag.Beep = MQ_2;
        }
        if (System.sensor.Body == ON)
        {
            System.flag.Beep = BODY;
            System.sensor.Body = OFF;
        }
        if (System.sensor.Hum > SMOKE_MAX && System.sensor.Hum < SMOKE_MIN)
        {
            System.flag.Beep = DHT11_HUM;
        }
        if (System.sensor.Temp > TEMP_MAX && System.sensor.Temp < TEMP_MIN)
        {
            System.flag.Beep = DHT11_TEMP;
        }
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

void DealWith_oled()
{
    static bool oled_flag = false;
    if (System.flag.Screen == ON)
    {
        oled_flag = true;
        OLED_ShowString(0, 0, (u8 *)"Person  :     ");
        OLED_ShowString(0, 2, (u8 *)"Temp    :     ");
        OLED_ShowString(0, 4, (u8 *)"Humidity:    %");
        OLED_ShowString(0, 6, (u8 *)"Smoke   :    %");
        if (System.sensor.Body)
        {
            OLED_ShowString(80, 0, (u8 *)"Y");
        }
        else if (!System.sensor.Body)
        {
            OLED_ShowString(80, 0, (u8 *)"N");
        }
        OLED_ShowString(75, 2, (u8 *)"  ");

        OLED_ShowNum(75, 2, System.sensor.Temp, 2, 16);
        OLED_ShowString(75, 4, (u8 *)"  ");
        OLED_ShowNum(75, 4, System.sensor.Hum, 2, 16);
        OLED_ShowString(75, 6, (u8 *)"  ");
        OLED_ShowNum(75, 6, (int)(System.sensor.Smoke), 3, 16);
    }
    else if (System.flag.Screen == OFF)
    {
        OLED_Clear();
    }
}