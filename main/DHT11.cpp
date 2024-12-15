#include "Arduino.h"
#include "DHT11.h"

DHT11_Class::DHT11_Class(unsigned int pin)
{
    Data_pin = pin;
}

DHT11_Class::~DHT11_Class()
{
}

// void DHT11_Class::begin()
// {

// }

// float DHT11_Class::readHum()
// {
// }

// float DHT11_Class::readTemp()
// {
// }

pair<float, float> DHT11_Class::read()
{
    uint8_t data[5] = {0};
    float Temp = 0.0, Hum = 0.0;
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

uint8_t DHT11_Class::REDATA()
{
    uint8_t data = 0;

    if (!PinSTA)
    {
        pinMode(Data_pin, INPUT);
        PinSTA = true;
    }

    if (digitalRead(Data_pin) == LOW)
    {
        delayMicroseconds(80);
        if (digitalRead(Data_pin) == HIGH)
        {
            delayMicroseconds(80);

            for (int i = 7; i >= 0; i--)
            {
                while (digitalRead(Data_pin) == LOW)
                    ;
                uint32_t t = micros();
                while (digitalRead(Data_pin) == HIGH)
                    if ((micros() - t) > 50)
                    {
                        data |= (1 << i);
                    }
            }
        }
    }

    return data;
}

void DHT11_Class::WRDATA()
{
    PinSTA = false;
    pinMode(Data_pin, OUTPUT);
    digitalWrite(Data_pin, LOW); // 拉低至少 18 ms
    delay(18);
    digitalWrite(Data_pin, HIGH); // 拉高 20~40 µs
    delayMicroseconds(40);
}
