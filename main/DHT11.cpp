
#include "DHT11.h"


DHT11_Class::DHT11_Class(gpio_num_t pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
}

void DHT11_Class::setDelay(unsigned long delay)
{
    _delayMS = delay;
}

int DHT11_Class::readRawData(uint8_t data[5])
{
    delay(_delayMS);
    startSignal();
    unsigned long timeout_start = millis();

    while (digitalRead(_pin) == HIGH)
    {
         esp_task_wdt_reset(); // 喂狗
        if (millis() - timeout_start > TIMEOUT_DURATION)
        {
            return DHT11_Class::ERROR_TIMEOUT;
        }
    }

    if (digitalRead(_pin) == LOW)
    {
        delayMicroseconds(80);
        if (digitalRead(_pin) == HIGH)
        {
            delayMicroseconds(80);
            for (int i = 0; i < 5; i++)
            {
                data[i] = readByte();
                if (data[i] == DHT11_Class::ERROR_TIMEOUT)
                {
                    return DHT11_Class::ERROR_TIMEOUT;
                }
            }

            if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
            {
                return 0; // Success
            }
            else
            {
                return DHT11_Class::ERROR_CHECKSUM;
            }
        }
    }
    return DHT11_Class::ERROR_TIMEOUT;
}

uint8_t DHT11_Class::readByte()
{
    unsigned long timeout_start = millis();
    uint8_t value = 0;

    for (int i = 0; i < 8; i++)
    {
        while (digitalRead(_pin) == LOW)
        {
             esp_task_wdt_reset(); // 喂狗
           if (millis() - timeout_start > TIMEOUT_DURATION)
            {
                break;
            }
        }
 
        delayMicroseconds(30);
        if (digitalRead(_pin) == HIGH)
        {
            value |= (1 << (7 - i));
        }
        timeout_start = millis();
        while (digitalRead(_pin) == HIGH)
        {
            
             //esp_task_wdt_reset(); // 喂狗
            if (millis() - timeout_start > TIMEOUT_DURATION)
            {
                break;
            }
        }
    }
    return value;
}

void DHT11_Class::startSignal()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(18);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);
    pinMode(_pin, INPUT);
}

int DHT11_Class::readTemperature()
{
    uint8_t data[5];
    int error = readRawData(data);
    if (error != 0)
    {
        return error;
    }
    return data[2];
}

int DHT11_Class::readHumidity()
{
    uint8_t data[5];
    int error = readRawData(data);
    if (error != 0)
    {
        return error;
    }
    return data[0];
}

int DHT11_Class::readTemperatureHumidity(int &temperature, int &humidity)
{
    uint8_t data[5];
    int error = readRawData(data);
    if (error != 0)
    {
        return error;
    }
    humidity = data[0];
    temperature = data[2];
    return 0; // Indicate success
}

string DHT11_Class::getErrorString(int errorCode)
{
    switch (errorCode)
    {
    case DHT11_Class::ERROR_TIMEOUT:
        return "Error 253 Reading from DHT11_Class timed out.";
    case DHT11_Class::ERROR_CHECKSUM:
        return "Error 254 Checksum mismatch while reading from DHT11_Class.";
    default:
        return "Error Unknown.";
    }
}

// void DHT11_Class::pinMode(gpio_num_t pin,gpio_mode_t mode)
// {
//     gpio_config_t gpio_conf = {
//         .pin_bit_mask = 1ULL << pin,
//         .mode = mode,
//         .pull_up_en = GPIO_PULLUP_DISABLE,
//         .pull_down_en = GPIO_PULLDOWN_DISABLE,
//         .intr_type = GPIO_INTR_DISABLE
//     };
//     gpio_config(&gpio_conf);
// }

