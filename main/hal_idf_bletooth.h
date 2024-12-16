#ifndef HAL_IDF_BLETOOTH_H
#define HAL_IDF_BLETOOTH_H

/******************************************************************************
 * @file hal_idf_bletooth.h
 * @brief ESP蓝牙驱动
 *
 * Copyright (C) 2024, xfp23. All rights reserved.
 *
 * @author xfp23 (@github.com/xfp23)
 * @date 2024-8-23
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


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "user_queue.h"
// #include "init_System.h"
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "driver/gpio.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"

#include "esp_err.h"

#define BLE_LOG "ESP_BLE_ADVMASTER"

#define DEVICE_NAME_SIZE 32
#define BLE_MESSAGE_SIZE 1024

#define DEFAULT_SERVICE_UUID 0x00FF // 系统 默认的服务uuid
#define DEFAULT_CHAR_UUID 0xFF01   // 系统默认的特征uuid
 
#define adv_config_flag (1 << 0)      // 广播数据配置标志
#define scan_rsp_config_flag (1 << 1) // 扫描响应数据配置标志
#define ADV_UUID adv_service_uuid128  // 广播UUID

#define GATTS_NUM_HANDLE (4) // gatt 句柄数量

// 此处选择收到的数据类型 (测试用)
#define BLE_REVICE_STRING (1)
#define BLE_REVICE_BYTE (0)

// 缓冲区最大大小
#define PREPARE_BUF_MAX_SIZE (1024)

// 广播数据默认初始化
#define DEFAULT_ADV_DATA {                                                                                                                      \
    .set_scan_rsp = false,                                                /* 设置为假，表示这是广播数据而非扫描响应数据 */ \
    .include_name = true,                                                 /* 广播中包含设备名称 */                                     \
    .include_txpower = false,                                             /* 广播中不包含发射功率 */                                  \
    .min_interval = 0x0006,                                               /* 从设备连接的最小间隔，单位为 0.625ms */              \
    .max_interval = 0x0010,                                               /* 从设备连接的最大间隔，单位为 0.625ms */              \
    .appearance = 0x00,                                                   /* 设备外观 */                                                    \
    .manufacturer_len = 0,                                                /* 厂商数据长度 */                                              \
    .p_manufacturer_data = NULL,                                          /* 厂商数据指针 */                                              \
    .service_data_len = 0,                                                /* 服务数据长度 */                                              \
    .p_service_data = NULL,                                               /* 服务数据指针 */                                              \
    .service_uuid_len = sizeof(adv_service_uuid128),                      /* 服务UUID长度 */                                                \
    .p_service_uuid = adv_service_uuid128,                                /* 服务UUID */                                                      \
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT), /* 广播标志 */                                                    \
}

// 广播参数默认初始化
#define DEFAULT_ADV_PARAM {                                                                                        \
    .adv_int_min = 0x20,                                    /* 广播最小间隔，单位为 0.625ms */           \
    .adv_int_max = 0x40,                                    /*  广播最大间隔，单位为 0.625ms*/           \
    .adv_type = ADV_TYPE_IND,                               /*  广播类型，指示广播数据类型*/          \
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,                  /*  自有地址类型*/                               \
    .channel_map = ADV_CHNL_ALL,                            /*  广播频道映射，所有频道*/                \
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY, /*  广播过滤策略，允许任何扫描和连接*/ \
}

#define DEFAULT_SCAN_PARAM {                                                                                                  \
    .set_scan_rsp = true,                                                 /* 设置为真，表示这是扫描响应数据 */ \
    .include_name = true,                                                 /* 扫描响应中包含设备名称*/              \
    .include_txpower = true,                                              /* 扫描响应中包含发射功率*/              \
    .appearance = 0x00,                                                   /* 设备外观*/                                   \
    .manufacturer_len = 0,                                                /* 厂商数据长度*/                             \
    .p_manufacturer_data = NULL,                                          /* 厂商数据指针*/                             \
    .service_data_len = 0,                                                /* 服务数据长度*/                             \
    .p_service_data = NULL,                                               /* 服务数据指针*/                             \
    .service_uuid_len = sizeof(adv_service_uuid128),                      /* 服务UUID长度*/                               \
    .p_service_uuid = adv_service_uuid128,                                /* 服务UUID*/                                     \
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT), /* 扫描响应数据标志*/                       \
}

// 默认特征值
#define DEFAULT_CHAR_STR {0x11, 0x22, 0x33}

