#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "sdkconfig.h"

#include "esp_timer.h"

#include "driver/spi_master.h"

#include "adcSpi.h"

#include <stdio.h>
#include <esp_log.h>

#include "peripherals/adcSpi/ads8688.h"
#include "peripherals/adcSpi/adcSpi.h"
#include "peripherals/probe/probe.h"

#define SDO_IO 12
#define SDI_IO 11
#define SCLK_IO 10
#define CS_IO 9

#define HSPI_HOST SPI2_HOST
#define FREQ_HZ 8000000 /*!< SPI clock frequency */

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

static const char *TAG = "adcSpi";

// ADS8688 Command Definitions

spi_device_handle_t spi_handle;

DRAM_ATTR static uint8_t buffer1[256];
DRAM_ATTR static uint8_t buffer2[256];

struct enableAdc enabledAdc = {{true, true, true, true}};

esp_err_t write_register(uint8_t reg_addr, uint8_t data)
{
    uint8_t tx_buf[3] = {
        ((reg_addr & 0x7F) << 1) | CMD_WRITE_REG, // Command byte
        data,                                     // Data byte
        0x00                                      // Dummy byte (required by device)
    };
    uint8_t rx_buf[3] = {0};

    spi_transaction_t t = {
        .length = 24,
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf,
        .flags = 0,
    };

    esp_err_t ret = spi_device_transmit(spi_handle, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write register 0x%02X: %s", reg_addr, esp_err_to_name(ret));
        return ret;
    }
    return ret;
}

esp_err_t read_register(uint8_t reg_addr, uint8_t *data_out)
{
    uint8_t tx_buf[3] = {
        ((reg_addr & 0x7F) << 1) | CMD_READ_REG,
        0x00,
        0x00};
    uint8_t rx_buf[3] = {0};

    spi_transaction_t t = {
        .length = 24,
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf,
        .flags = 0,
    };

    esp_err_t ret = spi_device_transmit(spi_handle, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read register 0x%02X: %s", reg_addr, esp_err_to_name(ret));
        return ret;
    }

    *data_out = rx_buf[1] << 7 | rx_buf[2] >> 1;
    // ESP_LOGI(TAG, "Read register 0x%02X: 0x%02X", reg_addr, *data_out);
    return ret;
}

static esp_err_t send_command(uint16_t com, uint16_t *data_out)
{
    uint8_t tx_buf[] = {
        (com >> 8) & 0xFF,
        com & 0xFF,
        0, 0};
    uint8_t rx_buf[] = {0, 0, 0, 0};

    spi_transaction_t t = {
        .length = 32,
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf,
        .flags = 0,
    };

    esp_err_t ret = spi_device_transmit(spi_handle, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send command 0x%04X: %s", com, esp_err_to_name(ret));
        return ret;
    }
    if (data_out != NULL)
        *data_out = rx_buf[1] << 15 | rx_buf[2] << 7 | rx_buf[3] >> 1;

    return ret;
}
static IRAM_ATTR esp_err_t send_command_polling(uint16_t com, uint16_t *data_out)
{

    spi_transaction_t t = {
        .length = 32,
        .tx_data = {
            (com >> 8) & 0xFF, // Command high byte
            com & 0xFF,        // Command low byte
            0,                 // Dummy byte
            0                  // Dummy byte
        },
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
    };

    esp_err_t ret = spi_device_polling_transmit(spi_handle, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send command 0x%04X: %s", com, esp_err_to_name(ret));
        return ret;
    }

    if (data_out != NULL)
    {
        uint8_t *rx_buf = (uint8_t *)t.rx_data;
        *data_out = rx_buf[1] << 15 | rx_buf[2] << 7 | rx_buf[3] >> 1;
    }

    return ret;
}

DRAM_ATTR spi_transaction_t trans_buffer[8]; // Buffer for transactions

