/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "sdkconfig.h"

#include "update.h"

#include "math.h"
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"

#include "peripherals/i2s/i2s.h"

#include "defines.h"

#include "utils/values.h"

char *TAG = "I2S";

static i2s_chan_handle_t tx_handle;

void configI2s()
{

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);

    // chan_cfg.dma_desc_num = 4;
    chan_cfg.dma_desc_num = 8;
    chan_cfg.dma_frame_num = BLOCK_SIZE; // BLOCK_SIZE frames of 16-bit samples per channel, 2 channels = 48 bytes per frame

    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_6,
            .ws = GPIO_NUM_4,
            .dout = GPIO_NUM_5,
            .din = GPIO_NUM_5,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    i2s_channel_init_std_mode(tx_handle, &std_cfg);

    i2s_channel_enable(tx_handle);

    // testi2s();
}

// void sendI2sData()
// {
//     // for (int i = 0; i < BLOCK_SIZE; i++)
//     // {
//     //     outFrames[i].out = 0;
//     //     outFrames[i].aux = 0;
//     // }

//     size_t bytes_to_write = sizeof(outFrames);
//     size_t bytes_written = 0;

//     // TODO
//     i2s_channel_write(tx_handle, outFrames, bytes_to_write, &bytes_written, portMAX_DELAY);
// }

extern struct Frame outFrames[BLOCK_SIZE];

void sendI2sData()
{
    static int j = 0;
    int16_t buf[BLOCK_SIZE * 2];

    // int64_t start = esp_timer_get_time();

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        // int max = 100;
        // float val = ((float)j / (max / 2)) - 1.;
        // val = val * 0.5;
        // if (val < 0.)
        // val = -val;
        // val = -val;
        // buf[i * 2] = (int16_t)(val * 32767);     // Left channel
        // buf[i * 2 + 1] = (int16_t)(val * 32767); // Right channel
        // j = (j + 1) % max;
        buf[i * 2] = (int16_t)(-outFrames[i].out * 32767);     // Left channel
        buf[i * 2 + 1] = (int16_t)(-outFrames[i].aux * 32767); // Right channel
    }
    size_t bytes_to_write = sizeof(buf);
    size_t bytes_written = 0;
    // TODO
    i2s_channel_write(tx_handle, buf, bytes_to_write, &bytes_written, portMAX_DELAY);
    // memcpy(dma_buf->buf, buf, dma_buf->size);

    // int64_t end = esp_timer_get_time();
    // ESP_LOGI(TAG, "Wrote %d bytes in %lld µs", bytes_written, end - start);
}

void testi2s()
{

    int16_t buf[BLOCK_SIZE * 2];

    int j = 0;
    while (1)
    {
        /* Fill the buffer with data */
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            int max = 100;
            float val = ((float)j / (max / 2)) - 1.;
            // val = val * 0.5;
            // if (val < 0.)
            // val = -val;
            val = -val;
            buf[i * 2] = (int16_t)(val * 32767);     // Left channel
            buf[i * 2 + 1] = (int16_t)(val * 32767); // Right channel
            j = (j + 1) % max;
        }

        size_t bytes_to_write = sizeof(buf);
        size_t bytes_written = 0;

        i2s_channel_write(tx_handle, buf, bytes_to_write, &bytes_written, portMAX_DELAY);

        // ESP_LOGI(TAG, "Wrote %zu bytes to I2S channel", bytes_written);
    }

    /* If the configurations of slot or clock need to be updated,
     * stop the channel first and then update it */
    // i2s_channel_disable(tx_handle);
    // std_cfg.slot_cfg.slot_mode = I2S_SLOT_MODE_MONO; // Default is stereo
    // i2s_channel_reconfig_std_slot(tx_handle, &std_cfg.slot_cfg);
    // std_cfg.clk_cfg.sample_rate_hz = 96000;
    // i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg);

    /* Have to stop the channel before deleting it */
    // i2s_channel_disable(tx_handle);
    /* If the handle is not needed any more, delete it to release the channel resources */
    // i2s_del_channel(tx_handle);u
}