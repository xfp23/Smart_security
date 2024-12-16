#ifndef NIMBLE_H
#define NIMBLE_H

#include "user_queue.h"
#include "hal_idf_bletooth.h"

extern user_queue_t BLE_QUEUE;


class Nimble_Server {
public:
    // 初始化，服务 蓝牙名称 uuid 和特征 uuid
    void begin(char* name);
    char * get_BleData();
    bool bleSta();
    void Write(uint8_t *data,size_t size);
private:
esp_ble_adv_data_t adv_data;
};


#endif