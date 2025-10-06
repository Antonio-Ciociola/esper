#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"

#include "esp_lcd_panel_vendor.h"

#include "hal/ledc_types.h"
#include "driver/ledc.h"

#include "defines.h"

void configProbe(){
    // Configure GPIO 13 for LEDC PWM output
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .freq_hz          = 100000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num       = PROB_GPIO,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = 0,
        .hpoint         = 0,
        .flags.output_invert = 0
    };
    ledc_channel_config(&ledc_channel);

}

void setProbeLevel(uint8_t level) {
    if (level > 255) {
        level = 255; // Clamp to max value
    }
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, level);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}