#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#ifdef __cplusplus
#include <cstdint>
#define C_EXTERN extern "C"
#else
#include <stdint.h>
#define C_EXTERN extern
#endif

#ifdef MAIN_CPP
#define GLOBAL_EXTERN 
#else
#define GLOBAL_EXTERN C_EXTERN
#endif

#define NUM_BUFFERS 4
#define MAX_BUFFER_SIZE (625*2)

C_EXTERN TIM_HandleTypeDef htim1;

#ifndef MAIN_CPP
GLOBAL_EXTERN void on_framestart();
GLOBAL_EXTERN void on_hsync();
GLOBAL_EXTERN void on_bank_changed();
#endif

GLOBAL_EXTERN uint16_t samples_wave[NUM_BUFFERS][MAX_BUFFER_SIZE];
GLOBAL_EXTERN uint16_t samples_hphase_cv[NUM_BUFFERS][MAX_BUFFER_SIZE];
GLOBAL_EXTERN uint16_t hwave[NUM_BUFFERS][MAX_BUFFER_SIZE];
GLOBAL_EXTERN uint16_t vwave[NUM_BUFFERS][MAX_BUFFER_SIZE];
GLOBAL_EXTERN uint16_t hphase_cv[NUM_BUFFERS][MAX_BUFFER_SIZE];

//Memory pointers and enables
GLOBAL_EXTERN uint8_t waveReadPtr;
GLOBAL_EXTERN uint8_t waveWritePtr;
GLOBAL_EXTERN uint8_t sampleReadPtr;
GLOBAL_EXTERN uint8_t sampleWritePtr;
GLOBAL_EXTERN uint8_t waveRenderComplete;
GLOBAL_EXTERN uint8_t captureEnable;

//Application variables
GLOBAL_EXTERN uint16_t hphase_slider;
GLOBAL_EXTERN uint16_t vphase_slider;
GLOBAL_EXTERN uint16_t vphase_cv;
GLOBAL_EXTERN uint16_t hphase;
GLOBAL_EXTERN uint16_t vphase;
GLOBAL_EXTERN uint16_t hphasecnt;
GLOBAL_EXTERN uint16_t vphasecnt;

GLOBAL_EXTERN uint16_t hres;
GLOBAL_EXTERN uint16_t vres;

//Video timing
GLOBAL_EXTERN uint8_t hsync_event;
GLOBAL_EXTERN uint8_t evenfield_event;
GLOBAL_EXTERN uint8_t oddfield_event;
GLOBAL_EXTERN uint8_t field;
GLOBAL_EXTERN uint8_t vsync;
GLOBAL_EXTERN uint16_t linecnt;
GLOBAL_EXTERN uint16_t lines_per_oddfield;
GLOBAL_EXTERN uint16_t lines_per_evenfield;
GLOBAL_EXTERN uint16_t lines_per_frame;
GLOBAL_EXTERN uint32_t dropped_frames;


#endif  // __GLOBALS_H__