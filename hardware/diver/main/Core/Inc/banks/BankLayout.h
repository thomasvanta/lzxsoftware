#include "WavePlusLUT.h"

#include "math_utils.h"
#include "perlin.h"

#include <cmath>

DiverBankBase* banks[] = {
    // Factory Bank 1 - Linear Ramp
    new WavePlusLUT(
        {.altA_default = 0.0f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::Constant},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            return ((l.i * DAC_MAX_VALUE) / l.vres) & 1023;
        }
    ),
    // Factory Bank 2 - Logarithmic Ramp
    new WavePlusLUT(
        {.altA_default = 0.0f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::Constant},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float a = float(l.i) / float(l.vres);
            float b = (a * a) * 1023.0f;
            float c = (a * 1023.0f * 2.0f) - (b);
            return uint16_t(c) & 1023;
        }
    ),
    // Factory Bank 3 - Exponential Ramp
    new WavePlusLUT(
        {.altA_default = 0.0f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::Constant},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float a = float(l.i) / float(l.vres);
            float b = (a * a) * 1023.0f;
            return uint16_t(b) & 1023;
        }
    ),
    // Unit tension curve - continuously variable log-linear-exp ramp [@dewb]
    new WavePlusLUT(
        {.altA_default = 0.5f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            // k = -4, 4. 0 is NaN, but limit approaching 0 is linear
            float k = 8.0 * (state.param_altA - 0.5f);
            if (fabs(k) < 0.00001)
            {
                return ((l.i * DAC_MAX_VALUE) / l.vres) & 1023;
            }

            // use sigmoid to control hard/soft edges
            // float edge = state.param_altB;

            float a = float(l.i) / float(l.vres);
            float b = (exp_approx(k * a) - 1.0f) / (exp_approx(k) - 1.0f);
            // float c = logistic_curve(constrain(b, 0.0f, 1.0f), edge, 0.5f);
            return uint16_t(b * DAC_MAX_VALUE) & 1023;
        }
    ),
    // Factory Banks 4/5/6 combined - stepped linear ramps, with extra tiling parameter
    new WavePlusLUT(
        {.altA_default = 0.5f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            uint8_t num_bits = uint8_t(7.0f * (1.0f - state.param_altA)) + 1;
            uint8_t bit_offset = uint8_t(8.0f * state.param_altB);
            uint16_t full_mask = 0b1111111111;
            uint16_t mask = full_mask >> bit_offset & ~(full_mask >> (bit_offset + num_bits));
            return (((l.i * DAC_MAX_VALUE) / l.vres) & mask) << bit_offset;
        }
    ),
    // Square wave oscillator (aka Checkered Tablecloth) [@dewb]
    new WavePlusLUT(
        {.altA_default = 0.5f,
         .altB_default = 0.5f,
         .scrollrange_default = ScrollRange::Tortoise,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            uint16_t wavelength = uint16_t(l.vres * (31.0f * state.param_altA + 1) / 32.0f);
            uint16_t pulsewidth = uint16_t(wavelength * state.param_altB) + 1;
            bool on = l.i % wavelength < pulsewidth;
            return (on != state.invert) * DAC_MAX_VALUE;
        }
    ),
    // Dynamic Perlin Noise [@VanTa]
    new WavePlusLUT(
        {.altA_default = 0.1f,
         .altB_default = 0.1f,
         .scrollrange_default = ScrollRange::Tortoise,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float pos = float(l.i) / float(l.vres);
            float freq = (64 * state.param_altA + 0.1f);
            int depth = int(12 * state.param_altB + 1);
            float n = perlin1d(pos, freq, depth, 4.0f);
            float val = state.invert ? std::abs(1 - n) : n;
            return val * DAC_MAX_VALUE;
        }
    ),
    // Sine wave oscillator [@VanTa]
    new WavePlusLUT(
        {.altA_default = 0.1f,
         .altB_default = 0.1f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float pos = float(l.i) / float(l.vres);
            float freq = l.vres * state.param_altA;
            float offset = 32 * state.param_altB;
            float sineWave = sin(pos * freq + offset);
            float val = state.invert ? std::abs(1.0f - sineWave) : sineWave;
            return val * DAC_MAX_VALUE;
        }
    ),
    // Factory Bank 7 - Waveform Visualization 1 - No Ramp
    new WavePlusLUT(
        {.altA_default = 0.0f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::Constant},
        nullptr
    ),
    // Factory Bank 8 - Waveform Visualization 2 - No Ramp
    new WavePlusLUT(
        {.altA_default = 0.0f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 1,
         .update_style = WavePlusLUT::UpdateStyle::Constant},
        nullptr
    )};
