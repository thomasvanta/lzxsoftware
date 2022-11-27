#include "DiverMode.h"
#include "WavePlusLUT.h"

DiverMode* banks[] = {
    // Factory Bank 1 - Linear Ramp
    new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::Constant},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            return ((l.i * DAC_MAX_VALUE) / l.vres) & 1023;
        }
    ),
    // Factory Bank 2 - Logarithmic Ramp
    new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::Constant},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float a = float(l.i) / float(l.vres);
            float b = (a * a) * 1023.0;
            float c = (a * 1023.0 * 2.0) - (b);
            return uint16_t(c) & 1023;
        }
    ),
    // Factory Bank 3 - Exponential Ramp
    new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::Constant},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float a = float(l.i) / float(l.vres);
            float b = (a * a) * 1023.0;
            return uint16_t(b) & 1023;
        }
    ),
    // Bank 4 - Factory Banks 4/5/6 - Stepped Ramps, with extra tiling parameter
    new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            uint8_t num_bits = uint8_t(8.0 * state.altA_slider / MAX_SLIDER_VALUE) + 1;
            uint8_t bit_offset = uint8_t(8.0 * state.altB_slider / MAX_SLIDER_VALUE);
            uint16_t full_mask = 0b1111111111;
            uint16_t mask = full_mask >> bit_offset & ~(full_mask >> (bit_offset + num_bits));
            return (((l.i * DAC_MAX_VALUE) / l.vres) & mask) << bit_offset;
        }
    ),
    // Bank 5 - Factory Bank 7 - Waveform Visualization 1 - No Ramp
    new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::Constant},
        nullptr
    ),
    // Bank 6 - Factory Bank 8 - Waveform Visualization 2 - No Ramp
    new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 1,
            .updateStyle = WavePlusLUT::UpdateStyle::Constant},
        nullptr
    ),
    // Bank 7 - Checkerboard Tablecloth (Square wave oscillator)
    new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            uint16_t wavelength = uint16_t(hres * (31.0 * state.altA_slider / MAX_SLIDER_VALUE + 1) / 32.0);
            uint16_t pulsewidth = uint16_t(wavelength * 2.0 * state.altB_slider / MAX_SLIDER_VALUE) + 1;
            bool on = l.i % wavelength < pulsewidth;
            return (on != state.invert) * DAC_MAX_VALUE;
        }
    )};
