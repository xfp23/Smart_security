#ifndef MQ_2_H
#define MQ_2_H

/******************************************************************************
 * @file MQ_2.h
 * @brief 烟雾传感器驱动
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

#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"

#define ADC1 ADC_UNIT_1
#define ADC2 ADC_UNIT_2
class MQ_2_Class
{

public:
    ~MQ_2_Class();// 析构
    void begin(adc_unit_t, adc_channel_t);
    float read();

private:
    adc_oneshot_unit_handle_t adc_handle;
     adc_channel_t MQ2_Channel;
    uint16_t getAnalog();
};

#endif