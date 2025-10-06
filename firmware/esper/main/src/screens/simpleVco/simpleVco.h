#pragma once

#include "screens/screen.h"
#include "update.h"
#include "defines.h"

#include "engines/waveengines.h"
#include "engines/simple.h"
#include "plaits/dsp/oscillator/oscillator.h"
#include "engines/plaits.h"


using namespace std;

#define PITCH_TABLE_SIZE 4096

class simpleVcoScreen : public screen
{
public:
    enableAdc enableadc = {{true, true, true, true}};
    int freq = 100;
    int engine = 0;
    int amplitude = 80;
    int harmonics = 50;
    int morph = 50;
    int timbre = 50;

    int timbreMod=50;
    int fmMod=50;
    int morphMod=50;

    PlaitsEngine plaitsEngine;

    simpleVcoScreen();
    void initWaveforms();
    void IRAM_ATTR update() override;
    void IRAM_ATTR refresh() override;
};