static IRAM_ATTR esp_err_t queue_read()
{
    static int count = 0;

    spi_transaction_t *t = &trans_buffer[count];

    t->length = 32; // 32 bits for a full read transaction
    t->length = 32;
    t->tx_data[0] = 0;
    t->tx_data[1] = 0;
    t->tx_data[2] = 0;
    t->tx_data[3] = 0; // Dummy data for the transaction
    t->flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    esp_err_t ret = spi_device_queue_trans(spi_handle, t, 0);
    count = (count + 1) % 8;

    if (ret != ESP_OK)
    {

        ESP_LOGE(TAG, "Failed to queue read transaction: %s", esp_err_to_name(ret));
        return ret;
    }

    return ret;
}

static esp_err_t get_read_result(uint16_t *data_out)
{
    spi_transaction_t *t;
    esp_err_t ret = spi_device_get_trans_result(spi_handle, &t, portMAX_DELAY);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get read result: %s", esp_err_to_name(ret));
        return ret;
    }
    if (t->rx_buffer == NULL)
    {
        ESP_LOGE(TAG, "Received buffer is NULL");
        return ESP_FAIL;
    }
    if (data_out != NULL)
    {
        uint8_t *rx_buf = (uint8_t *)t->rx_data;
        *data_out = rx_buf[1] << 15 | rx_buf[2] << 7 | rx_buf[3] >> 1;
    }
    return ret;
}

static IRAM_ATTR esp_err_t queue_read_polling()
{
    static int count = 0;

    spi_transaction_t *t = &trans_buffer[count];

    t->length = 32; // 32 bits for a full read transaction
    t->length = 32;
    t->tx_data[0] = 0;
    t->tx_data[1] = 0;
    t->tx_data[2] = 0;
    t->tx_data[3] = 0; // Dummy data for the transaction
    t->flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    esp_err_t ret = spi_device_polling_start(spi_handle, t, portMAX_DELAY);
    count = (count + 1) % 8;

    if (ret != ESP_OK)
    {

        ESP_LOGE(TAG, "Failed to queue read transaction: %s", esp_err_to_name(ret));
        return ret;
    }

    return ret;
}

static esp_err_t get_read_result_polling(uint16_t *data_out)
{
    static int count = 0;
    spi_transaction_t *t = &trans_buffer[count]; // Get the last transaction
    esp_err_t ret = spi_device_polling_end(spi_handle, portMAX_DELAY);
    count = (count + 1) % 8; // Update count for the next transaction
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get read result: %s", esp_err_to_name(ret));
        return ret;
    }
    if (t->rx_buffer == NULL)
    {
        ESP_LOGE(TAG, "Received buffer is NULL");
        return ESP_FAIL;
    }
    if (data_out != NULL)
    {
        uint8_t *rx_buf = (uint8_t *)t->rx_data;
        *data_out = rx_buf[1] << 15 | rx_buf[2] << 7 | rx_buf[3] >> 1;
    }
    return ret;
}

float adc_value_to_voltage(uint16_t adc_value)
{
    float vref = 10.0f;
    float voltage = (adc_value - 32768) * vref / 32767.0f; // 65535 for 16-bit ADC

    return voltage;
}

DRAM_ATTR uint16_t inVal[4];
DRAM_ATTR float inValF[4];

void setZero(int index)
{
    if (index < 0 || index >= 4)
    {
        ESP_LOGE(TAG, "Index out of bounds: %d", index);
        return;
    }
    inVal[index] = 32768; // Set to zero for 16-bit ADC
    inValF[index] = 0.0f;
}

void start_full_conversion()
{
    queue_read();
    queue_read();
    queue_read();
    queue_read();
}

void finish_full_conversion()
{
    get_read_result(&inVal[0]);
    get_read_result(&inVal[1]);
    get_read_result(&inVal[2]);
    get_read_result(&inVal[3]);
}

