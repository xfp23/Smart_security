#ifndef DHT11_H
#define DHT11_H
#include <utility>
#include <tuple>
#include <iostream>
#include "driver/gpio.h"
#include "esp_task_wdt.h"
#include "hal_idf.h"
using namespace std;

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
  DHT11_Class(gpio_num_t pin);
  ~DHT11_Class();
  void setDelay(unsigned long delay);
  // int readHumidity();
  // int readTemperature();

  int readTemperatureHumidity(int &temperature, int &humidity);

  // Constants to represent error codes.
  static const int ERROR_CHECKSUM = 254;    // Error code indicating checksum mismatch.
  static const int ERROR_TIMEOUT = 253;     // Error code indicating a timeout occurred during reading.
  static const int TIMEOUT_DURATION = 100; // Duration (in milliseconds) to wait before timing out.

  static string getErrorString(int errorCode);

private:
  gpio_num_t _pin;                    
  unsigned long _delayMS = 500; 

  int readRawData(uint8_t data[5]);

// void pinMode(gpio_num_t,gpio_mode_t);

  uint8_t readByte();


  void startSignal();
};


#endif