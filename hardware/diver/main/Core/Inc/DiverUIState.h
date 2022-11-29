#ifndef __DIVERUISTATE_H__
#define __DIVERUISTATE_H__

#include <cstdint>

// We probably don't need this many ranges; can experiment,
// get feedback and condense later.
enum class ScrollRange
{
    Strobe = 0,
    Cheetah,
    Hare,
    Tortoise,
    Snail,
    Tar,
    NUM_SCROLL_RANGES
};

struct DiverUIState
{
    DiverUIState(const uint8_t num_banks)
        : num_banks(num_banks){};

    uint8_t selected_bank;
    const uint8_t num_banks;

    uint8_t captureEnable;

    uint8_t mirrorx;
    uint8_t mirrory;
    uint8_t scrollx;
    uint8_t scrolly;
    uint8_t invert;

    ScrollRange scrollx_range;
    ScrollRange scrolly_range;

    uint16_t hphase_slider;
    uint16_t vphase_slider;
    uint16_t vphase_cv;
    uint16_t hphasecnt;
    uint16_t vphasecnt;

    float param_hphase;
    float param_vphase;
    float param_altA;
    float param_altB;
};

#endif
