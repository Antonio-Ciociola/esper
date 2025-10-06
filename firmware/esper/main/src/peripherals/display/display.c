#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"

#include "esp_lcd_panel_vendor.h"

static const char *TAG = "Display";

#define I2C_BUS_PORT-1
#define LCD_PIXEL_CLOCK_HZ (400 * 1000)
#define LCD_SDA 7
#define LCD_SCL 8

#define PIN_NUM_RST -1
#define I2C_HW_ADDR 0x3C

#define LCD_H_RES 128
#define LCD_V_RES 64

// Bit number used to represent command and parameter
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

esp_lcd_panel_handle_t panel_handle = NULL;

void turn_display_on_off(bool state){
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, state));
}

void configDisplay(void)
{
    ESP_LOGI(TAG, "Started configDisplay");

    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = LCD_SDA,
        .scl_io_num = LCD_SCL,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = I2C_HW_ADDR,
        .scl_speed_hz = LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,       // According to SSD1306 datasheet
        .lcd_cmd_bits = LCD_CMD_BITS,   // According to SSD1306 datasheet
        .lcd_param_bits = LCD_CMD_BITS, // According to SSD1306 datasheet
        .dc_bit_offset = 6,             // According to SSD1306 datasheet
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = PIN_NUM_RST,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = LCD_V_RES,
    };

    panel_config.vendor_config = &ssd1306_config;
    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, false));

    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));


    ESP_LOGI(TAG, "Initialize LVGL");
    lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_cfg.task_affinity = 0; // Pin LVGL task to core 0
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_H_RES * LCD_V_RES ,
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = true,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = true,
            .mirror_y = true,
        },
        .flags = {
            .buff_dma = true, .buff_spiram = false, .swap_bytes = false, .full_refresh = true,//see
            .sw_rotate = false, // true: software; false: hardware
            .direct_mode = false,
        },
};

    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);

    

    lvgl_port_lock(0);

    lv_disp_set_rotation(disp, LV_DISPLAY_ROTATION_0);

    lv_theme_t *th = lv_theme_mono_init(disp, true, LV_FONT_DEFAULT);
    /* Set the mono system theme */
    if (th == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize mono theme.");
        return;
    }

    // Set the theme for the display
    lv_display_set_theme(disp, th);

    lv_display_set_default(disp);

    // turn_display_on_off(true);

    lvgl_port_unlock();

    ESP_LOGI(TAG, "Finished configDisplay");
}
