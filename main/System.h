#ifndef SYSTEM_H
#define SYSTEM_H

#include "Beep.h"
#include "DHT11.h"
#include "pins.h"
#include "MQ_2.h"
#include "esp_task_wdt.h"
#include "timer.h"
#include "ble.h"
#include "oled.h"

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
//"scrON" 开启屏幕显示 
//"scrOFF" 关闭屏幕 
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
    uint8_t Power : 1;
    uint8_t Screen : 1;      
    uint8_t Reserve_bit : 6; 
    Beep_alarm_t Beep;   
} System_flag_t;

typedef struct
{
    int Temp; 
    int Hum;   
    float Smoke; 
    bool Body;  
} Sensor_t;

#define TEMP_MIN 0
#define TEMP_MAX 40
#define HUM_MIN 20
#define HUM_MAX 70
#define SMOKE_MIN 70
#define SMOKE_MAX 100


typedef struct
{
    timer_flag_t timer; 
    System_flag_t flag;
    Sensor_t sensor;   
} System_Class_t;

// extern void DealWith_Body();
extern void Delawith_Beep();
extern void DealWith_ble();
extern void DealWith_error();
extern void DealWith_oled();
extern volatile System_Class_t System;
extern void System_Init(void);
extern Nimble_Server ble;
extern Timer_Class timer;        
extern DHT11_Class dht;
extern Beep_Class Beep;            
extern MQ_2_Class MQ;            
#endif