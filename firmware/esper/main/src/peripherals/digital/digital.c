#include "driver/gpio.h"
#include "esp_log.h"

#include "defines.h"

static const char *TAG = "Digital";

void configDigital(void)
{
    ESP_LOGI(TAG, "Started configDigital");

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PROB_GPIO),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);

    gpio_set_level(PROB_GPIO, 1);

    ESP_LOGI(TAG, "Finished configDigital");
}