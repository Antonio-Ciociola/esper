#include "defines.h"
#include "math.h"

class SineOscillator
{
public:
    float phase_;     // Current phase of the oscillator
    float frequency_; // Frequency of the sine wave
    float amplitude_; // Amplitude of the sine wave
    SineOscillator()
    {
        phase_ = 0.0f;
        frequency_ = 0.0f;
        amplitude_ = 0.0f;
    }

    inline float Next(float frequency)
    {
        phase_ += frequency * SAMPSTEP;
        if (phase_ >= 1.0f)
            phase_ -= 1.0f; // Wrap around the phase

        // return amplitude_ * sinf(2.0f * M_PI * phase_);
        return amplitude_ * sinf(2.0f * M_PI * phase_);
    }

    void Render(float frequency, float amplitude, float *out, size_t size)
    {
        RenderInternal(frequency, amplitude, out, size);
    }

    void RenderInternal(float frequency, float amplitude, float *out, size_t size)
    {
        frequency_ = frequency;
        amplitude_ = amplitude;

        for (size_t i = 0; i < size; ++i)
        {
            out[i] = Next(frequency_);
        }

    }
};