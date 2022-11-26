#ifndef __DIVERUISTATE_H__
#define __DIVERUISTATE_H__

#include <cstdint>

struct DiverUIState
{
    DiverUIState(const uint8_t num_banks) : num_banks(num_banks) {};

    uint8_t selected_bank;
    const uint8_t num_banks;

    uint8_t captureEnable;

    uint8_t mirrorx;
    uint8_t mirrory;
    uint8_t scrollx;
    uint8_t scrolly;
    uint8_t invert;

    uint16_t hphase_slider;
    uint16_t vphase_slider;
    uint16_t vphase_cv;
    uint16_t hphasecnt;
    uint16_t vphasecnt;
};

#endif