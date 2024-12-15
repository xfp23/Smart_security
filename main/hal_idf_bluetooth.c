#include "hal_idf_bletooth.h"
#include "user_queue.h"

user_queue_t BLE_QUEUE = {0};

ESP_Class_ble_t ESP_BLE = {0};

bool register_flage = false;
/// 声明静态函数，用于处理GATT事件
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static uint8_t char_str[] = {0x11, 0x22, 0x33}; // 特征值
static esp_gatt_char_prop_t a_property = 0;     // 特征属性

// 特征值的属性定义
static esp_attr_value_t gatts_demo_char1_val = DEFAULT_CONFIG_CHAR;

static uint8_t adv_config_done = 0; // 广播配置完成标志

// 广播服务的UUID
static uint8_t adv_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    // 第一个UUID，16位，[12]，[13] 是值
    0xfb,
    0x34,
    0x9b,
    0x5f,
    0x80,
    0x00,
    0x00,
    0x80,
    0x00,
    0x10,
    0x00,
    0x00,
    0xEE,
    0x00,
    0x00,
    0x00,
    // 第二个UUID，32位，[12]，[13]，[14]，[15] 是值
    0xfb,
    0x34,
    0x9b,
    0x5f,
    0x80,
    0x00,
    0x00,
    0x80,
    0x00,
    0x10,
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
};

// 广播数据定义
static esp_ble_adv_data_t adv_data = DEFAULT_ADV_DATA;

// 扫描响应数据定义
static esp_ble_adv_data_t scan_rsp_data = DEFAULT_SCAN_PARAM;

// 广播参数设置
static esp_ble_adv_params_t adv_params = DEFAULT_ADV_PARAM;

#define PROFILE_NUM 2      // GATT配置文件数量
#define PROFILE_A_APP_ID 0 // 配置文件A的应用ID
#define PROFILE_B_APP_ID 1 // 配置文件B的应用ID

// 存储GATT接口的配置文件数组
static gatts_profile_inst_t gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler, // GATT事件处理函数
        .gatts_if = ESP_GATT_IF_NONE,              // 初始时未获取GATT接口，设置为ESP_GATT_IF_NONE
    }};

void write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
prepare_type_env_t prepare_env;
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {

    // 广告数据设置完成事件
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag); // 更新广告配置标志
        if (adv_config_done == 0)
        {
            // 如果所有广告配置完成，开始广播
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;

    // 扫描响应数据设置完成事件
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag); // 更新扫描响应配置标志
        if (adv_config_done == 0)
        {
            // 如果所有扫描响应配置完成，开始广播
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;

    // 广播启动完成事件
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        // 广播启动完成事件，指示广播启动成功或失败
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(BLE_LOG, "Advertising start failed");
        }
        break;

    // 广播停止完成事件
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            // 广播停止失败
            ESP_LOGE(BLE_LOG, "Advertising stop failed");
        }
        else
        {
            // 广播停止成功
            ESP_LOGI(BLE_LOG, "Stop adv successfully");
        }
        break;

    // 更新连接参数事件
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(BLE_LOG, "update connection params status = %d, min_int = %d, max_int = %d, conn_int = %d, latency = %d, timeout = %d",
                 param->update_conn_params.status,   // 更新状态
                 param->update_conn_params.min_int,  // 最小连接间隔
                 param->update_conn_params.max_int,  // 最大连接间隔
                 param->update_conn_params.conn_int, // 当前连接间隔
                 param->update_conn_params.latency,  // 延迟
                 param->update_conn_params.timeout); // 超时
        break;

    default:
        // 处理所有未处理的事件
        break;
    }
}

