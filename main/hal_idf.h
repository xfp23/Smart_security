#ifndef HAL_IDF_H
#define HAL_IDF_H
/**
 * @file 创建日期:2024/08/02
 * @author 彭小风
 * @brief 使用此库需要idf版本V5.2 配置cmake为 REQUIRES driver esp_timer 否则会报错
 */

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
#define millis()       (esp_timer_get_time() / 1000)
#define DELAY_US(us)   esp_rom_delay_us(us)
#define DELAY_MS(ms)   vTaskDelay(pdMS_TO_TICKS(ms))

/*********************************GPIO***************************/
#define pinMode(pin, mode)  gpio_set_direction(pin, mode)

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
