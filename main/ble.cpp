#include "ble.h"


void Nimble_Server::begin(char* name)
{
    InitBle_adv(name,DEFAULT_SERVICE_UUID,DEFAULT_CHAR_UUID,APPEARANCE_GENERIC_ACTUATOR);
          adv_data = {
        .set_scan_rsp = false,
        .include_name = false,
        .include_txpower = false,
        .min_interval = 0x20,
        .max_interval = 0x40,
        .appearance = 0x00,
        .manufacturer_len = 0,
        .p_manufacturer_data = 0,
        .service_data_len = 0,
        .p_service_data = NULL,
        .service_uuid_len = 0,
        .p_service_uuid = NULL,
        .flag = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,
    };
}

bool Nimble_Server::bleSta()
{
    return ESP_BLE.flag.ble_STA;
}

char* Nimble_Server::get_BleData()
{
    get_queue_message(&BLE_QUEUE,(char*)ESP_BLE.content.Message,1024);
    return (char*)ESP_BLE.content.Message;
}

void Nimble_Server::Write(uint8_t *data, size_t size)
{
    // 如果之前已经分配过内存，需要释放
    if (adv_data.p_manufacturer_data != NULL) {
        free(adv_data.p_manufacturer_data);
    }

    // 分配新内存并拷贝数据
    adv_data.p_manufacturer_data = (uint8_t *)malloc(size);
    if (adv_data.p_manufacturer_data == NULL) {
        ESP_LOGE("Nimble_Server", "Memory allocation failed for manufacturer data");
        return;
    }

    memcpy(adv_data.p_manufacturer_data, data, size);
    adv_data.manufacturer_len = size;

    // 配置广播数据
    esp_err_t err = esp_ble_gap_config_adv_data(&adv_data);
    printf("数据已发送");
    if (err != ESP_OK) {
        ESP_LOGE("Nimble_Server", "Failed to configure advertising data, err = %d", err);
    }
}
