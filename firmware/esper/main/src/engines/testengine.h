#include "synths/test/sine.h"

class testEngine
{
public:
    SineOscillator sine_oscillator1;
    SineOscillator sine_oscillator2;

    float frequency_; // Frequency of the sine wave
    float amplitude_; // Amplitude of the sine wave
    testEngine()
    {
    }

    void Render(float frequency, float amplitude, float *out, float *aux, size_t size)
    {
        sine_oscillator1.Render(frequency, amplitude, out, size);
        sine_oscillator2.Render(frequency, amplitude, aux, size);
    }
};