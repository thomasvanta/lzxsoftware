#include "WavePlusLUT.h"

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
    // Bank 4 - Factory Banks 4/5/6 - Stepped Ramps, with extra tiling parameter
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
    // Bank 5 - Factory Bank 7 - Waveform Visualization 1 - No Ramp
    new WavePlusLUT(
        {.altA_default = 0.0f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::Constant},
        nullptr
    ),
    // Bank 6 - Factory Bank 8 - Waveform Visualization 2 - No Ramp
    new WavePlusLUT(
        {.altA_default = 0.0f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 1,
         .update_style = WavePlusLUT::UpdateStyle::Constant},
        nullptr
    ),
    // Bank 7 - Checkerboard Tablecloth (Square wave oscillator)
    new WavePlusLUT(
        {.altA_default = 0.5f,
         .altB_default = 0.5f,
         .scrollrange_default = ScrollRange::Tortoise,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            uint16_t wavelength = uint16_t(vres * (31.0f * state.param_altA + 1) / 32.0f);
            uint16_t pulsewidth = uint16_t(wavelength * state.param_altB) + 1;
            bool on = l.i % wavelength < pulsewidth;
            return (on != state.invert) * DAC_MAX_VALUE;
        }
    ),
    // Bank 8 - Dynamic Perlin Noise
    new WavePlusLUT(
        {.altA_default = 0.1f,
         .altB_default = 0.1f,
         .scrollrange_default = ScrollRange::Tortoise,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float pos = float(l.i) / float(l.vres);
            float freq = (64 * state.param_altA + 0.1) / MAX_SLIDER_VALUE;
            int depth = int(12 * state.param_altB / MAX_SLIDER_VALUE + 1);
            float n = perlin1d(pos, freq, depth, 4.0);
            float val = state.invert ? std::abs(1 - n) : n;
            return val * DAC_MAX_VALUE;
        }
    )};