#define DEFAULT_CONFIG_CHAR {                                    \
    .attr_max_len = 0x40,         /* 特征值的最大长度 */ \
    .attr_len = sizeof(char_str), /* 特征值的实际长度*/  \
    .attr_value = char_str,       /* 特征值数据*/           \
}
    // GATT 服务 UUID
    enum
    {
        GENERIC_ACCESS_SERVICE = 0x1800,                // 通用访问
        ALARM_NOTIFICATION_SERVICE = 0x1811,            // 闹钟通知
        AUTOMATION_IO_SERVICE = 0x1815,                 // 自动化输入输出
        BATTERY_SERVICE = 0x180F,                       // 电池数据
        BINARY_SENSOR_SERVICE = 0x183B,                 // 二元传感器
        BLOOD_PRESSURE_SERVICE = 0x1810,                // 血压
        BODY_COMPOSITION_SERVICE = 0x181B,              // 身体组成
        BOND_MANAGEMENT_SERVICE = 0x181E,               // 设备绑定管理
        LINK_LOSS_SERVICE = 0x1803,                     // 连接丢失
        DEVICE_INFORMATION_SERVICE = 0x180A,            // 设备信息
        IMMEDIATE_ALERT_SERVICE = 0x1802,               // 立即警报
        HEALTH_THERMOMETER_SERVICE = 0x1809,            // 健康温度计
        HID_SERVICE = 0x1812,                           // 人体接口设备
        HEART_RATE_SERVICE = 0x180D,                    // 心率
        GLUCOSE_SERVICE = 0x1808,                       // 血糖
        RUNNING_SPEED_AND_CADENCE_SERVICE = 0x1814,     // 跑步速度与踏频
        TX_POWER_SERVICE = 0x1804,                      // 传输功率
        LOCATION_AND_NAVIGATION_SERVICE = 0x1819,       // 位置和导航
        CONTINUOUS_GLUCOSE_MONITORING_SERVICE = 0x1813, // 持续血糖监测
        ENVIRONMENTAL_SENSING_SERVICE = 0x181A,         // 环境传感
        SMART_ALERT_SERVICE = 0x1816,                   // 智能警报
        PHONE_ALERT_STATUS_SERVICE = 0x1817,            // 电话警报状态
    };

    // GATT 特征 UUID
    enum
    {
        AEROBIC_HEART_RATE_LOWER_LIMIT = 0x2A7E, // 有氧心律下限
        AEROBIC_HEART_RATE_UPPER_LIMIT = 0x2A84, // 有氧心律上限
        AEROBIC_THRESHOLD = 0x2A7F,              // 有氧运动阈值
        HEART_RATE_MEASUREMENT = 0x2A37,         // 心率测量
        BODY_SENSOR_LOCATION = 0x2A38,           // 身体传感器位置
        GLUCOSE_MEASUREMENT = 0x2A18,            // 血糖测量
        GLUCOSE_MEASUREMENT_CONTEXT = 0x2A34,    // 血糖测量上下文
        TX_POWER_LEVEL = 0x2A07,                 // 传输功率级别
        BATTERY_LEVEL = 0x2A19,                  // 电池电量
        DEVICE_NAME = 0x2A00,                    // 设备名称
        SYSTEM_ID = 0x2A23,                      // 系统 ID
        MANUFACTURER_NAME_STRING = 0x2A29,       // 制造商名称
        SERIAL_NUMBER_STRING = 0x2A25,           // 序列号
        FIRMWARE_REVISION_STRING = 0x2A26,       // 固件版本
        HARDWARE_REVISION_STRING = 0x2A27,       // 硬件版本
        SOFTWARE_REVISION_STRING = 0x2A28,       // 软件版本
        PNP_ID = 0x2A50,                         // PnP ID
        LOCATION_AND_NAVIGATION = 0x2AA3,        // 位置和导航
        ALERT_LEVEL = 0x2A06,                    // 警报级别
        RSC_MEASUREMENT = 0x2A53,                // 跑步速度和踏频测量
        RSC_FEATURE = 0x2A54,                    // 跑步速度和踏频特性
    };

    // GATT 描述符枚举
    enum
    {
        CHARACTERISTIC_AGGREGATE_FORMAT = 0x2905,       // 特征汇总格式
        CHARACTERISTIC_EXTENDED_PROPERTIES = 0x2900,    // 特性扩展属性
        CHARACTERISTIC_PRESENTATION_FORMAT = 0x2904,    // 特征描述格式
        CHARACTERISTIC_USER_DESCRIPTION = 0x2901,       // 特征用户描述
        CLIENT_CHARACTERISTIC_CONFIGURATION = 0x2902,   // 客户端特征配置
        ENVIRONMENTAL_SENSING_CONFIGURATION = 0x290B,   // 环境感应配置
        ENVIRONMENTAL_SENSING_MEASUREMENT = 0x290C,     // 环境感测
        ENVIRONMENTAL_SENSING_TRIGGER_SETTING = 0x290D, // 环境感应触发设定
        EXTERNAL_REPORT_REFERENCE = 0x2907,             // 外部报告参考
        NUMBER_OF_DIGITALS = 0x2909,                    // 数字个数
        REPORT_REFERENCE = 0x2908,                      // 报告参考
        SERVER_CHARACTERISTIC_CONFIGURATION = 0x2903,   // 服务器特征配置
        TIME_TRIGGER_SETTING = 0x290E,                  // 时间触发设定
        VALID_RANGE = 0x2906,                           // 有效范围
        VALUE_TRIGGER_SETTING = 0x290A                  // 数值触发设定
    };

    // 蓝牙设备外观

