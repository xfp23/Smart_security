#ifndef BEEP_H
#define BEEP_H

#include <stdint.h>
/******************************************************************************
 * @file Beep.h
 * @brief 蜂鸣器驱动
 *
 * Copyright (C) 2024, xfp23. All rights reserved.
 *
 * @author xfp23 (@github.com/xfp23)
 * @date 2024-12-16
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


typedef enum{
    PWM_RUN, // pwm驱动
    DIG_RUN // 数字驱动
}Beep_run_t;

class Beep_Class 
{
    // Beep_Class();
    // ~Beep_Class();
    public:
   void begin(uint16_t freq,ledc_channel_t channel,gpio_num_t gpio);
   void begin(gpio_num_t gpio);
   void turn_on(uint8_t count,uint16_t time,float duty);
   void turn_on(uint8_t count,uint16_t ms);
   void turn_off();
    private:
    gpio_num_t Beep_pin;
    Beep_run_t Beep_run;
    ledc_channel_t Beep_channel;

};

#endif