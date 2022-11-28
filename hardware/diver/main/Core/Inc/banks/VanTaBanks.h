#ifndef __VANTABANKS_H__
#define __VANTABANKS_H__

#include "WavePlusLUT.h"
#include "perlin.h"
#include <cmath>

struct VanTaBanks
{
    DiverBankBase* PerlinStatic = new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::Constant},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            return perlin1d(float(l.i) / float(l.vres), 333, 3) * DAC_MAX_VALUE;
        }
    );

    DiverBankBase* PerlinDynamic = new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float pos = float(l.i) / float(l.vres);
            float freq = (64 * state.altA_slider + 0.1) / MAX_SLIDER_VALUE;
            int depth = int(12 * state.altB_slider / MAX_SLIDER_VALUE + 1);
            float n = perlin1d(pos, freq, depth, 4.0);
            float val = state.invert ? std::abs(1 - n) : n;
            return val * DAC_MAX_VALUE;
        }
    );

    DiverBankBase* SineWave = new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float pos = float(l.i) / float(l.vres);
            float freq = l.vres * state.altA_slider / MAX_SLIDER_VALUE;
            float offset = 64 * state.altB_slider / MAX_SLIDER_VALUE;
            float sineWave = sin(pos * freq + offset);
            float val = state.invert ? std::abs(1.0 - sineWave) : sineWave;
            return val * DAC_MAX_VALUE;
        }
    );

    DiverBankBase* weirdWave = new WavePlusLUT(
        WavePlusLUT::Options{
            .deinterlace_mode = 0,
            .updateStyle = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float pos = float(l.i) / float(l.vres);
            float freq = l.hres * state.altA_slider / MAX_SLIDER_VALUE;
            float freq2 = l.hres * state.altB_slider / MAX_SLIDER_VALUE;
            float sineWave = sin(pos * freq);
            float val = state.invert ? std::abs(sineWave) : sineWave;
            float cosWave = cos(pos * freq2);
            return (val / cosWave) * DAC_MAX_VALUE;
        }
    );
};

VanTaBanks banksVanTa;

#endif