void get_full_conversion()
{
    int64_t start_time = esp_timer_get_time(); // Get start time
    queue_read();
    queue_read();
    queue_read();
    queue_read();
    int64_t end_time = esp_timer_get_time();    // Get end time
    int64_t time_queue = end_time - start_time; // Calculate elapsed time

    start_time = esp_timer_get_time(); // Get start time for results
    get_read_result(&inVal[0]);
    get_read_result(&inVal[1]);
    get_read_result(&inVal[2]);
    get_read_result(&inVal[3]);
    end_time = esp_timer_get_time();          // Get end time for results
    int64_t time_get = end_time - start_time; // Calculate elapsed time

    ESP_LOGI(TAG, "Queue time: %lld us, Get result time: %lld us, freq: %.0f hz", time_queue, time_get,
             4e6 / (time_queue + time_get));
}

static SemaphoreHandle_t adc_mutex;

void unlock_adc()
{
    if (xSemaphoreGive(adc_mutex) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to unlock ADC mutex");
        abort();
    }
}

IRAM_ATTR void get_full_conversion_polling()
{
    xSemaphoreTake(adc_mutex, portMAX_DELAY);

    for (int i = 0; i < 4; i++)
    {
        if (enabledAdc.in[i])
            send_command_polling(NO_OP, &inVal[i]);
        else
            setZero(i);
    }

    for (int i = 0; i < 4; i++)
    {
        inValF[i] = adc_value_to_voltage(inVal[i]);
    }

    xSemaphoreGive(adc_mutex);
}

void configAdcSpi()
{
    ESP_LOGI(TAG, "Starting ADC SPI configuration");

    // Initialize the SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num = SDO_IO,
        .mosi_io_num = SDI_IO,
        .sclk_io_num = SCLK_IO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_DISABLED));
    ESP_LOGI(TAG, "SPI bus initialized");

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = FREQ_HZ,
        .mode = 1,
        .spics_io_num = CS_IO,
        .queue_size = 8,
        .flags = 0,
    };

    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle));

    send_command(RST, NULL);
    write_register(AUTO_SEQ_EN, 0b1111);
    send_command(AUTO_RST, NULL);

    adc_mutex = xSemaphoreCreateMutex();

    ESP_LOGI(TAG, "ADC SPI configuration complete");
}

void configAdcEnabled(struct enableAdc enable)
{
    xSemaphoreTake(adc_mutex, portMAX_DELAY);
    enabledAdc = enable;
    uint8_t reg = 0x00;
    if (enable.in[0])
        reg |= 0b00000001; // Enable channel 0
    if (enable.in[1])
        reg |= 0b00000010; // Enable channel 1
    if (enable.in[2])
        reg |= 0b00000100; // Enable channel 2
    if (enable.in[3])
        reg |= 0b00001000; // Enable channel 3
    ESP_LOGI(TAG, "Enabling ADC channels: 0x%02X", reg);
    esp_err_t ret = write_register(AUTO_SEQ_EN, reg);
    send_command(AUTO_RST, NULL);
    xSemaphoreGive(adc_mutex);

    return;
}

void handleProbe()
{
    const int seq_length = 11;
    const int max_counter = 10;
    const int range_divisor = 4;

    int sequence[] = {1, 3, 2, 1, 2, 1, 3, 2, 1, 2, 3};
    static int counters[] = {0, 0, 0, 0};
    static int index = 0;

    for (int i = 0; i < 4; i++)
    {
        int value = inValF[i] * 256 / 3.3;
        if (value > (sequence[index] - 0.5) * 256 / range_divisor && value < (sequence[index] + 0.5) * 256 / range_divisor)
        {
            counters[i]= min(counters[i] + 1, max_counter);
        }
        else
        {
            counters[i] = max(counters[i] - 1, 0);
        }

        if (counters[i] > max_counter / 2)
            setZero(i);

        // if (counters[i] == 5)
        //     ESP_LOGI(TAG, "Channel %d: Read value = %d, Wanted value = %d", i, value, sequence[index] * 256 / range_divisor);
    }

    index = (index + 1) % seq_length;

    int level = sequence[index] * 256 / range_divisor;
    setProbeLevel(level);
}