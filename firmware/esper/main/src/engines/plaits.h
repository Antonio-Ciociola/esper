#pragma once
#include <math.h>
#include <stddef.h>
#include "utils.h"
#include "engines/tables.h"
#include "plaits/dsp/voice.h"

class PlaitsEngine
{
public:
    plaits::Modulations modulations;
    plaits::Patch patch;
    plaits::Voice voice;
    const char *engines =
        "0 Pr classic wfm\n"
        "1 Waveshp Osc\n"
        "2 Two-Op FM\n"
        "3 Grn Frmnt Osc\n"
        "4 Harmonic Osc\n"
        "5 Wavetable Osc\n"
        "6 Chords\n"
        "7 Vowel Speech\n"
        "8 Granular Cloud\n"
        "9 Filtered Noise\n"
        "10 Harmo Noise\n"
        "11 Inharmo Str\n"
        "12 Modal Resntr\n"
        "13 An Bass Drum\n"
        "14 An Snare Drum\n"
        "15 An Hi-hat\n"
        "16 Wave filtered\n"
        "17 Phase distort\n"
        "18 6-op\n"
        "19 6-op\n"
        "20 6-op\n"
        "21 Wave terrain\n"
        "22 String mach\n"
        "23 Chiptune";

    PlaitsEngine()
    {
        char *shared_buffer = (char *)malloc(16384);
        stmlib::BufferAllocator allocator(shared_buffer, 16384);
        voice.Init(&allocator);

        patch.note = 60.0f; // Middle C
        patch.harmonics = 0.5f;
        patch.timbre = 0.5f;
        patch.morph = 0.5f;
        patch.frequency_modulation_amount = 0.0f;
        patch.timbre_modulation_amount = 0.0f;
        patch.morph_modulation_amount = 0.0f;

        patch.engine = 9;        // Default engine
        patch.decay = 0.5f;      // Default decay
        patch.lpg_colour = 0.5f; // Default LPG colour

        modulations.engine = 0.0f;
        modulations.note = 0.0f;
        modulations.frequency = 0.0f;
        modulations.harmonics = 0.0f;
        modulations.timbre = 0.0f;
        modulations.morph = 0.0f;
        modulations.trigger = 0.0f;
        modulations.level = 0.0f;

        modulations.frequency_patched = false;
        modulations.timbre_patched = false;
        modulations.morph_patched = false;
        modulations.trigger_patched = false;
        modulations.level_patched = false;
    }

    void IRAM_ATTR setPatch(float frequency, float harmonics, float timbre, float morph, int engine,float timbreMod , float fmMod , float morphMod )
    {
        patch.note = 12.0f * log2f((float)frequency / 440.0f) + 69.0f;
        patch.harmonics = harmonics;
        patch.timbre = timbre;
        patch.morph = morph;

        engine = engine < 16 ? engine + 8 : engine - 16; // Adjust engine index if needed
        if (engine < 0 || engine >= plaits::kMaxEngines)
            engine = 0;
        patch.engine = engine;

        patch.timbre_modulation_amount = timbreMod;
        patch.frequency_modulation_amount = fmMod;
        patch.morph_modulation_amount = morphMod;
    }

    void IRAM_ATTR setModulations(float frequency, float harmonics, float timbre, float morph, float trigger, float level)
    {
        modulations.frequency = frequency;
        modulations.harmonics = harmonics;
        modulations.timbre = timbre;
        modulations.morph = morph;
        modulations.trigger = trigger;
        modulations.level = level;

        modulations.frequency_patched = true;
        modulations.timbre_patched = true;
        modulations.morph_patched = true;
        modulations.trigger_patched = true;
        modulations.level_patched = true;
    }


    void IRAM_ATTR Render(float amplitude, float *out, float *aux, size_t size)
    {



        static plaits::Voice::Frame output[BLOCK_SIZE];

        voice.Render(patch, modulations, output, size);


        for (size_t i = 0; i < size; ++i)
        {
            out[i] = (float)output[i].out * amplitude; // Apply amplitude scaling
            aux[i] = (float)output[i].aux * amplitude; // Apply amplitude scaling
        }
    }
};