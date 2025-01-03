#include "System.h"

extern "C" void app_main(void)
{

    System_Init();
    esp_task_wdt_add(NULL);
    while (1)
    {
        esp_task_wdt_reset(); 
        if (System.timer.system_1ms == ON) 
        {
            System.timer.system_1ms = OFF;
            DealWith_error();
            DealWith_ble();
        }

        if (System.timer.system_10ms == ON)
        {
            System.timer.system_10ms = OFF;
            System.sensor.Smoke = MQ.read();
            DealWith_Body();
        }

        if (System.timer.system_100ms == ON)
        {
            System.timer.system_100ms = OFF;
            Delawith_Beep();
        }

        if (System.timer.system_500ms == ON)
        {
            System.timer.system_500ms = OFF;
            dht.readTemperatureHumidity((int &)System.sensor.Temp, (int &)System.sensor.Hum);
        }

        if (System.timer.system_1000ms == ON)
        {
            System.timer.system_1000ms = OFF;

            DealWith_oled();
        }
    }
}
