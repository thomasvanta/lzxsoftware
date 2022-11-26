#ifndef __DIVERUI_H__
#define __DIVERUI_H__

#include <cstdint>
#include "DiverUIState.h"

class DiverUI
{
public:
    DiverUIState& state;

    DiverUI(DiverUIState& state) : state(state) {};

    void Pots_Poll();
    void Buttons_Poll();

    void Display_Init();
    void Display_Refresh();

    void OnInterruptHSync();

    uint8_t frozen;

    uint8_t bank_display_mode;
    uint8_t bank_display_counter;
    
    uint8_t trigger_rising;
    uint8_t trigger_falling;
    uint8_t trigger_state;

    uint8_t trigger_display_mode;
    uint8_t trigger_enable_freeze;
    uint8_t trigger_enable_clear;
    uint8_t trigger_enable_mirrorx;
    uint8_t trigger_enable_mirrory;
    uint8_t trigger_enable_scrollx;
    uint8_t trigger_enable_scrolly;
    uint8_t trigger_enable_invert;

private:
    void TVP5150AM1_Setup();
    void I2C_WriteRegister(uint32_t address, uint8_t byte1, uint8_t byte2);

    uint16_t ledupdatecnt;
    uint8_t reg[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
};

#endif