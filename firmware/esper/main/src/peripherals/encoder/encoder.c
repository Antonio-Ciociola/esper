#include "freertos/FreeRTOS.h"
#include "esp_log.h"

#include <string.h>
#include <encoder.h>
#include <esp_idf_lib_helpers.h>
#include "esp_lvgl_port.h"

#define RE_A_GPIO 1
#define RE_B_GPIO 2
#define RE_BTN_GPIO 3

#define EV_QUEUE_LEN 32

static QueueHandle_t event_queue;

rotary_encoder_event_t e;

static rotary_encoder_t re;
int val = 0;

static const char *TAG = "Encoder";

void configEncoder(void)
{
    ESP_LOGI(TAG, "Started configEncoder");

    // Create event queue for rotary encoders
    event_queue = xQueueCreate(EV_QUEUE_LEN, sizeof(rotary_encoder_event_t));

    // Setup rotary encoder library
    ESP_ERROR_CHECK(rotary_encoder_init(event_queue));

    // Add one encoder
    memset(&re, 0, sizeof(rotary_encoder_t));
    re.pin_a = RE_A_GPIO;
    re.pin_b = RE_B_GPIO;
    re.pin_btn = RE_BTN_GPIO;
    ESP_ERROR_CHECK(rotary_encoder_add(&re));

    ESP_LOGI(TAG, "Initial value: %d", val);

    rotary_encoder_enable_acceleration(&re, 100);
    // rotary_encoder_disable_acceleration(&re);

    ESP_LOGI(TAG, "Finished configEncoder");
}

void getEncoderValue(int *ret_val, bool *pressed)

{
    while (xQueueReceive(event_queue, &e, 0))
    {
        ESP_LOGD(TAG, "Event: %d, diff: %ld", e.type, e.diff);
        switch (e.type)
        {
        case RE_ET_BTN_PRESSED:
            *pressed = true;
            break;
        case RE_ET_BTN_RELEASED:
            *pressed = false;
            break;
        case RE_ET_BTN_CLICKED:
            break;
        case RE_ET_BTN_LONG_PRESSED:
            break;
        case RE_ET_CHANGED:
            *ret_val -= e.diff;
            break;
        default:
            break;
        }
    }
}

int encoder_diff = 0;
bool encoder_pressed = false;
extern lv_indev_t *indev;

void updateEncoder()
{
    int old_encoder_diff = encoder_diff;
    bool old_encoder_pressed = encoder_pressed;
    getEncoderValue(&encoder_diff, &encoder_pressed);

    if (old_encoder_diff != encoder_diff || old_encoder_pressed != encoder_pressed || encoder_pressed)
    {

        lvgl_port_lock(0);
        lv_indev_read(indev);
        lvgl_port_unlock();
    }
}