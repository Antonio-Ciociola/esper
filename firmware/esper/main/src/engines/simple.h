#pragma once
#include <math.h>
#include <stddef.h>
#include "utils.h"
#include "engines/tables.h"
#include "plaits/dsp/oscillator/oscillator.h"
#include "plaits/dsp/oscillator/sine_oscillator.h"

class SimpleEngine
{
public:
    plaits::Oscillator oscillator;
    plaits::SineOscillator sineOscillator;
    enum class WaveType
    {
        Sine,
        Triangle,
        Square,
        Sawtooth

    };

    SimpleEngine()
    {
        oscillator.Init();
        sineOscillator.Init();
    }

    void Render(float frequency, float amplitude, WaveType type, float duty, float *out, size_t size)
    {
        frequency /= 48000.0f; // Normalize frequency to 48kHz sample rate
        switch (type)
        {
        case WaveType::Sine:
            sineOscillator.Render(frequency, out, size);
            break;
        case WaveType::Triangle:
            oscillator.Render<plaits::OSCILLATOR_SHAPE_TRIANGLE>(frequency, 0.5f, out, size);
            break;
        case WaveType::Square:
            oscillator.Render<plaits::OSCILLATOR_SHAPE_SQUARE>(frequency, 1.0f - duty, out, size);
            break;
        case WaveType::Sawtooth:
            oscillator.Render<plaits::OSCILLATOR_SHAPE_SAW>(frequency, 0.5f, out, size);
            break;
        }
        for (size_t i = 0; i < size; ++i)
        {
            out[i] *= amplitude; // Apply amplitude scaling
        }
    }
};