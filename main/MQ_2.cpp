#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"
#include "MQ_2.h"
#include "esp_err.h"

/**
 * @brief 初始化MQ2
 *
 * @param 指定ADC1 or ADC2
 * @param 指定ADC采样通道
 */
void MQ_2_Class::begin(adc_unit_t unit, adc_channel_t channel)
{
    adc_oneshot_unit_init_cfg_t adc_cfg = {
        .unit_id = unit,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_cfg, &adc_handle));
    adc_oneshot_chan_cfg_t adc_chan_cfg = {
        .atten = ADC_ATTEN_DB_0,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel, &adc_chan_cfg));
}

/**
 * @brief 读取烟雾百分比
 *
 * @return 返回烟雾百分比 转化成百分比需要乘以100
 */
float MQ_2_Class::read()
{
    const int ADC_MAX = 4095;
    int rawValue = getAnalog();
    if (rawValue < 0)
        rawValue = 0;
    if (rawValue > ADC_MAX)
        rawValue = ADC_MAX;

    return static_cast<float>(rawValue) / ADC_MAX;
}

uint16_t MQ_2_Class::getAnalog()
{
    int result;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_9, &result));
    return (uint16_t)result;
}

MQ_2_Class::~MQ_2_Class()
{
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}