#ifndef __VANTABANKS_H__
#define __VANTABANKS_H__

#include "WavePlusLUT.h"
#include "perlin.h"
#include <cmath>

struct VanTaBanks
{
    DiverBankBase* PerlinStatic = new WavePlusLUT(
        {.altA_default = 0.0f,
         .altB_default = 0.0f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::Constant},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            return perlin1d(float(l.i) / float(l.vres), 333, 3) * DAC_MAX_VALUE;
        }
    );

    DiverBankBase* PerlinDynamic = new WavePlusLUT(
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
    );

    DiverBankBase* SineWave = new WavePlusLUT(
        {.altA_default = 0.1f,
         .altB_default = 0.1f,
         .scrollrange_default = ScrollRange::Tar,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float pos = float(l.i) / float(l.vres);
            float freq = l.vres * state.param_altA / MAX_SLIDER_VALUE;
            float offset = 64 * state.param_altB / MAX_SLIDER_VALUE;
            float sineWave = sin(pos * freq + offset);
            float val = state.invert ? std::abs(1.0 - sineWave) : sineWave;
            return val * DAC_MAX_VALUE;
        }
    );

    DiverBankBase* weirdWave = new WavePlusLUT(
        {.altA_default = 0.1f,
         .altB_default = 0.1f,
         .scrollrange_default = ScrollRange::Hare,
         .deinterlace_mode = 0,
         .update_style = WavePlusLUT::UpdateStyle::PerFrame},
        [](WavePlusLUT::Lookup& l, DiverUIState& state)
        {
            float pos = float(l.i) / float(l.vres);
            float freq = l.hres * state.param_altA / MAX_SLIDER_VALUE;
            float freq2 = l.hres * state.param_altB / MAX_SLIDER_VALUE;
            float sineWave = sin(pos * freq);
            float val = state.invert ? std::abs(sineWave) : sineWave;
            float cosWave = cos(pos * freq2);
            return (val / cosWave) * DAC_MAX_VALUE;
        }
    );
};

VanTaBanks banksVanTa;

#endif