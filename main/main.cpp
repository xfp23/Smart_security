#include <stdio.h>
#include "hal_idf.h"
#include "ble.h"
#include "oled.h"
#include "Beep.h"
#include "DHT11.h"
#include "pins.h"
#include "MQ_2.h"
#include "esp_task_wdt.h"

Nimble_Server ble;
DHT11_Class dht(DHT11_PIN);
Beep_Class beep;
MQ_2_Class MQ;
int DHT_data[2];

extern "C" void app_main(void)
{
    ble.begin("Smart_security");
    MQ.begin(ADC2, ADC_CHANNEL_9);
    beep.begin(BEEP_PIN);
    beep.turn_on(2, 150);
    // esp_task_wdt_config_t wdg = {
    //     .timeout_ms = 10000,
    //     .idle_core_mask = 1,
    //     .trigger_panic = true
    //     };
    // esp_task_wdt_init(&wdg);
    esp_task_wdt_add(NULL); // NULL表示当前任务

    while (1)
    {
        esp_task_wdt_reset(); // 喂狗
        if (ble.bleSta())
        {

            printf("%s", ble.get_BleData());

            delay(5000);
            ble.Write((uint8_t *)"hello world", 12);
        }
        dht.readTemperatureHumidity(DHT_data[0], DHT_data[1]);

        // DHT_data[0] = dht.readHumidity();
        // printf("温度 : %d℃\n", DHT_data[0]);

        // DHT_data[1] = dht.readTemperature();
        // printf("湿度 : %d%%\n", DHT_data[1]);
        printf("温度 : %d℃ 湿度 : %d%%\n", DHT_data[0], DHT_data[1]);
        printf("%ld\n", millis());
        delay(1000);
    }
}
