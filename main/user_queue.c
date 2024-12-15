#include "user_queue.h"
#include "string.h"
#include "esp_err.h"
#include "esp_log.h"

void Init_queue(user_queue_t *queue,const char *lable)
{
    memset(queue, 0, sizeof(user_queue_t));
#if LOG_LANG_CHINESE || LOG_LANG_ENGLISH
    queue->label = lable;
#endif
}

void enqueue(user_queue_t *queue, const char *message)
{
    if (queue->count == QUEUE_SIZE)
    {
#if LOG_LANG_ENGLISH
        ESP_LOGE(QUEUE_LOG, "%s : Queue is full, cannot enqueue message.\n", queue->label);
#endif

#if LOG_LANG_CHINESE
        ESP_LOGE(QUEUE_LOG, "%s : 队列已满,禁止入队.\n", queue->label);
#endif
        return; // 队列已满,不能再入队
    }
    queue->Messages[queue->tail] = strdup(message);
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    queue->count++;
#if LOG_LANG_CHINESE
    ESP_LOGI(QUEUE_LOG, "%s: 消息已入队 \n 队列空间剩余: %d", queue->label, QUEUE_SIZE - queue->count);
#endif

#if LOG_LANG_ENGLISH
    ESP_LOGI(QUEUE_LOG, "%s: Message has been queued \n Queue space remaining: %d", queue->label, QUEUE_SIZE - queue->count);
#endif
}

const char *dequeue(user_queue_t *queue)
{
    if (queue->count == 0)
    {
#if LOG_LANG_CHINESE
        ESP_LOGE(QUEUE_LOG, "%s: 队列为空,出队失败", queue->label);
#endif

#if LOG_LANG_ENGLISH
        ESP_LOGE(QUEUE_LOG, "%s: The queue is empty and dequeue failed.", queue->label);
#endif
        return NULL;
    }
    const char *msg = queue->Messages[queue->head];
    queue->Messages[queue->head] = NULL;
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    queue->count--;
#if LOG_LANG_CHINESE
    ESP_LOGI(QUEUE_LOG, "%s: 消息已出队 \n 队列空间剩余: %d", queue->label, QUEUE_SIZE - queue->count);
#endif

#if LOG_LANG_ENGLISH
    ESP_LOGI(QUEUE_LOG, "%s: The message has been dequeued \n Queue space remaining: %d", queue->label, QUEUE_SIZE - queue->count);
#endif
    return msg;
}

void get_queue_message(user_queue_t *queue, char *str, size_t size)
{
    char *msg = dequeue(queue);
    if (msg == NULL)
    {
        memset(str, 0, size);
        return;
    }

    memcpy(str, msg, strlen(msg));
    str[strlen(msg) + 1] = '\0';
    free((void *)msg);
}