#include "DiverUI.h"
#include "DiverUITables.h"

#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "gpio.h"

#include "globals.h"

#include <math.h>
#include <string>
#include <stdio.h>

extern "C" I2C_HandleTypeDef hi2c1;

enum
{
    kSlider1 = 0,
    kSlider2,
    kCV1,
    kCV2,
    kCV3,
    NUM_POTENTIOMETERS
};

struct PotentiometerConfig
{
    std::string name;
    uint16_t value;
};

PotentiometerConfig pots[NUM_POTENTIOMETERS] = {
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0}};
enum
{
    kButtonScrollY = 0,
    kButtonScrollX,
    kButtonInvert,
    kButtonMirrorX,
    kButtonMirrorY,
    kButtonFreeze,
    kButtonClear,
    kButtonBankSelect,
    kButtonRestore,
    kButtonMap,
    kTriggerIn,
    NUM_BUTTONS
};

enum
{
    kLED_Bargraph1 = 22,
    kLED_Bargraph2 = 21,
    kLED_Bargraph3 = 20,
    kLED_Bargraph4 = 19,
    kLED_Bargraph5 = 18,
    kLED_Bargraph6 = 16,
    kLED_Bargraph7 = 15,
    kLED_Bargraph8 = 14,
    kLED_Bargraph9 = 13,
    kLED_Bargraph10 = 12,
    kLED_Bargraph11 = 10,
    kLED_Bargraph12 = 9,
    kLED_Bargraph13 = 8,
    kLED_Bargraph14 = 7,
    kLED_Bargraph15 = 6,
    kLED_Bargraph16 = 4,
    kLED_Bargraph17 = 3,
    kLED_Bargraph18 = 2,
    kLED_Bargraph19 = 1,
    kLED_Bargraph20 = 0,
    kLED_MirrorX = 24,
    kLED_MirrorY = 25,
    kLED_Invert = 26,
    kLED_ScrollX = 27,
    kLED_ScrollY = 28,
    kLED_Freeze = 30,
    kLED_Clear = 31
};

struct ButtonConfig
{
    std::string name;
    uint8_t value;
    uint8_t last;
    uint8_t rising;
    uint8_t falling;
    uint8_t togglestate;
};

ButtonConfig buttons[NUM_BUTTONS] = {
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0},
    {"", 0}};

struct LEDConfig
{
    uint8_t brightness;
    uint8_t lastBrightness;
};

LEDConfig leds[48];

static const uint8_t display_address[1] = {IS32FL3738_ADDRESS_A};

