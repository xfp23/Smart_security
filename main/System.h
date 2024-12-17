#ifndef SYSTEM_H
#define SYSTEM_H

#include "Beep.h"
#include "DHT11.h"
#include "pins.h"
#include "MQ_2.h"
#include "esp_task_wdt.h"
#include "timer.h"
#include "ble.h"

#define ON 1
#define OFF 0

#define DealWith_Body() do \
{                           \
  System.sensor.Body = digitalRead(BODY_PIN); \
} while (0)


/**
 * @brief 蓝牙通信协议制定
 *
 */
// "beep" 蜂鸣器响 
//"screen" 开启屏幕显示 
//"close" 关闭屏幕 
//"OFF" 关机
//"ON" 开机

typedef struct
{
    uint8_t system_1ms : 1;    // 1ms标志
    uint8_t system_10ms : 1;   // 10ms标志
    uint8_t system_100ms : 1;  // 100ms标志
    uint8_t system_500ms : 1;  // 500ms标志
    uint8_t system_1000ms : 1; // 1000ms标志
} timer_flag_t;

typedef enum {
    BEEP_OFF,
    MQ_2,
    DHT11_TEMP,
    DHT11_HUM,
    BODY,
    SW,
    BLE,
}Beep_alarm_t; // 蜂鸣器响

typedef struct
{
    uint8_t Power : 1; // 开机
    // uint8_t PowerOFF : 1;    // 关机
    uint8_t Screen : 1;      // 屏幕
    uint8_t Reserve_bit : 6; // 保留
    Beep_alarm_t Beep;       // 蜂鸣器
} System_flag_t;

typedef struct
{
    int Temp;  // 温度
    int Hum;   // 湿度
    float Smoke; // 烟雾
    bool Body;   // 人体红外
} Sensor_t;




typedef struct
{
    timer_flag_t timer; // 定时器
    System_flag_t flag; // 系统标志
    Sensor_t sensor;    // 系统传感器值
    // uint8_t Rx_buffer[256];// 蓝牙接收缓存
} System_Class_t;

// extern void DealWith_Body();
extern void Delawith_Beep();
extern void DealWith_ble();
extern void DealWith_error();
extern volatile System_Class_t System;
extern void System_Init(void);
extern Nimble_Server ble; // 蓝牙
extern Timer_Class timer;          // 定时器对象
extern DHT11_Class dht; // 温湿度传感器
extern Beep_Class Beep;            // 蜂鸣器
extern MQ_2_Class MQ;              // 烟雾传感器
#endif