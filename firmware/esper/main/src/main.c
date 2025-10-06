#include "freertos/FreeRTOS.h"
#include "esp_log.h"

#include "peripherals/display/display.h"
#include "peripherals/encoder/encoder.h"
#include "peripherals/digital/digital.h"
#include "peripherals/probe/probe.h"
#include "peripherals/lvgl/lvgl.h"
#include "peripherals/i2s/i2s.h"
#include "peripherals/adcSpi/adcSpi.h"
#include "peripherals/adc/testadc.h"

#include "utils/benchmark.h"
#include "utils/i2cscan.h"

#include "update.h"
#include "graphic.h"

static const char *TAG = "espvco";

void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(2000));

    ESP_LOGI(TAG, "Main running on core %d", xPortGetCoreID());

    configDisplay();
    configEncoder();
    configLvgl();
    configDigital();
    configProbe();
    configAdcSpi();
    startGraphic();
    configI2s();
    configUpdate();

    // ESP_LOGI("MEM", "Free heap size: %" PRIu32 " KB", esp_get_free_heap_size() / 1024);
    // ESP_LOGI("MEM", "Free DRAM: %zu KB", heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024);
    // ESP_LOGI("MEM", "Free IRAM: %zu KB", heap_caps_get_free_size(MALLOC_CAP_EXEC) / 1024);
    // ESP_LOGI("MEM", "Free PSRAM: %zu KB", heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024);

    while (1)
    {
        // Make it non blocking
        runGraphic();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // i2cscan(8, 7);
}
    