void DiverUI::Buttons_Poll(void)
{
    // Store previous values
    for (uint32_t i = 0; i < NUM_BUTTONS; i++)
    {
        buttons[i].last = buttons[i].value;
        buttons[i].rising = 0;
        buttons[i].falling = 0;
    }

    // Read new values
    if (HAL_GPIO_ReadPin(SW0_GPIO_Port, SW0_Pin))
    {
        buttons[kButtonScrollY].value = 0;
    }
    else
    {
        buttons[kButtonScrollY].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin))
    {
        buttons[kButtonScrollX].value = 0;
    }
    else
    {
        buttons[kButtonScrollX].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin))
    {
        buttons[kButtonInvert].value = 0;
    }
    else
    {
        buttons[kButtonInvert].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin))
    {
        buttons[kButtonMirrorX].value = 0;
    }
    else
    {
        buttons[kButtonMirrorX].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW7_GPIO_Port, SW7_Pin))
    {
        buttons[kButtonMirrorY].value = 0;
    }
    else
    {
        buttons[kButtonMirrorY].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW8_GPIO_Port, SW8_Pin))
    {
        buttons[kButtonFreeze].value = 0;
    }
    else
    {
        buttons[kButtonFreeze].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW9_GPIO_Port, SW9_Pin))
    {
        buttons[kButtonClear].value = 0;
    }
    else
    {
        buttons[kButtonClear].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW10_GPIO_Port, SW10_Pin))
    {
        buttons[kButtonMap].value = 0;
    }
    else
    {
        buttons[kButtonMap].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW11_GPIO_Port, SW11_Pin))
    {
        buttons[kButtonRestore].value = 0;
    }
    else
    {
        buttons[kButtonRestore].value = 1;
    }
    if (HAL_GPIO_ReadPin(SW12_GPIO_Port, SW12_Pin))
    {
        buttons[kButtonBankSelect].value = 0;
    }
    else
    {
        buttons[kButtonBankSelect].value = 1;
    }
    // if (HAL_GPIO_ReadPin(TRIG_IN_GPIO_Port, TRIG_IN_Pin))  { buttons[kTriggerIn].value = 1; }
    // else {	buttons[kTriggerIn].value = 0; }

    // Rising and falling edge flag detection
    for (uint32_t i = 0; i < NUM_BUTTONS; i++)
    {
        if (!buttons[i].last && buttons[i].value)
        {
            buttons[i].rising = 1;
        }
        else if (buttons[i].last && !buttons[i].value)
        {
            buttons[i].falling = 1;
        }
    }

    buttons[kTriggerIn].rising = 0;
    buttons[kTriggerIn].falling = 0;
    buttons[kTriggerIn].value = trigger_state;

    if (trigger_rising > 0)
    {
        buttons[kTriggerIn].rising = trigger_rising;
    }
    else if (trigger_falling > 0)
    {
        buttons[kTriggerIn].falling = trigger_falling;
    }

    trigger_falling = 0;
    trigger_rising = 0;

    if (buttons[kButtonRestore].rising)
    {
        for (uint32_t i = 0; i < 11; i++)
        {
            buttons[i].togglestate = 0;
            buttons[i].rising = 0;
        }
        frozen = 0;
        trigger_display_mode = 0;
        bank_display_mode = 0;
        bank_display_counter = 0;
        trigger_display_mode = 0;
        trigger_enable_freeze = 0;
        trigger_enable_clear = 0;
        trigger_enable_mirrorx = 0;
        trigger_enable_mirrory = 0;
        trigger_enable_scrollx = 0;
        trigger_enable_scrolly = 0;
        trigger_enable_invert = 0;

        state.selected_bank = 0;
        on_bank_changed();
    }

    // Handle Clear+Slider alt params
    if (buttons[kButtonClear].rising)
    {
        // Enable alt parameters on sliders
        hSlider.Lock();
        vSlider.Lock();
    }
    else if (buttons[kButtonClear].falling)
    {
        // Release sliders into catchup mode
        hSlider.Unlock();
        vSlider.Unlock();
    }

    // Handle Clear+Scroll range selection
    if (buttons[kButtonClear].value)
    {
        if (buttons[kButtonScrollX].rising)
        {
            state.scrollx_range = (ScrollRange)(((int)state.scrollx_range + 1) % (int)ScrollRange::NUM_SCROLL_RANGES);
            buttons[kButtonScrollX].rising = 0;
        }
        if (buttons[kButtonScrollY].rising)
        {
            state.scrolly_range = (ScrollRange)(((int)state.scrolly_range + 1) % (int)ScrollRange::NUM_SCROLL_RANGES);
            buttons[kButtonScrollY].rising = 0;
        }
    }

    trigger_display_mode = buttons[kButtonMap].value;
    if (trigger_display_mode)
    {
        if (buttons[kButtonFreeze].rising)
        {
            trigger_enable_freeze = !trigger_enable_freeze;
            // buttons[kButtonFreeze].rising = 0;
            // buttons[kButtonFreeze].togglestate = !buttons[kButtonFreeze].togglestate;
            if (trigger_enable_freeze && trigger_enable_clear)
            {
                trigger_enable_clear = 0;
            }
        }
        if (buttons[kButtonClear].rising)
        {
            trigger_enable_clear = !trigger_enable_clear;
            // buttons[kButtonClear].rising = 0;
            // buttons[kButtonClear].togglestate = !buttons[kButtonClear].togglestate;
            if (trigger_enable_freeze && trigger_enable_clear)
            {
                trigger_enable_freeze = 0;
            }
        }
        if (buttons[kButtonMirrorX].rising)
        {
            trigger_enable_mirrorx = !trigger_enable_mirrorx;
            // buttons[kButtonMirrorX].rising = 0;
            // buttons[kButtonMirrorX].togglestate = !buttons[kButtonMirrorX].togglestate;
        }
        if (buttons[kButtonMirrorY].rising)
        {
            trigger_enable_mirrory = !trigger_enable_mirrory;
            // buttons[kButtonMirrorY].rising = 0;
            // buttons[kButtonMirrorY].togglestate = !buttons[kButtonMirrorY].togglestate;
        }
        if (buttons[kButtonInvert].rising)
        {
            trigger_enable_invert = !trigger_enable_invert;
            // buttons[kButtonInvert].rising = 0;
            // buttons[kButtonInvert].togglestate = !buttons[kButtonInvert].togglestate;
        }
        if (buttons[kButtonScrollX].rising)
        {
            trigger_enable_scrollx = !trigger_enable_scrollx;
            // buttons[kButtonScrollX].rising = 0;
            // buttons[kButtonScrollX].togglestate = !buttons[kButtonScrollX].togglestate;
        }
        if (buttons[kButtonScrollY].rising)
        {
            trigger_enable_scrolly = !trigger_enable_scrolly;
            // buttons[kButtonScrollY].rising = 0;
            // buttons[kButtonScrollY].togglestate = !buttons[kButtonScrollY].togglestate;
        }
    }
    else
    {
        for (uint32_t i = 0; i < NUM_BUTTONS; i++)
        {
            if (buttons[i].rising == 1)
            {
                buttons[i].togglestate = !buttons[i].togglestate;
            }
        }
    }

    if (trigger_enable_freeze)
    {
        buttons[kButtonFreeze].rising = buttons[kTriggerIn].rising | buttons[kButtonFreeze].rising;
    }

    if (trigger_enable_clear)
    {
        buttons[kButtonClear].rising = buttons[kTriggerIn].rising | buttons[kButtonClear].rising;
    }

    if (trigger_enable_mirrorx)
    {
        state.mirrorx = buttons[kButtonMirrorX].togglestate ^ buttons[kTriggerIn].value;
    }
    else
    {
        state.mirrorx = buttons[kButtonMirrorX].togglestate;
    }

    if (trigger_enable_mirrory)
    {
        state.mirrory = buttons[kButtonMirrorY].togglestate ^ buttons[kTriggerIn].value;
    }
    else
    {
        state.mirrory = buttons[kButtonMirrorY].togglestate;
    }

    if (trigger_enable_invert)
    {
        state.invert = buttons[kButtonInvert].togglestate ^ buttons[kTriggerIn].value;
    }
    else
    {
        state.invert = buttons[kButtonInvert].togglestate;
    }

    if (trigger_enable_scrollx)
    {
        state.scrollx = buttons[kButtonScrollX].togglestate ^ buttons[kTriggerIn].value;
    }
    else
    {
        state.scrollx = buttons[kButtonScrollX].togglestate;
    }

    if (trigger_enable_scrolly)
    {
        state.scrolly = buttons[kButtonScrollY].togglestate ^ buttons[kTriggerIn].value;
    }
    else
    {
        state.scrolly = buttons[kButtonScrollY].togglestate;
    }

    if (buttons[kButtonBankSelect].rising)
    {
        if (bank_display_mode)
        {
            state.selected_bank = (state.selected_bank + 1) % state.num_banks;
            on_bank_changed();
        }
        bank_display_counter = 0;
    }

    if (bank_display_counter < 91)
    {
        bank_display_counter++;
    }
    else
    {
        bank_display_counter = 91;
    }
    if (bank_display_counter > 90)
    {
        bank_display_mode = 0;
    }
    else
    {
        bank_display_mode = 1;
    }

    state.captureEnable = 0;
    if (buttons[kButtonFreeze].rising)
    {

        if (frozen == 1)
        {
            state.captureEnable = 1;
        }
        frozen = 1;
    }
    else if (buttons[kButtonClear].rising)
    {
        frozen = 0;
        state.captureEnable = 1;
    }
    else if (!frozen)
    {
        state.captureEnable = 1;
    }

    if (state.scrollx)
    {
        //

        uint16_t speed;
        if (state.hphase_slider >= (hres >> 1))
        {
            speed = state.hphase_slider - (hres >> 1);
            uint16_t displacement = uint16_t(speed >> (int)state.scrollx_range);
            state.hphasecnt = (state.hphasecnt + displacement) % hres;
        }
        else
        {
            speed = (hres >> 1) - state.hphase_slider;
            uint16_t displacement = uint16_t(speed >> (int)state.scrollx_range);
            state.hphasecnt = (hres + state.hphasecnt - displacement) % hres;
        }
        // hphase_slider = hphasecnt;
    }

    if (state.scrolly)
    {
        uint16_t speed;
        if (state.vphase_slider >= (vres >> 1))
        {
            speed = state.vphase_slider - (vres >> 1);
            uint16_t displacement = uint16_t(speed >> (int)state.scrolly_range);
            state.vphasecnt = (state.vphasecnt + displacement) % vres;
        }
        else
        {
            speed = (vres >> 1) - state.vphase_slider;
            uint16_t displacement = uint16_t(speed >> (int)state.scrolly_range);
            state.vphasecnt = (vres + state.vphasecnt - displacement) % vres;
        }
        // vphase_slider = vphasecnt;
    }
}

