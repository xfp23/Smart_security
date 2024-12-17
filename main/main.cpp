#include "System.h"

extern "C" void app_main(void)
{

    System_Init();
    esp_task_wdt_add(NULL);
    while (1)
    {
    esp_task_wdt_reset(); // 喂狗
        printf("SYSTEM LOOP\n");
        if (System.timer.system_1ms == ON) // 1ms 标志
        {
            printf("1ms\n");
            System.timer.system_1ms = OFF;
            DealWith_error();
        }

        if (System.timer.system_10ms == ON) // 10ms 标志
        {
            printf("10ms\n");
            System.timer.system_10ms = OFF;
            System.sensor.Smoke = MQ.read();
        }

        if (System.timer.system_100ms == ON) // 100ms 标志
        {
            printf("100ms\n");
            System.timer.system_100ms = OFF;
            Delawith_Beep();
        }

        if (System.timer.system_500ms == ON) // 500ms 标志
        {
            printf("500ms\n");

            System.timer.system_500ms = OFF;
            // DealWith_Body();
            // DealWith_ble();
        }

        if (System.timer.system_1000ms == ON) // 1000ms 标志
        {
            printf("1000ms\n");
            System.timer.system_1000ms = OFF;
            dht.readTemperatureHumidity((int &)System.sensor.Temp, (int &)System.sensor.Hum);
        }
    }
    delay(10);
}
