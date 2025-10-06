
#include "i2cscan.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

void i2cscan(int scl, int sda)
{
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = -1,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    ESP_LOGI("I2C Scan", "Scanning I2C bus...");
    for (int addr = 0; addr < 128; addr++)
    {
        if (addr == 0x00 || addr == 0x78) // Skip reserved addresses
            continue;

        esp_err_t ret = i2c_master_probe(i2c_bus, addr, 1000);
        if (ret == ESP_OK)
        {
            ESP_LOGI("I2C Scan", "Device found at address: 0x%02X", addr);
        }
    }

    ESP_ERROR_CHECK(i2c_del_master_bus(i2c_bus));
}