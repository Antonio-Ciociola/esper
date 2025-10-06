#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include <vector>

#include "defines.h"
#include "peripherals/display/display.h"
#include "screens/screen.h"
#include "screens/simpleVco/simpleVco.h"
#include "screens/scope/scope.h"
#include "peripherals/encoder/encoder.h"

using namespace std;

static const char *TAG = "graphic";

void (*updateFunction)(int count) = NULL;

bool connector_active = false;


extern "C" void IRAM_ATTR update(int count)
{
    screens[current_screen]->update();
}

extern "C" void startGraphic()
{
    ESP_LOGI(TAG, "Starting graphic");

    // connect_screen = new connectScreen();

    screens.push_back(std::make_unique<simpleVcoScreen>());
    screens.push_back(std::make_unique<scopeScreen>());

    screens[0]->select();

    lvgl_port_lock(0);
    lv_group_set_editing(screens[0]->group, false);
    lvgl_port_unlock();

    turn_display_on_off(true);

    updateFunction = update;
}

extern "C" void runGraphic()
{
    updateEncoder();
    screens[current_screen]->refresh();
}
