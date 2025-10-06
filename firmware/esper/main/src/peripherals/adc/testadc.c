#include "driver/adc.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"

#include "esp_timer.h"
#include "esp_adc/adc_oneshot.h"

#define ADC_WIDTH ADC_WIDTH_BIT_12
#define ADC_ATTEN ADC_ATTEN_DB_11

void testadc()
{
    esp_err_t ret;

    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_WIDTH,
        .atten = ADC_ATTEN,
    };
    // adc_channel_t ADC_CHANNEL = adc_oneshot_io_to_channel(3, &init_config1.unit_id, &ADC_CHANNEL);
    adc_channel_t ADC_CHANNEL = ADC2_CHANNEL_2;
    ESP_LOGI("ADC", "Using ADC channel: %d", ADC_CHANNEL);
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &chan_config));

    while (1)
    {
        // Perform ADC conversion
        int adc_reading;
        int64_t start_time = esp_timer_get_time();
        ret = adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_reading);
        int64_t end_time = esp_timer_get_time();
        ESP_LOGI("ADC", "ADC read time: %lld us", (end_time - start_time));
        ESP_LOGI("ADC", "ADC Reading: %d", adc_reading);
        // Convert ADC reading to voltage
        float voltage = (adc_reading / 4095.0) * 3.3; // Assuming 12-bit resolution and 3.3V reference
        ESP_LOGI("ADC", "Voltage: %.3f V", voltage);
        vTaskDelay(pdMS_TO_TICKS(250)); // Delay for 1 second
    }
}