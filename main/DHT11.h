#ifndef DHT11_H
#define DHT11_H
#include <utility>
#include <tuple>
#include <iostream>
#include "driver/gpio.h"


/******************************************************************************
 * @file DHT11.h
 * @brief DHT11驱动
 *
 * Copyright (C) 2024, xfp23. All rights reserved.
 *
 * @author xfp23 (@github.com/xfp23)
 * 
 * This software is provided "as is", without any warranties or guarantees, 
 * either express or implied, including but not limited to the implied warranties 
 * of merchantability and fitness for a particular purpose. In no event shall the 
 * author be liable for any damages, including direct, indirect, incidental, or 
 * consequential damages, arising from the use of this software.
 * 
 * Permission is hereby granted, free of charge, to use, copy, modify, and/or 
 * distribute this software, provided that the above copyright notice and this 
 * permission notice are included in all copies or substantial portions of the software.
 *
 * This software is also licensed under the MIT License.
 * 
 *****************************************************************************/
using namespace std;


class DHT11_Class
{

public:
DHT11_Class(gpio_num_t);
~DHT11_Class();

float getHum();
float getTemp();
pair<float, float> read();
private:
// bool PinSTA = false;
gpio_num_t Data_pin;
uint8_t REDATA();
void WRDATA();
void setInput();
void SetOutput();
float Hum;
float Temp;
};


#endif