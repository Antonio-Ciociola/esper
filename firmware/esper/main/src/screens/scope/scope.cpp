#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "defines.h"
#include "scope.h"
#include "math.h"

using namespace std;

static const char *TAG = "Scope";

static DRAM_ATTR uint8_t *buffer;

void canvas_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_current_target(e);
    scopeScreen *scope = (scopeScreen *)lv_event_get_user_data(e); // Get instance

    lv_event_code_t code = lv_event_get_code(e);
    // ESP_LOGI(TAG, "Event code: %d", (int)code);
    if (code == LV_EVENT_KEY)
    {
        uint32_t key = lv_event_get_key(e);
        if (key == LV_KEY_LEFT)
            scope->window_us = min(10000000., scope->window_us * 1.3); // Zoom out, ensure div is at least 1
        else if (key == LV_KEY_RIGHT)
            scope->window_us = max(500., scope->window_us / 1.3); // Zoom out, ensure div is at least 1
    }
    else if (code == LV_EVENT_SHORT_CLICKED)
        scope->switchMode();
}

scopeScreen::scopeScreen()
{

    screen::enableadc = enableadc;

    values = static_cast<float *>(heap_caps_malloc(VALUES_SIZE * sizeof(float), MALLOC_CAP_INTERNAL));
    values_copy = static_cast<float *>(heap_caps_malloc(WIDTH_SCOPE * sizeof(float), MALLOC_CAP_INTERNAL));

    for (int i = 0; i < VALUES_SIZE; i++)
    {
        values[i] = 0;
    }

    lv_obj_set_scrollbar_mode(scrn, LV_SCROLLBAR_MODE_OFF);

    t = new title(scrn, group, "SCOPE");

    lvgl_port_lock(0);
    canvas = lv_canvas_create(scrn);
    // buffer = static_cast<uint8_t *>(lv_malloc(WIDTH_SCOPE * HEIGHT_SCOPE * 2));
    buffer = static_cast<uint8_t *>(heap_caps_malloc(WIDTH_SCOPE * HEIGHT_SCOPE * 2, MALLOC_CAP_INTERNAL));

    lv_canvas_set_buffer(canvas, buffer, WIDTH_SCOPE, HEIGHT_SCOPE, LV_COLOR_FORMAT_RGB565);
    lv_obj_align(canvas, LV_ALIGN_TOP_MID, 0, 15);
    lv_canvas_fill_bg(canvas, COLOR_BG, LV_OPA_COVER);

    lv_obj_add_flag(canvas, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(canvas, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_flag(canvas, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_add_event_cb(canvas, canvas_event_cb, LV_EVENT_ALL, this); // Handle rotation
    lv_group_add_obj(group, canvas);

    lvgl_port_unlock();
}

void IRAM_ATTR scopeScreen::update()
{
    float val = inValF[0];

    // ESP_LOGI(TAG, "Scope value: %f", val);

    float lastVal = last;
    last = val;

    direction = direction + (val - lastVal);
    direction = fmaxf(-0.3, fminf(0.3, direction));

    if (rolling)
    {

        time += TIMER_PERIOD_MICRO;
        while (time > values_time && current != VALUES_SIZE)
        {
            values[current] = val;
            values_time += window_us / WIDTH_SCOPE;
            current = current + 1;
        }
        if (current == VALUES_SIZE)
        {
            current = 0;
            values_time -= time;
            time = 0;
        }
    }
    else
    {
        if (full)
            return;

        if (current == 0)
        {
            if (val > 1.1 || val < 0.9 || direction < 0.25)
            {
                return;
            }
        }

        time += TIMER_PERIOD_MICRO;
        while (time > values_time && current != WIDTH_SCOPE)
        {
            values[current] = val;
            values_time += window_us / WIDTH_SCOPE;
            current = current + 1;
        }
        if (current == WIDTH_SCOPE)
        {
            current = 0;
            values_time = 0;
            time = 0;
            full = true;
        }
    }
}

void scopeScreen::switchMode()
{
    rolling = !rolling;
    current = 0;
    full = false;
    values_time = 0;
    time = 0;
}

void IRAM_ATTR scopeScreen::refresh()
{

    // static int64_t last_refresh_time = esp_timer_get_time();
    // int64_t current_time = esp_timer_get_time();
    // int64_t elapsed_time = current_time - last_refresh_time;
    // last_refresh_time = current_time;

    // ESP_EARLY_LOGI(TAG, "Elapsed time refresh: %lld ms", elapsed_time/1000);

    // Clear the canvas

    int start;

    if (rolling)
    {
        start = (current - WIDTH_SCOPE + VALUES_SIZE) % VALUES_SIZE;
        // stop = current;
    }
    else
    {
        if (!full)
            return;
        start = 0;
        // stop = WIDTH_SCOPE;
    }

    for (int i = 0; i < WIDTH_SCOPE; i++)
    {
        values_copy[i] = values[(start + i) % VALUES_SIZE];
    }

    lvgl_port_lock(0);

    lv_canvas_fill_bg(canvas, COLOR_BG, LV_OPA_COVER);

    if (1)
    {
        float min_val = values_copy[0];
        float max_val = values_copy[0];
        for (int i = 1; i < WIDTH_SCOPE; i++)
        {
            if (values_copy[i] < min_val)
                min_val = values_copy[i];
            if (values_copy[i] > max_val)
                max_val = values_copy[i];
        }

        float scale = (max_val - min_val);
        if (scale < 1e-6)
            scale = 1.0f; // Prevent division by zero

        // Normalize values_copy to range [-10, 10]
        for (int i = 0; i < WIDTH_SCOPE; i++)
        {
            values_copy[i] = -9.5f + 19.0f * (values_copy[i] - min_val) / scale;
        }
    }

    for (int i = 0; i < WIDTH_SCOPE; i++)
    {

        int y = ((0.5 + values_copy[i] / 20) * (HEIGHT_SCOPE - 1));

        // int y = ((values_copy[i] / 10) * (HEIGHT_SCOPE - 1));
        // y = fmax(0, y); // Ensure y is not less than 0

        y = (HEIGHT_SCOPE - 1) - y;
        y = min(HEIGHT_SCOPE - 1, max(0, y));
        // y = max(0, y);
        lv_canvas_set_px(canvas, i, y, COLOR_FG, LV_OPA_COVER);
    }

    if (!rolling)
        full = false;

    lvgl_port_unlock();
}
