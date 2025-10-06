#pragma once

#include "screens/screen.h"
#include "update.h"
#include "defines.h"

using namespace std;

#define WIDTH_SCOPE 128
#define HEIGHT_SCOPE 64
#define VALUES_SIZE 256 // The more time refresh takes, the bigger it needs to be

class scopeScreen : public screen
{
public:
    enableAdc enableadc = {{true, false, false, false}};
    lv_obj_t *canvas;
    float *values;
    float *values_copy;
    float window_us = 10000;
    float time = 0;
    float values_time = 0;
    int current = 0;
    int trigger = 100;
    float last = 0;
    float direction = 0;
    bool rolling = true;
    bool full = false;
    scopeScreen();
    void switchMode();
    void IRAM_ATTR update() override;
    void IRAM_ATTR refresh() override;
};