void DiverUI::Init()
{
    Pots_Init();
    Display_Init();
}

void DiverUI::Display_Init()
{
    TVP5150AM1_Setup();
    HAL_Delay(250);

    // HAL_GPIO_WritePin(GPIOC, LED_SDB_Pin, GPIO_PIN_RESET);
    // HAL_Delay(100);
    // HAL_GPIO_WritePin(GPIOC, LED_SDB_Pin, GPIO_PIN_SET);

    for (int i = 0; i < 1; i++)
    {
        I2C_WriteRegister(display_address[i], 0xFE, 0XC5); // Unlock
        I2C_WriteRegister(display_address[i], 0xFD, 0X03); // PG03 Configuration Registers
        if (i == 0)                                        // Disable Software Shutdown and set to master mode
        {
            I2C_WriteRegister(display_address[i], 0x00, 0b01000011);
        }
        else // Disable Software Shutdown and set to slave mode
        {
            I2C_WriteRegister(display_address[i], 0x00, 0b10000011);
        }

        I2C_WriteRegister(display_address[i], 0x01, 0x80); // Global Current Register Set to 42mA.
        I2C_WriteRegister(display_address[i], 0x0F, 0x07); // Enable De-ghosting 32k SWy Pullup
        I2C_WriteRegister(display_address[i], 0x10, 0x07); // Enable De-ghosting 32k CSx Pulldown
        I2C_WriteRegister(display_address[i], 0xFE, 0XC5); // Unlock
        I2C_WriteRegister(display_address[i], 0xFD, 0X00); // PG00 LED on/off
        for (int j = 0; j < 24; j++)
        {
            I2C_WriteRegister(display_address[i], j, 0x00); // Turn all LEDs on
        }

        for (int j = 0; j < 48; j++)
        {
            leds[j].brightness = 0;
            leds[j].lastBrightness = 0;
        }
    }

    // uint8_t curLED = 0;
    for (int i = 0; i < 1; i++)
    {
        I2C_WriteRegister(display_address[i], 0xFE, 0XC5); // Unlock
        I2C_WriteRegister(display_address[i], 0xFD, 0X01); // PG01 LED PWM
        for (int k = 0; k < 48; k++)
        {
            // curLED = (48 * i) + k;
            I2C_WriteRegister(display_address[i], display_pwm_startreg[k], 255);
            I2C_WriteRegister(display_address[i], display_pwm_startreg[k] + 1, 255);
            I2C_WriteRegister(display_address[i], display_pwm_startreg[k] + 16, 255);
            I2C_WriteRegister(display_address[i], display_pwm_startreg[k] + 17, 255);
        }
    }
}

