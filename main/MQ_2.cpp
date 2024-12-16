
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
      MQ2_Channel = channel;
    adc_oneshot_unit_init_cfg_t adc_cfg = {
        .unit_id = unit,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_cfg, &adc_handle));
    adc_oneshot_chan_cfg_t adc_chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
  
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel, &adc_chan_cfg));
}
/**
 * @brief 读取烟雾浓度百分比
 *
 * @return 返回烟雾浓度百分比，范围是 0-100%
 */
float MQ_2_Class::read()
{
    const int ADC_MAX = 4095; // 12位ADC的最大值
    const float Vref = 3.3;   // 参考电压（根据实际供电调整）
    const float R_L = 10.0;   // 负载电阻，单位 kΩ（根据电路设计调整）
    const float R0 = 10.0;    // 传感器在干净空气中的电阻值，单位 kΩ（需校准）
    const float SCALE_FACTOR = 100.0; // 将比例换算为百分比

    // 获取 ADC 原始值
    uint16_t rawValue = getAnalog();

    // 限制 ADC 范围
    if (rawValue > ADC_MAX)
    {
        rawValue = ADC_MAX;
    }

    // 计算输出电压
    float Vout = (static_cast<float>(rawValue) / ADC_MAX) * Vref;

    // 计算传感器电阻 Rs
    float Rs = R_L * (Vref - Vout) / Vout;

    // 计算 Rs/R0 比值
    float ratio = Rs / R0;

    // 将 Rs/R0 比值转换为浓度百分比 (简单比例关系)
    float smokePercentage = (1.0 / ratio) * SCALE_FACTOR;

    // 限制百分比在合理范围内
    if (smokePercentage > 100.0)
        smokePercentage = 100.0;
    else if (smokePercentage < 0.0)
        smokePercentage = 0.0;

    return smokePercentage;
}


uint16_t MQ_2_Class::getAnalog()
{
    int result;
esp_err_t ret = adc_oneshot_read(adc_handle,MQ2_Channel,&result);
if (ret != ESP_OK) {
    printf("ADC read failed: %s\n", esp_err_to_name(ret));
    //result = 0; // 返回一个默认值
}
printf("ADC Value : %d Vol : %.2f\n",result,result *3.3/4096);
return (uint16_t)result;

}

MQ_2_Class::~MQ_2_Class()
{
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}