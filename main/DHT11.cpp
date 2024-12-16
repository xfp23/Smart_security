#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DHT11.h"
#include "esp_rom_sys.h"
DHT11_Class::DHT11_Class(gpio_num_t pin)
{
    Data_pin = pin;
}

DHT11_Class::~DHT11_Class()
{

}

void DHT11_Class::setInput()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << Data_pin,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
}

void DHT11_Class::SetOutput()
{
       gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << Data_pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
}

pair<float, float> DHT11_Class::read()
{
    uint8_t data[5] = {0};
    WRDATA();
    for (int byte = 0; byte < 5; byte++)
    {
        data[byte] = REDATA();
    }
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
    {
        Hum = data[0] + (float)data[1] / 100;
        Temp = data[2] + (float)data[3] / 100;
    }
    
    return make_pair(Hum, Temp);
}

uint8_t DHT11_Class::REDATA() {
    uint8_t data = 0;
    // 检查起始信号
    if (gpio_get_level(Data_pin) == 0) {
        esp_rom_delay_us(80); 
        if (gpio_get_level(Data_pin) == 1) {
            esp_rom_delay_us(80); 

            // 开始读取 8 位数据
            for (int i = 7; i >= 0; i--) {
                // 等待信号变为低电平（每个数据位的开始）
                uint32_t timeout = 1000; // 超时时间（微秒）
                while (gpio_get_level(Data_pin) == 0 && --timeout > 0) {
                    esp_rom_delay_us(1);
                }
                timeout = 1000;
                while (gpio_get_level(Data_pin) == 1 && --timeout > 0) {
                    esp_rom_delay_us(1);
                }

                // 检查高电平持续时间
                esp_rom_delay_us(40); // 等待至少 40us
                if (gpio_get_level(Data_pin) == 1) {
                    data |= (1 << i); // 如果高电平持续时间长，则为 1
                }

                // 等待信号变为低电平，准备下一个数据位
                timeout = 1000;
                while (gpio_get_level(Data_pin) == 1 && --timeout > 0) {
                    esp_rom_delay_us(1);
                }
            }
        }
    }

    return data;
}


void DHT11_Class::WRDATA()
{
    SetOutput();
    gpio_set_level(Data_pin, 0); // 拉低至少 18 ms
    vTaskDelay(pdMS_TO_TICKS(18));
    gpio_set_level(Data_pin, 1); // 拉高 20~40 µs
    esp_rom_delay_us(40);
}


float DHT11_Class::getHum()
{
    return Hum;
}

float DHT11_Class::getTemp()
{
    return Temp;
}