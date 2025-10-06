#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "peripherals/display/display.h"

static const char *TAG = "Lvgl";

extern int encoder_diff;
extern bool encoder_pressed;

lv_indev_t *indev;

// Callback function for the encoder input device
static void encoder_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    data->enc_diff = encoder_diff;
    encoder_diff = 0;
    data->state = encoder_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

// Initialize the encoder as an LVGL input device
void encoderInit(void)
{
    ESP_LOGI(TAG, "Started encoderInit");
    // Initialize the LVGL input device driver

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);
    lv_indev_set_scroll_throw(indev, 1);
    lv_indev_set_read_cb(indev, encoder_read_cb);
    // lv_group_set_focus_cb(group, focus_cb);
    // lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_ACTIVE);

    ESP_LOGI(TAG, "Finished encoderInit");
}

void test_lvgl(void)
{
    ESP_LOGI(TAG, "Testing LVGL");
    lvgl_port_lock(0);
    lv_obj_t *scrn = lv_obj_create(NULL);
    lv_obj_t *label = lv_label_create(scrn);
    lv_label_set_text(label, "Hello, LVGL!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_scr_load(scrn);

    lvgl_port_unlock();
    turn_display_on_off(true);
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Running LVGL task");
        lv_task_handler();
    }
}

void configLvgl(void)
{
    // test_lvgl();
    encoderInit();
}
