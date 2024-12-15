#ifndef USER_QUEUE_H
#define USER_QUEUE_H

#include "stdint.h"
#include "stddef.h"

/*
 * 版权所有 © 重庆锑锑锑智能装备有限公司 2024。保留所有权利。  
 * Copyright © Chongqing TTT Intelligent Equipment Co., Ltd. 2024. All rights reserved.
 *
 * 本软件及其相关文档受版权法和其他知识产权法保护。  
 * This software and its related documentation are protected by copyright laws and other intellectual property rights.
 *
 * 未经授权，不得复制、修改或分发本软件的任何部分。  
 * Unauthorized copying, modification, or distribution of any part of this software is strictly prohibited.
 *
 * 使用本代码即表示同意遵守适用的许可协议或合同条款。  
 * Use of this code implies agreement to the applicable license or contract terms.
 *
 * 有关详细的许可信息，请参阅项目根目录中的 LICENSE 文件。  
 * For detailed license information, please refer to the LICENSE file in the root directory of the project.
 */

/**
 * @file user_queue.h
 * @brief 处理字符串消息的队列定义，适用于 IDF 环境，并支持 IDF 日志打印功能。
 * @note 宏 `QUEUE_SIZE` 可扩展以增加队列大小，最大不得超过 65535，非特殊场景建议不要超过 255 或 1024。
 */

#ifdef __cplusplus
extern "C"
{
#endif

// 请选择队列日志语言 : 英语和汉语，同时置 1 表示双语输出，置 0 表示关闭日志
#define LOG_LANG_CHINESE 0
#define LOG_LANG_ENGLISH 0

#define QUEUE_SIZE 255  // 队列大小
#define QUEUE_LOG "USER QUEUE"

// 用户消息队列结构体
typedef struct {
    char *Messages[QUEUE_SIZE];  // 存储消息指针的数组
    uint16_t head;                // 消息头部索引
    uint16_t tail;                // 消息尾部索引
    uint16_t count;               // 当前消息数量

#if LOG_LANG_CHINESE || LOG_LANG_ENGLISH
    const char *label;            // 用于日志打印的标签
#endif

} user_queue_t;  // 用户消息队列，支持字符串消息的存储和释放

/**
 * @brief 初始化队列
 * @param queue 要初始化的队列对象
 * @param label 日志标签，用于区分不同队列的日志,如果禁用日志则不传此参数
 */
extern void Init_queue(user_queue_t *queue, const char *lable);

/**
 * @brief 消息入队
 * @param queue 要操作的队列对象
 * @param message 要添加到队列中的消息
 */
extern void enqueue(user_queue_t *queue, const char *message);

/**
 * @brief 消息出队
 * @param queue 要操作的队列对象
 * @return 返回出队的消息指针，如果队列为空返回 NULL
 */
extern const char* dequeue(user_queue_t *queue);

/**
 * @brief 得到队列中的消息
 * @param 队列对象
 * @param 存储消息的指针
 * @param 存储消息指针的容纳空间 单位: 字节
 */
extern void get_queue_message(user_queue_t *queue, char * str,size_t size);


#ifdef __cplusplus
}
#endif

#endif // USER_QUEUE_H