static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    // 应用注册事件
    case ESP_GATTS_REG_EVT:
        /*if (!register_flage)
        {
            register_flage = true;*/

            ESP_LOGI(BLE_LOG, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
            // 设置服务的 UUID 和 ID
            gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
            gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = ESP_BLE.uuid.service_uuid; // ESP_BLE 0x00FF

            // 设置设备名称
            esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(ESP_BLE.ble_name);
            ESP_LOGI(BLE_LOG, "BlueTooth name : %s", ESP_BLE.ble_name);
            if (set_dev_name_ret)
            {
                ESP_LOGE(BLE_LOG, "set device name failed, error code = %x", set_dev_name_ret);
            }

            // 配置广告数据
            esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
            if (ret)
            {
                ESP_LOGE(BLE_LOG, "config adv data failed, error code = %x", ret);
            }
            adv_config_done |= adv_config_flag;

            // 配置扫描响应数据
            ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
            if (ret)
            {
                ESP_LOGE(BLE_LOG, "config scan response data failed, error code = %x", ret);
            }
            adv_config_done |= scan_rsp_config_flag;

            // 创建服务
            esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, 4);
     //   }
        break;

    // 处理读请求事件
    case ESP_GATTS_READ_EVT:
    {
        ESP_LOGI(BLE_LOG, "GATT_READ_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        rsp.attr_value.value[0] = 0xde;
        rsp.attr_value.value[1] = 0xed;
        rsp.attr_value.value[2] = 0xbe;
        rsp.attr_value.value[3] = 0xef;
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    }

        // 处理写请求事件
    case ESP_GATTS_WRITE_EVT:
    {
        ESP_LOGI(BLE_LOG, "GATT_WRITE_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep)
        {
            ESP_LOGI(BLE_LOG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            esp_log_buffer_hex(BLE_LOG, param->write.value, param->write.len);
            enqueue(&BLE_QUEUE,(char *)param->write.value);
            //get_queue_message(&BLE_QUEUE,(char *)ESP_BLE.content.Message,1024);
            ESP_BLE.content.size = param->write.len;
            printf("%.*s",ESP_BLE.content.size,ESP_BLE.content.Message);
            //memcpy(ESP_BLE.content.Message, param->write.value, param->write.len);
            // sprintf(ESP_BLE.content.Message,"%.*s",param->write.value, param->write.len);

#if BLE_REVICE_STRING
            ESP_LOGI(BLE_LOG,"BLE MESSAGE : %.*s", ESP_BLE.content.size, ESP_BLE.content.Message);
#elif BLE_REVICE_BYTE
            for (int count = 0; count < ESP_BLE.content.size - 1; count++)
            {
                printf("%4x", ESP_BLE.content.Message[count]);
            }
#endif
            if (gl_profile_tab[PROFILE_A_APP_ID].descr_handle == param->write.handle && param->write.len == 2)
            {
                uint16_t descr_value = param->write.value[1] << 8 | param->write.value[0];
                if (descr_value == 0x0001)
                {
                    if (a_property & ESP_GATT_CHAR_PROP_BIT_NOTIFY)
                    {
                        ESP_LOGI(BLE_LOG, "notify enable");
                        uint8_t notify_data[15];
                        for (int i = 0; i < sizeof(notify_data); ++i)
                        {
                            notify_data[i] = i % 0xff;
                        }
                        // the size of notify_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                    sizeof(notify_data), notify_data, false);
                    }
                }
                else if (descr_value == 0x0002)
                {
                    if (a_property & ESP_GATT_CHAR_PROP_BIT_INDICATE)
                    {
                        ESP_LOGI(BLE_LOG, "indicate enable");
                        uint8_t indicate_data[15];
                        for (int i = 0; i < sizeof(indicate_data); ++i)
                        {
                            indicate_data[i] = i % 0xff;
                        }
                        // the size of indicate_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                    sizeof(indicate_data), indicate_data, true);
                    }
                }
                else if (descr_value == 0x0000)
                {
                    ESP_LOGI(BLE_LOG, "notify/indicate disable ");
                }
                else
                {
                    ESP_LOGE(BLE_LOG, "unknown descr value");
                    esp_log_buffer_hex(BLE_LOG, param->write.value, param->write.len);
                }
            }
        }
        write_event_env(gatts_if, &prepare_env, param);
        ESP_LOGI(BLE_LOG," 写请求事件触发");
        break;
    }

    // 处理执行写事件
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(BLE_LOG, "ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        ESP_LOGI(BLE_LOG," 处理执行写事件触发");
        break;

    // 处理 MTU 更新事件
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(BLE_LOG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        ESP_LOGI(BLE_LOG," 更新MTU事件触发");
        break;

    // 处理注销事件
    case ESP_GATTS_UNREG_EVT:
        break;

    // 处理创建服务事件
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(BLE_LOG, "CREATE_SERVICE_EVT, status %d,  service_handle %d", param->create.status, param->create.service_handle);
        gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = ESP_BLE.uuid.char_uuid; // 0xFF01

        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_A_APP_ID].service_handle);
        a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
        esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                        a_property,
                                                        &gatts_demo_char1_val, NULL);
        if (add_char_ret)
        {
            ESP_LOGE(BLE_LOG, "add char failed, error code =%x", add_char_ret);
        }
        ESP_LOGI(BLE_LOG," 创建服务事件触发");
        break;

    // 处理包含服务事件
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;

    // 处理添加特征事件
    case ESP_GATTS_ADD_CHAR_EVT:
    {
        uint16_t length = 0;
        const uint8_t *prf_char;

        ESP_LOGI(BLE_LOG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d",
                 param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        gl_profile_tab[PROFILE_A_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        esp_err_t get_attr_ret = esp_ble_gatts_get_attr_value(param->add_char.attr_handle, &length, &prf_char);
        if (get_attr_ret == ESP_FAIL)
        {
            ESP_LOGE(BLE_LOG, "ILLEGAL HANDLE");
        }

        ESP_LOGI(BLE_LOG, "the gatts demo char length = %x", length);
        for (int i = 0; i < length; i++)
        {
            ESP_LOGI(BLE_LOG, "prf_char[%x] =%x", i, prf_char[i]);
        }
        esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                               ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
        if (add_descr_ret)
        {
            ESP_LOGE(BLE_LOG, "add char descr failed, error code =%x", add_descr_ret);
        }
        ESP_LOGI(BLE_LOG," 添加特征事件触发");
        break;
    }

    // 处理添加特征描述符事件
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        gl_profile_tab[PROFILE_A_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        ESP_LOGI(BLE_LOG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
                 ESP_LOGI(BLE_LOG," 添加描述符事件触发");
        break;

    // 处理删除服务事件
    case ESP_GATTS_DELETE_EVT:
    ESP_LOGI(BLE_LOG," 删除服务事件触发");
        break;

    // 处理启动服务事件
    case ESP_GATTS_START_EVT:
        ESP_LOGI(BLE_LOG, "SERVICE_START_EVT, status %d, service_handle %d",
                 param->start.status, param->start.service_handle);
                 ESP_LOGI(BLE_LOG," 启动服务事件触发");
        break;

    // 处理停止服务事件
    case ESP_GATTS_STOP_EVT:
    ESP_LOGI(BLE_LOG," 停止服务事件触发");
        break;

    // 处理设备连接事件
    case ESP_GATTS_CONNECT_EVT:
    {
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* 对于 iOS 系统，请参考 Apple 官方文档了解 BLE 连接参数的限制。*/
        conn_params.latency = 0;
        conn_params.max_int = 0x20; // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10; // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;  // timeout = 400*10ms = 4000ms
        ESP_LOGI(BLE_LOG, "ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->connect.conn_id;
        // 开始向对端设备发送更新连接参数请求
        esp_ble_gap_update_conn_params(&conn_params);
        ESP_LOGI(BLE_LOG," 设备已连接");
        ESP_BLE.flag.ble_STA = true;
        break;
    }

    // 处理设备断开事件
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(BLE_LOG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
 ESP_BLE.flag.ble_STA = false;
        ESP_LOGI(BLE_LOG,"设备已断开");
        break;

    // 处理确认事件
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(BLE_LOG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK)
        {
            esp_log_buffer_hex(BLE_LOG, param->conf.value, param->conf.len);
        }
         ESP_LOGI(BLE_LOG,"确认事件已触发");
        break;

    // 其他事件
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* 如果事件是注册事件，则为每个配置文件存储 gatts_if */
    if (event == ESP_GATTS_REG_EVT)
    {
        if (param->reg.status == ESP_GATT_OK)
        {
            // 存储与应用程序 ID 相关的 gatts_if
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        }
        else
        {
            // 注册失败时输出日志
            ESP_LOGI(BLE_LOG, "注册应用程序失败, app_id %04x, 状态 %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }

    /* 如果 gatts_if 等于配置文件 A，则调用配置文件 A 的回调处理函数，
     * 这里调用每个配置文件的回调函数 */
    do
    {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++)
        {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE 表示不指定特定的 gatt_if，需要调用每个配置文件的回调函数 */
                gatts_if == gl_profile_tab[idx].gatts_if)
            {
                if (gl_profile_tab[idx].gatts_cb)
                {
                    // 调用相应配置文件的回调函数处理事件
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

/**
 * @brief 初始化蓝牙广播者
 * @param 蓝牙设备被扫描到的名字，
 * @param 服务uuid
 * @param 特征uuid
 * @param 设备外观
 */
void InitBle_adv(char *ble_name, uint16_t service_uuid, uint16_t char_uuid, int appear)
{
    // memcpy(ESP_BLE.ble_name, ble_name, strlen(ble_name) + 1);
    Init_queue(&BLE_QUEUE,"BLE");
    strncpy(ESP_BLE.ble_name, ble_name, strlen(ble_name) + 1);
    ESP_BLE.uuid.service_uuid = service_uuid;
    ESP_BLE.uuid.char_uuid = char_uuid;
    // ESP_BLE.gatts_profile_inst.gatts_cb = gatts_profile_a_event_handler;
    // ESP_BLE.gatts_profile_inst.gatts_if = ESP_GATT_IF_NONE;
    scan_rsp_data.appearance = appear;
    memset(&ESP_BLE.flag, 0, sizeof(Ble_flag_t));
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);

    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    esp_ble_gatts_register_callback(gatts_event_handler);
    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    esp_ble_gatts_app_register(PROFILE_B_APP_ID);
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret)
    {
        ESP_LOGE(BLE_LOG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    return;
}

void write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    esp_gatt_status_t status = ESP_GATT_OK;
    if (param->write.need_rsp)
    {
        if (param->write.is_prep)
        {
            if (prepare_write_env->prepare_buf == NULL)
            {
                prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
                prepare_write_env->prepare_len = 0;
                if (prepare_write_env->prepare_buf == NULL)
                {
                    ESP_LOGE(BLE_LOG, "No memory for prepare buffer");
                    status = ESP_GATT_NO_RESOURCES;
                }
            }
            else
            {
                if (param->write.offset > PREPARE_BUF_MAX_SIZE)
                {
                    status = ESP_GATT_INVALID_OFFSET;
                }
                else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE)
                {
                    status = ESP_GATT_INVALID_ATTR_LEN;
                }
            }

            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
            if (gatt_rsp != NULL)
            {
                gatt_rsp->attr_value.len = param->write.len;
                gatt_rsp->attr_value.handle = param->write.handle;
                gatt_rsp->attr_value.offset = param->write.offset;
                gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
                memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
                free(gatt_rsp);
            }
            else
            {
                ESP_LOGE(BLE_LOG, "malloc failed for gatt_rsp");
            }

            if (status == ESP_GATT_OK)
            {
                memcpy(prepare_write_env->prepare_buf + param->write.offset, param->write.value, param->write.len);
                prepare_write_env->prepare_len += param->write.len;
            }
        }
        else
        {
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, NULL);
        }
    }
}

