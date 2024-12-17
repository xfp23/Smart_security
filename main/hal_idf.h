#ifndef HAL_IDF_H
#define HAL_IDF_H

/******************************************************************************
 * @file hal_idf.h
 * @brief idf Arduino驱动
 * @note 使用此库需要idf版本V5.2.x 配置cmake为 REQUIRES driver esp_timer 否则会报错
 * @note 使用滴答计数器之前请将滴答时钟配置为1ms 在 FreeRTOSConfig.h中 的 configTICK_RATE_HZ = 1000
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "soc/gpio_num.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define ESP32_C3 
//#define ESP32_S3 
//#define ESP32    

/*************************延时************************/
#define millis()      pdTICKS_TO_MS(xTaskGetTickCount()) // (esp_timer_get_time()/1000)
#define delayMicroseconds(us)   esp_rom_delay_us(us)
#define delay(ms)   vTaskDelay(pdMS_TO_TICKS(ms))

/*********************************GPIO***************************/
#define pinMode(pin, mode)                 \
    do                                     \
    {                                      \
        if (pin != -1)                     \
        {                                  \
            gpio_set_direction(pin, mode); \
        }                                  \
    } while (0)
    
#define digitalWrite(pin, level) gpio_set_level(pin, level)
#define digitalRead(pin) gpio_get_level(pin)
#define OUTPUT GPIO_MODE_OUTPUT
#define INPUT GPIO_MODE_INPUT

enum {
    HIGH = 0x01, //高电平
    LOW = 0x00,  //低电平
    DEF_PIN = -1
};
/**************************************UART*************************/
typedef struct UART_DEV UART_DEV;

#define RX_BUFFER_SIZE (1024) //RX缓冲区
#define TX_BUFFER_SIZE (1024) //TX缓冲区
#define UART_TIMEOUT 100  //超时等待时间

typedef void (*INIT_UART)(UART_DEV *dev, uart_port_t port, int badu, int rx_pin, int tx_pin);
typedef void (*SEND_CHARS)(UART_DEV *dev, char ch);
//typedef char* (*READ_STR)(UART_DEV *dev, size_t len); //已废弃
typedef int8_t (*READ_BYTE)(UART_DEV *dev);
typedef void (*SEND_BYTE)(UART_DEV *dev, uint8_t *data, size_t len);
typedef char (*READ_CHAR)(UART_DEV *dev);
typedef void (*SEND_STR)(UART_DEV *dev, char* str);

#define UART0 UART_NUM_0
#define UART1 UART_NUM_1


#if SOC_UART_HP_NUM > 2
#define UART2 UART_NUM_2
#endif



typedef struct UART_DEV {
    uart_port_t uart_num;
    INIT_UART init;
    SEND_CHARS send_char;
    //READ_STR read_str; //已废弃
    SEND_STR send_str;
    READ_BYTE read_byte;
    SEND_BYTE send_byte;
    READ_CHAR read_char;
} UART_DEV;

void create_new_uart(UART_DEV *dev); //创建新串口
void HAL_init_uart(UART_DEV *dev, uart_port_t port, int badu, int rx_pin, int tx_pin);
void HAL_send_char(UART_DEV *dev, char ch);
void HAL_send_bytes(UART_DEV *dev, uint8_t *data, size_t len);
int8_t HAL_read_bytes(UART_DEV *dev);
char HAL_read_char(UART_DEV *dev);
void HAL_send_str(UART_DEV *dev, char* str);

#ifdef __cplusplus
}
#endif

#endif