void DiverUI::Pots_Init()
{
    hSlider.Init(&state.param_hphase, &state.param_altA, 1.0f, 0.0f);
    vSlider.Init(&state.param_vphase, &state.param_altB, 1.0f, 0.0f);
}

void DiverUI::Display_Refresh()
{
    if (ledupdatecnt == 0)
    {
        if (trigger_display_mode)
        {
            if (trigger_enable_mirrorx)
            {
                leds[kLED_MirrorX].brightness = 255;
            }
            else
            {
                leds[kLED_MirrorX].brightness = 0;
            }

            if (trigger_enable_mirrory)
            {
                leds[kLED_MirrorY].brightness = 255;
            }
            else
            {
                leds[kLED_MirrorY].brightness = 0;
            }

            if (trigger_enable_invert)
            {
                leds[kLED_Invert].brightness = 255;
            }
            else
            {
                leds[kLED_Invert].brightness = 0;
            }

            if (trigger_enable_scrollx)
            {
                leds[kLED_ScrollX].brightness = 255;
            }
            else
            {
                leds[kLED_ScrollX].brightness = 0;
            }

            if (trigger_enable_scrolly)
            {
                leds[kLED_ScrollY].brightness = 255;
            }
            else
            {
                leds[kLED_ScrollY].brightness = 0;
            }

            if (trigger_enable_freeze)
            {
                leds[kLED_Freeze].brightness = 255;
            }
            else
            {
                leds[kLED_Freeze].brightness = 0;
            }

            if (trigger_enable_clear)
            {
                leds[kLED_Clear].brightness = 255;
            }
            else
            {
                leds[kLED_Clear].brightness = 0;
            }
        }
        else
        {
            if (state.mirrorx)
            {
                leds[kLED_MirrorX].brightness = 255;
            }
            else
            {
                leds[kLED_MirrorX].brightness = 0;
            }
            if (state.mirrory)
            {
                leds[kLED_MirrorY].brightness = 255;
            }
            else
            {
                leds[kLED_MirrorY].brightness = 0;
            }
            if (state.invert)
            {
                leds[kLED_Invert].brightness = 255;
            }
            else
            {
                leds[kLED_Invert].brightness = 0;
            }

            if (state.scrollx)
            {
                leds[kLED_ScrollX].brightness = 255;
            }
            else
            {
                leds[kLED_ScrollX].brightness = 0;
            }
            if (state.scrolly)
            {
                leds[kLED_ScrollY].brightness = 255;
            }
            else
            {
                leds[kLED_ScrollY].brightness = 0;
            }
            if (state.captureEnable)
            {
                leds[kLED_Freeze].brightness = 0;
            }
            else
            {
                leds[kLED_Freeze].brightness = 255;
            }

            if (buttons[kButtonClear].rising)
            {
                leds[kLED_Clear].brightness = 255;
            }
            else
            {
                leds[kLED_Clear].brightness = 0;
            }
        }

        uint32_t j = 525 / 20; // number of samples to average
        uint32_t ledbargraph[20];

        for (uint32_t i = 0; i < 20; i++)
        {
            // i = led segment

            uint32_t k = j * i; // starting sample offset
            uint32_t accum = 0;
            for (uint32_t l = 0; l < j; l++)
            {
                accum += hwave[waveReadPtr][k + l];
            }

            // ledbargraph[i] = accum / j;
            if ((accum / j) >= (512))
            {
                ledbargraph[i] = 255;
            }
            else
            {
                ledbargraph[i] = 0;
            }
            if (bank_display_mode)
            {
                if (state.selected_bank == i)
                {
                    ledbargraph[i] = 255;
                }
                else
                {
                    ledbargraph[i] = 0;
                }
            }
        }

        leds[kLED_Bargraph1].brightness = ledbargraph[0];
        leds[kLED_Bargraph2].brightness = ledbargraph[1];
        leds[kLED_Bargraph3].brightness = ledbargraph[2];
        leds[kLED_Bargraph4].brightness = ledbargraph[3];
        leds[kLED_Bargraph5].brightness = ledbargraph[4];
        leds[kLED_Bargraph6].brightness = ledbargraph[5];
        leds[kLED_Bargraph7].brightness = ledbargraph[6];
        leds[kLED_Bargraph8].brightness = ledbargraph[7];
        leds[kLED_Bargraph9].brightness = ledbargraph[8];
        leds[kLED_Bargraph10].brightness = ledbargraph[9];
        leds[kLED_Bargraph11].brightness = ledbargraph[10];
        leds[kLED_Bargraph12].brightness = ledbargraph[11];
        leds[kLED_Bargraph13].brightness = ledbargraph[12];
        leds[kLED_Bargraph14].brightness = ledbargraph[13];
        leds[kLED_Bargraph15].brightness = ledbargraph[14];
        leds[kLED_Bargraph16].brightness = ledbargraph[15];
        leds[kLED_Bargraph17].brightness = ledbargraph[16];
        leds[kLED_Bargraph18].brightness = ledbargraph[17];
        leds[kLED_Bargraph19].brightness = ledbargraph[18];
        leds[kLED_Bargraph20].brightness = ledbargraph[19];

        uint8_t curLED = 0;
        for (int i = 0; i < 1; i++)
        {
            for (int b = 0; b < 24; b++)
            {
                reg[b] = 0;
            }
            for (int k = 0; k < 48; k++)
            {
                curLED = (48 * i) + k;

                if (leds[curLED].brightness > 0)
                {
                    switch (k)
                    {
                    case 0:
                        reg[0x00] = reg[0x00] | 0b00000011;
                        reg[0x02] = reg[0x02] | 0b00000011;
                        break;
                    case 1:
                        reg[0x04] = reg[0x04] | 0b00000011;
                        reg[0x06] = reg[0x06] | 0b00000011;
                        break;
                    case 2:
                        reg[0x08] = reg[0x08] | 0b00000011;
                        reg[0x0A] = reg[0x0A] | 0b00000011;
                        break;
                    case 3:
                        reg[0x0C] = reg[0x0C] | 0b00000011;
                        reg[0x0E] = reg[0x0E] | 0b00000011;
                        break;
                    case 4:
                        reg[0x10] = reg[0x10] | 0b00000011;
                        reg[0x12] = reg[0x12] | 0b00000011;
                        break;
                    case 5:
                        reg[0x14] = reg[0x14] | 0b00000011;
                        reg[0x16] = reg[0x16] | 0b00000011;
                        break;

                    case 6:
                        reg[0x00] = reg[0x00] | 0b00001100;
                        reg[0x02] = reg[0x02] | 0b00001100;
                        break;
                    case 7:
                        reg[0x04] = reg[0x04] | 0b00001100;
                        reg[0x06] = reg[0x06] | 0b00001100;
                        break;
                    case 8:
                        reg[0x08] = reg[0x08] | 0b00001100;
                        reg[0x0A] = reg[0x0A] | 0b00001100;
                        break;
                    case 9:
                        reg[0x0C] = reg[0x0C] | 0b00001100;
                        reg[0x0E] = reg[0x0E] | 0b00001100;
                        break;
                    case 10:
                        reg[0x10] = reg[0x10] | 0b00001100;
                        reg[0x12] = reg[0x12] | 0b00001100;
                        break;
                    case 11:
                        reg[0x14] = reg[0x14] | 0b00001100;
                        reg[0x16] = reg[0x16] | 0b00001100;
                        break;

                    case 12:
                        reg[0x00] = reg[0x00] | 0b00110000;
                        reg[0x02] = reg[0x02] | 0b00110000;
                        break;
                    case 13:
                        reg[0x04] = reg[0x04] | 0b00110000;
                        reg[0x06] = reg[0x06] | 0b00110000;
                        break;
                    case 14:
                        reg[0x08] = reg[0x08] | 0b00110000;
                        reg[0x0A] = reg[0x0A] | 0b00110000;
                        break;
                    case 15:
                        reg[0x0C] = reg[0x0C] | 0b00110000;
                        reg[0x0E] = reg[0x0E] | 0b00110000;
                        break;
                    case 16:
                        reg[0x10] = reg[0x10] | 0b00110000;
                        reg[0x12] = reg[0x12] | 0b00110000;
                        break;
                    case 17:
                        reg[0x14] = reg[0x14] | 0b00110000;
                        reg[0x16] = reg[0x16] | 0b00110000;
                        break;

                    case 18:
                        reg[0x00] = reg[0x00] | 0b11000000;
                        reg[0x02] = reg[0x02] | 0b11000000;
                        break;
                    case 19:
                        reg[0x04] = reg[0x04] | 0b11000000;
                        reg[0x06] = reg[0x06] | 0b11000000;
                        break;
                    case 20:
                        reg[0x08] = reg[0x08] | 0b11000000;
                        reg[0x0A] = reg[0x0A] | 0b11000000;
                        break;
                    case 21:
                        reg[0x0C] = reg[0x0C] | 0b11000000;
                        reg[0x0E] = reg[0x0E] | 0b11000000;
                        break;
                    case 22:
                        reg[0x10] = reg[0x10] | 0b11000000;
                        reg[0x12] = reg[0x12] | 0b11000000;
                        break;
                    case 23:
                        reg[0x14] = reg[0x14] | 0b11000000;
                        reg[0x16] = reg[0x16] | 0b11000000;
                        break;

                    case 24:
                        reg[0x01] = reg[0x01] | 0b00000011;
                        reg[0x03] = reg[0x03] | 0b00000011;
                        break;
                    case 25:
                        reg[0x05] = reg[0x05] | 0b00000011;
                        reg[0x07] = reg[0x07] | 0b00000011;
                        break;
                    case 26:
                        reg[0x09] = reg[0x09] | 0b00000011;
                        reg[0x0B] = reg[0x0B] | 0b00000011;
                        break;
                    case 27:
                        reg[0x0D] = reg[0x0D] | 0b00000011;
                        reg[0x0F] = reg[0x0F] | 0b00000011;
                        break;
                    case 28:
                        reg[0x11] = reg[0x11] | 0b00000011;
                        reg[0x13] = reg[0x13] | 0b00000011;
                        break;
                    case 29:
                        reg[0x15] = reg[0x15] | 0b00000011;
                        reg[0x17] = reg[0x17] | 0b00000011;
                        break;

                    case 30:
                        reg[0x01] = reg[0x01] | 0b00001100;
                        reg[0x03] = reg[0x03] | 0b00001100;
                        break;
                    case 31:
                        reg[0x05] = reg[0x05] | 0b00001100;
                        reg[0x07] = reg[0x07] | 0b00001100;
                        break;
                    case 32:
                        reg[0x09] = reg[0x09] | 0b00001100;
                        reg[0x0B] = reg[0x0B] | 0b00001100;
                        break;
                    case 33:
                        reg[0x0D] = reg[0x0D] | 0b00001100;
                        reg[0x0F] = reg[0x0F] | 0b00001100;
                        break;
                    case 34:
                        reg[0x11] = reg[0x11] | 0b00001100;
                        reg[0x13] = reg[0x13] | 0b00001100;
                        break;
                    case 35:
                        reg[0x15] = reg[0x15] | 0b00001100;
                        reg[0x17] = reg[0x17] | 0b00001100;
                        break;

                    case 36:
                        reg[0x01] = reg[0x01] | 0b00110000;
                        reg[0x03] = reg[0x03] | 0b00110000;
                        break;
                    case 37:
                        reg[0x05] = reg[0x05] | 0b00110000;
                        reg[0x07] = reg[0x07] | 0b00110000;
                        break;
                    case 38:
                        reg[0x09] = reg[0x09] | 0b00110000;
                        reg[0x0B] = reg[0x0B] | 0b00110000;
                        break;
                    case 39:
                        reg[0x0D] = reg[0x0D] | 0b00110000;
                        reg[0x0F] = reg[0x0F] | 0b00110000;
                        break;
                    case 40:
                        reg[0x11] = reg[0x11] | 0b00110000;
                        reg[0x13] = reg[0x13] | 0b00110000;
                        break;
                    case 41:
                        reg[0x15] = reg[0x15] | 0b00110000;
                        reg[0x17] = reg[0x17] | 0b00110000;
                        break;

                    case 42:
                        reg[0x01] = reg[0x01] | 0b11000000;
                        reg[0x03] = reg[0x03] | 0b11000000;
                        break;
                    case 43:
                        reg[0x05] = reg[0x05] | 0b11000000;
                        reg[0x07] = reg[0x07] | 0b11000000;
                        break;
                    case 44:
                        reg[0x09] = reg[0x09] | 0b11000000;
                        reg[0x0B] = reg[0x0B] | 0b11000000;
                        break;
                    case 45:
                        reg[0x0D] = reg[0x0D] | 0b11000000;
                        reg[0x0F] = reg[0x0F] | 0b11000000;
                        break;
                    case 46:
                        reg[0x11] = reg[0x11] | 0b11000000;
                        reg[0x13] = reg[0x13] | 0b11000000;
                        break;
                    case 47:
                        reg[0x15] = reg[0x15] | 0b11000000;
                        reg[0x17] = reg[0x17] | 0b11000000;
                        break;
                    }
                }
                // if (leds[curLED].brightness != leds[curLED].lastBrightness)
                //{

                //	I2C_WriteRegister(display_address[i], display_pwm_startreg[k], gamma8[leds[curLED].brightness]);
                //	I2C_WriteRegister(display_address[i], display_pwm_startreg[k] + 1, gamma8[leds[curLED].brightness]);
                //	I2C_WriteRegister(display_address[i], display_pwm_startreg[k] + 16, gamma8[leds[curLED].brightness]);
                //	I2C_WriteRegister(display_address[i], display_pwm_startreg[k] + 17, gamma8[leds[curLED].brightness]);

                // leds[curLED].lastBrightness = leds[curLED].brightness;
                //}
                // leds[curLED].lastBrightness = leds[curLED].brightness;
            }
        }
    }

    while (linecnt < (vres - 3))
    {
        // wait
    }

    I2C_WriteRegister(display_address[0], 0xFE, 0XC5); // Unlock
    I2C_WriteRegister(display_address[0], 0xFD, 0X00); // PG00 LED ON/OFF

    for (int k = 0; k < 6; k++)
    {
        if (ledupdatecnt == 0)
        {
            I2C_WriteRegister(display_address[0], k, reg[k]);
        }
        else if (ledupdatecnt == 1)
        {
            I2C_WriteRegister(display_address[0], k + 6, reg[k + 6]);
        }
        else if (ledupdatecnt == 2)
        {
            I2C_WriteRegister(display_address[0], k + 12, reg[k + 12]);
        }
        else if (ledupdatecnt == 3)
        {
            I2C_WriteRegister(display_address[0], k + 18, reg[k + 18]);
        }
    }
    ledupdatecnt = (ledupdatecnt + 1) % 4;
}