enum
{
    APPEARANCE_UNKNOWN = 0x0000,                    // 未知外观
    APPEARANCE_GENERIC_PHONE = 0x0040,              // 通用手机
    APPEARANCE_GENERIC_COMPUTER = 0x0080,           // 通用计算机
    APPEARANCE_GENERIC_KEYBOARD = 0x0001,           // 通用键盘
    APPEARANCE_GENERIC_MOUSE = 0x0002,              // 通用鼠标
    APPEARANCE_HEART_RATE_MONITOR = 0x000C,         // 心率监测器
    APPEARANCE_BLOOD_PRESSURE_MONITOR = 0x000E,     // 血压监测仪
    APPEARANCE_GENERIC_GLUCOSE_METER = 0x0008,      // 通用血糖仪
    APPEARANCE_GENERIC_HEALTH_THERMOMETER = 0x0030, // 通用体温计
    APPEARANCE_GENERIC_REMOTE_CONTROL = 0x0020,     // 通用遥控器
    
    // 工业设备外观标识符
    APPEARANCE_GENERIC_SENSOR = 0x0100,              // 通用传感器
    APPEARANCE_TEMPERATURE_SENSOR = 0x0101,          // 温度传感器
    APPEARANCE_PRESSURE_SENSOR = 0x0102,             // 压力传感器
    APPEARANCE_HUMIDITY_SENSOR = 0x0103,             // 湿度传感器
    APPEARANCE_VIBRATION_SENSOR = 0x0104,            // 振动传感器
    APPEARANCE_FLOW_METER = 0x0105,                  // 流量计
    APPEARANCE_LEVEL_SENSOR = 0x0106,                // 液位传感器
    APPEARANCE_FACTORY_CONTROLLER = 0x0107,          // 工厂控制器
    APPEARANCE_MACHINE_MONITOR = 0x0108,             // 机器监控器
    APPEARANCEIndustrial_SWITCH = 0x0109,            // 工业开关
    APPEARANCE_GENERIC_ACTUATOR = 0x010A,            // 通用执行器

};


    typedef struct
    {
        uint8_t *prepare_buf; // 准备缓冲区指针
        int prepare_len;      // 准备缓冲区长度
    } prepare_type_env_t;

    // GATT配置文件实例
    typedef struct
    {
        esp_gatts_cb_t gatts_cb;       // GATT事件处理回调函数
        uint16_t gatts_if;             // GATT接口
        uint16_t app_id;               // 应用ID
        uint16_t conn_id;              // 连接ID
        uint16_t service_handle;       // 服务句柄
        esp_gatt_srvc_id_t service_id; // 服务ID
        uint16_t char_handle;          // 特征句柄
        esp_bt_uuid_t char_uuid;       // 特征UUID
        esp_gatt_perm_t perm;          // 特征权限
        esp_gatt_char_prop_t property; // 特征属性
        uint16_t descr_handle;         // 描述符句柄
        esp_bt_uuid_t descr_uuid;      // 描述符UUID
    } gatts_profile_inst_t;

    // 蓝牙模块标志位
    typedef struct
    {
        uint8_t ble_STA : 1;              // 蓝牙连接状态
        uint8_t ble_Name_change : 1;      // 是否改变蓝牙名称的标志
        uint8_t ble_adv_sta : 1;          // 广播成功与否
        uint8_t ble_adv_stop_sta : 1;     // 广播停止与否？
        uint8_t ble_update_param_sta : 1; // 更新参数否？
        uint8_t Reserved_bit : 3;         // 保留位
    } Ble_flag_t;

    // gatt uuid 基本设置
    typedef struct
    {
        uint16_t service_uuid; // 服务uuid
        uint16_t char_uuid;    // 特征uuid
        uint16_t descr_uuid;   // 描述符uuid
    } ble_gatts_uuid_t;

    typedef struct
    {
        uint8_t Message[BLE_MESSAGE_SIZE]; // 消息内容
        uint16_t size;                     // 消息大小
    } ble_content_t;

    // 蓝牙模块对象类型
    typedef struct
    {
        char ble_name[DEVICE_NAME_SIZE]; // 蓝牙名称
        ble_content_t content;           // 收到的数据
        Ble_flag_t flag;
        gatts_profile_inst_t gatts_profile_inst; // GATT配置文件实例
        ble_gatts_uuid_t uuid;
    } ESP_Class_ble_t;

    // extern void InitBle();

    extern void InitBle_adv(char *ble_name, uint16_t service_uuid, uint16_t char_uuid, int appear);
    extern ESP_Class_ble_t ESP_BLE;
    // extern user_queue_t BLE_QUEUE;
//    extern esp_ble_adv_data_t adv_data; // 发送数据的广播数据
#ifdef __cplusplus
}
#endif

#endif