// #pragma once
// #include <math.h>
// #include <stddef.h>
// #include "utils.h"
// #include "engines/tables.h"

// class WaveEngine
// {
// public:
//     virtual void Render(float frequency, float amplitude, float *out, size_t size) = 0;
//     virtual ~WaveEngine() {}
// };

// class SineEngine : public WaveEngine
// {
//     float phase = 0.0f;

// public:
//     void Render(float frequency, float amplitude, float *out, size_t size) override
//     {
//         ParameterInterpolator freqInterp(frequency, frequency, size);
//         ParameterInterpolator ampInterp(amplitude, amplitude, size);
//         for (size_t i = 0; i < size; ++i)
//         {
//             float freq = freqInterp.Next();
//             float amp = ampInterp.Next();
//             phase += freq * (1.0f / 48000.0f); // assuming 48kHz sample rate
//             if (phase >= 1.0f)
//                 phase -= 1.0f;
//             out[i] = amp * sinf(2.0f * M_PI * phase);
//         }
//     }
// };

// class SquareEngine : public WaveEngine
// {
//     float phase = 0.0f;

// public:
//     void Render(float frequency, float amplitude, float *out, size_t size) override
//     {
//         ParameterInterpolator freqInterp(frequency, frequency, size);
//         ParameterInterpolator ampInterp(amplitude, amplitude, size);
//         for (size_t i = 0; i < size; ++i)
//         {
//             float freq = freqInterp.Next();
//             float amp = ampInterp.Next();
//             phase += freq * (1.0f / 48000.0f);
//             if (phase >= 1.0f)
//                 phase -= 1.0f;
//             out[i] = amp * (phase < 0.5f ? 1.0f : -1.0f);
//         }
//     }
// };

// class TriangleEngine : public WaveEngine
// {
//     float phase = 0.0f;

// public:
//     void Render(float frequency, float amplitude, float *out, size_t size) override
//     {
//         ParameterInterpolator freqInterp(frequency, frequency, size);
//         ParameterInterpolator ampInterp(amplitude, amplitude, size);
//         for (size_t i = 0; i < size; ++i)
//         {
//             float freq = freqInterp.Next();
//             float amp = ampInterp.Next();
//             phase += freq * (1.0f / 48000.0f);
//             if (phase >= 1.0f)
//                 phase -= 1.0f;
//             float t = phase < 0.5f ? phase : 1.0f - phase;
//             out[i] = amp * (4.0f * t - 1.0f);
//         }
//     }
// };

// class TableEngine : public WaveEngine
// {
// public:
//     float phase = 0.0f;
//     int wave = 0; // Index of the current waveform in the wavetable

// public:
//     void Render(float frequency, float amplitude, float *out, size_t size) override
//     {
//         ParameterInterpolator freqInterp(frequency, frequency, size);
//         ParameterInterpolator ampInterp(amplitude, amplitude, size);
//         for (size_t i = 0; i < size; ++i)
//         {
//             float freq = freqInterp.Next();
//             float amp = ampInterp.Next();
//             phase += freq * (1.0f / 48000.0f);
//             if (phase >= 1.0f)
//                 phase -= 1.0f;
//             unsigned int index = phase * 128;            // Assuming 132 samples in the wavetable
//             float t = wavetable[wave][index] / 32768.0f; // Normalize to -1.0 to 1.0
//             out[i] = amp * (t);
//         }
//     }
// };