void DiverUI::TVP5150AM1_Setup()
{
    uint8_t decoder_address = 0xBA;                       // I2CSEL = HIGH
    I2C_WriteRegister(decoder_address, 0x03, 0b00000101); // Enable sync outputs
    I2C_WriteRegister(decoder_address, 0x0F, 0b00000000); // Enable FID output/Disable GLCO output
}

void DiverUI::I2C_WriteRegister(uint32_t address, uint8_t byte1, uint8_t byte2)
{
    uint8_t i2cData[2];
    i2cData[0] = byte1;
    i2cData[1] = byte2;
    HAL_I2C_Master_Transmit(&hi2c1, address, i2cData, 2, 100);
}

void DiverUI::OnActivateNewMode()
{
    hSlider.Realign();
    vSlider.Realign();
}

void DiverUI::OnOddField()
{
    Buttons_Poll();
    Display_Refresh();
}

void DiverUI::OnInterruptHSync()
{
    ADC_ChannelConfTypeDef sConfig;

    // if (linecnt > 20) { __HAL_TIM_ENABLE(&htim1); }
    uint16_t sample;

    // HAL_ADC_PollForConversion(&hadc1, 1);
    sample = 4095 - HAL_ADC_GetValue(&hadc1);
    if (sample >= 4095 + 256 - 1024)
    {
        sample = 4095 + 256 - 1024;
    }
    else if (sample <= 2047 + 256 - 1024)
    {
        sample = 2047 + 256 - 1024;
    }
    sample -= 2047 + 256 - 1024;
    sample = sample >> 1;
    // sample = (samples[sampleWritePtr][linecnt - VBLANK] + sample) >> 1;
    // samples[sampleWritePtr][linecnt - VBLANK] = (sample + samples[sampleWritePtr][linecnt - VBLANK - 1]) >> 1;
    // samples[sampleWritePtr][linecnt - VBLANK] = (sample + samples[sampleWritePtr][linecnt - VBLANK - 1]) >> 1;
    if (sample > 1023)
    {
        sample = 1023;
    }
    if (linecnt == 0)
    {
        samples_wave[sampleWritePtr][linecnt - VBLANK] = sample;
    }
    else if (linecnt < 3)
    {
        samples_wave[sampleWritePtr][linecnt - VBLANK] = (sample + samples_wave[sampleWritePtr][linecnt - VBLANK - 1]) >> 1;
    }
    else
    {
        samples_wave[sampleWritePtr][linecnt - VBLANK] = (sample + samples_wave[sampleWritePtr][linecnt - VBLANK - 1] + samples_wave[sampleWritePtr][linecnt - VBLANK - 2] + samples_wave[sampleWritePtr][linecnt - VBLANK - 3]) >> 2;
    }

    // samples[sampleWritePtr][linecnt - VBLANK] = (4095 - HAL_ADC_GetValue(&hadc1)) >> 2;
    // sConfig.Channel = ADC_CHANNEL_0;
    // sConfig.Rank = 2;
    // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    // sConfig.Channel = ADC_CHANNEL_1;
    // sConfig.Rank = 3;
    // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = 1;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    // sConfig.Channel = ADC_CHANNEL_3;
    // sConfig.Rank = 4;
    // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 2;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1);
    sample = 4095 - HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1);
    sample = (sample + 4095 - HAL_ADC_GetValue(&hadc1)) >> 1;
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1);
    sample = (sample + 4095 - HAL_ADC_GetValue(&hadc1)) >> 1;
    /*if (sample >= 4095 + 0 - 2048)
    {
        sample = 4095 + 0 - 2048;
    }
    else if (sample <= 0)
    {
        sample = 0;
    }
    sample -= 0;*/
    if (sample <= 32)
    {
        sample = 32;
    }
    // sample -= 32;
    sample = (sample >> 1);

    if (sample >= 709)
    {
        sample = 709;
    }
    // sample = (hres + sample - 185) % hres;

    // sample = (samples_hphase[sampleWritePtr][(vres + linecnt - VBLANK - 3) % vres] + samples_hphase[sampleWritePtr][(vres + linecnt - VBLANK - 2) % vres] + samples_hphase[sampleWritePtr][(vres + linecnt - VBLANK - 1) % vres] + sample) >> 2;

    // if (sample > vres)
    //{
    //	sample = vres;
    // }
    // if(sample > 1023)
    //{
    //	sample = 1023;
    // }

    if (linecnt == 0)
    {
        // samples_hphase[sampleWritePtr][linecnt - VBLANK] = (sample + samples_hphase[(4 + (sampleWritePtr - 1)) % 4][vres - 1]) >> 1;
        // samples_hphase[sampleWritePtr][linecnt - VBLANK] = (sample + samples_hphase[(4 + sampleWritePtr - 1) % 4][vres - 1] + samples_hphase[(4 + sampleWritePtr - 1) % 4][vres - 2] + samples_hphase[(4 + sampleWritePtr - 1) % 4][vres - 3]) >> 2;
        samples_hphase_cv[sampleWritePtr][linecnt - VBLANK] = sample;
    }
    else if (linecnt < 3)
    {
        samples_hphase_cv[sampleWritePtr][linecnt - VBLANK] = (sample + samples_hphase_cv[sampleWritePtr][linecnt - VBLANK - 1]) >> 1;
    }
    else
    {
        samples_hphase_cv[sampleWritePtr][linecnt - VBLANK] = (sample + samples_hphase_cv[sampleWritePtr][linecnt - VBLANK - 1] + samples_hphase_cv[sampleWritePtr][linecnt - VBLANK - 2] + samples_hphase_cv[sampleWritePtr][linecnt - VBLANK - 3]) >> 2;
    }

    // samples_hphase[sampleWritePtr][linecnt - VBLANK] = (4095 - HAL_ADC_GetValue(&hadc1)) >> 2;
    if (HAL_GPIO_ReadPin(TRIG_IN_GPIO_Port, TRIG_IN_Pin) && trigger_state == 0)
    {
        trigger_rising++;
        trigger_state = 1;
    }
    else if (!HAL_GPIO_ReadPin(TRIG_IN_GPIO_Port, TRIG_IN_Pin) && trigger_state == 1)
    {
        trigger_falling++;
        trigger_state = 0;
    }

    if (linecnt == 0)
    {

        sConfig.Channel = ADC_CHANNEL_0;
        sConfig.Rank = 1;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_1;
        sConfig.Rank = 2;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_2;
        sConfig.Rank = 3;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_3;
        sConfig.Rank = 4;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_4;
        sConfig.Rank = 5;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        if (sample >= MAX_SLIDER_VALUE)
        {
            sample = MAX_SLIDER_VALUE;
        }

        vSlider.ProcessControlRate(1.0f * sample / MAX_SLIDER_VALUE);
        vSlider.ProcessUIRate();
        state.vphase_slider = uint16_t(state.param_vphase * vres);
    }
    else if (linecnt == 1)
    {
        // HAL_ADC_PollForConversion(&hadc1, 1);
        // sample = HAL_ADC_GetValue(&hadc1);
        // HAL_ADC_Start(&hadc1);
        // Reset to sample hphase slider
        sConfig.Channel = ADC_CHANNEL_0;
        sConfig.Rank = 2;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_1;
        sConfig.Rank = 1;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_2;
        sConfig.Rank = 3;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_3;
        sConfig.Rank = 4;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_4;
        sConfig.Rank = 5;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = HAL_ADC_GetValue(&hadc1) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + HAL_ADC_GetValue(&hadc1)) >> 1;
        // sample = (sample + (HAL_ADC_GetValue(&hadc1))) >> 1;
        if (sample >= MAX_SLIDER_VALUE)
        {
            sample = MAX_SLIDER_VALUE;
        }

        hSlider.ProcessControlRate(1.0f - (1.0f * sample / MAX_SLIDER_VALUE));
        hSlider.ProcessUIRate();
        state.hphase_slider = uint16_t((1.0f - state.param_hphase) * hres);
    }
    else if (linecnt == 2)
    {
        // Reset to sample vphase CV
        sConfig.Channel = ADC_CHANNEL_0;
        sConfig.Rank = 2;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_1;
        sConfig.Rank = 3;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_2;
        sConfig.Rank = 4;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_3;
        sConfig.Rank = 1;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        sConfig.Channel = ADC_CHANNEL_4;
        sConfig.Rank = 5;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        HAL_ADC_Start(&hadc1);

        HAL_ADC_PollForConversion(&hadc1, 1);
        // sample = (sample + (4095 - HAL_ADC_GetValue(&hadc1))) >> 1;
        sample = 4095 - HAL_ADC_GetValue(&hadc1);

        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + 4095 - HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + 4095 - HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + 4095 - HAL_ADC_GetValue(&hadc1)) >> 1;
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 1);
        sample = (sample + 4095 - HAL_ADC_GetValue(&hadc1)) >> 1;
        if (sample >= 4095 + 361 - 2048)
        {
            sample = 4095 + 361 - 2048;
        }
        else if (sample <= 361)
        {
            sample = 361;
        }
        sample -= 361;
        sample = ((sample)*vres) / 2035;
        state.vphase_cv = ((state.vphase_cv * 3 + sample) >> 2);
    }

    state.vphase_slider &= 0b1111111111110;
    state.vphase_cv &= 0b1111111111110;

    // sConfig.Channel = ADC_CHANNEL_0;
    // sConfig.Rank = 2;
    // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    // sConfig.Channel = ADC_CHANNEL_1;
    // sConfig.Rank = 3;
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = 2;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    // sConfig.Channel = ADC_CHANNEL_3;
    // sConfig.Rank = 4;
    // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start(&hadc1);
}
