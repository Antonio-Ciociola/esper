#include "freertos/FreeRTOS.h"
#include "driver/gptimer.h"
#include "driver/dac_oneshot.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#include "defines.h"
#include "update.h"
#include "utils/values.h"
#include "peripherals/adcSpi/adcSpi.h"

#include "peripherals/i2s/i2s.h"

#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#define LOG_UPDATE_TIME false

extern adc_channel_t pwm1_chan, pwm2_chan;

struct Frame outFrames[BLOCK_SIZE];
DRAM_ATTR bool enabledac = true;
DRAM_ATTR int updateFreq = 48000;
extern uint16_t inVal[4];
extern float inValF[4];

static const char *TAG = "update";

extern void (*updateFunction)(int count);

TaskHandle_t update_task_handle;

bool log_enabled = false;

int64_t updtimesarray[FREQUENCY] = {0};

static void IRAM_ATTR printstats(int64_t *timesarray){
    int64_t sum = 0;
    int64_t min = timesarray[0];
    int64_t max = timesarray[0];
    for (int i = 0; i < FREQUENCY; i++) {
        int64_t val = timesarray[i];
        sum += val;
        if (val < min) min = val;
        if (val > max) max = val;
    }
    float mean = (float)sum / FREQUENCY;

    // Calculate standard deviation
    float variance = 0.0f;
    for (int i = 0; i < FREQUENCY; i++) {
        float diff = (float)timesarray[i] - mean;
        variance += diff * diff;
    }
    variance /= FREQUENCY;
    float stddev = sqrtf(variance);

    ESP_LOGI(TAG, "Update stats: mean=%.2f us, min=%lld us, max=%lld us, stddev=%.2f us", mean, min, max, stddev);
}


static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR update_task(void *pvParameters)
{
    static int count = 0;

    while (1)
    {
        int64_t start_time = esp_timer_get_time();

        get_full_conversion_polling();
        handleProbe();

        taskENTER_CRITICAL(&spinlock);   // disable task preemption
        int64_t after_adc = esp_timer_get_time();

        if (updateFunction != NULL)
        {
            updateFunction(count);
        }
        int64_t after_update = esp_timer_get_time();
        taskEXIT_CRITICAL(&spinlock);    // re-enable task preemption

        sendI2sData();
        int64_t after_set = esp_timer_get_time();

        updtimesarray[count] = after_update - after_adc;

        if (LOG_UPDATE_TIME && count == FREQUENCY - 1)
        {
            int64_t adc_time = after_adc - start_time;
            int64_t update_time = after_update - after_adc;
            int64_t set_time = after_set - after_update;
            int64_t total_time = after_set - start_time;
            // ESP_LOGI(TAG, "Update running on core %d", xPortGetCoreID());
            ESP_LOGI(TAG, "Elapsed ADC: %lld µs\nElapsed update: %lld µs\nElapsed set: %lld µs\nElapsed total: %lld µs\n",
                     adc_time, update_time, set_time, total_time);
            // ESP_LOGI(TAG, "Frequency: %d Hz", FREQUENCY);
            // ESP_LOGI(TAG, "ADC: %d %d %d %d", inVal[0], inVal[1], inVal[2], inVal[3]);
            ESP_LOGI(TAG, "ADC: %f %f %f %f", inValF[0], inValF[1], inValF[2], inValF[3]);
            // ESP_LOGI(TAG, "Elapsed update: %lld µs", after_update - after_adc);
            // ESP_LOGI(TAG, "Elapsed set: %lld µs", after_set - after_update);
            // ESP_LOGI(TAG, "Elapsed total: %lld µs\n", after_set - start_time);
            printstats(updtimesarray);
        }

        count = (count + 1) % FREQUENCY;
    }
}

static bool IRAM_ATTR on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(update_task_handle, &xHigherPriorityTaskWoken);
    return xHigherPriorityTaskWoken == pdTRUE; // Yield after ISR if needed
}

void configUpdate(void)
{

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION,
        .intr_priority = 3,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = TIMER_COUNT,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm_cb,
    };
    // ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    // ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    xTaskCreatePinnedToCore(update_task, "UpdateTask", 4096, NULL, 24, &update_task_handle, 1); // Pin to core 1

    // ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // ESP_ERROR_CHECK(gptimer_start(gptimer));

    ESP_LOGI(TAG, "finished initUpdate");
}
