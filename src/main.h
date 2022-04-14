#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "GradientPalette_Structs.h"
#include <TeensyThreads.h>

namespace Main
{
    typedef enum {
        UNKNOWN_FIRST = -1,
        TFT_RAW_PIXELS = 0,
        TFT_BIQUBIC_INTERPOLATE = 1,
        USB_VIDEO_STREAM = 2,
        USB_ASCII_ART = 3,
        USB_TEMP_VALUES_ASCII = 4,
        UNKNOWN_LAST = 5
    } OUTPUT_TARGET;

    typedef enum {
        START = 0
    } MAIN_THREAD_STATE;

    MAIN_THREAD_STATE mainState = MAIN_THREAD_STATE::START;

    OUTPUT_TARGET outTarget = OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE; 
    void(*CallBack_outTarget_Print)(void);

    void(*CallBack_outTarget_Interpolate)(void);

    void nonInterpolate(void) { }

    #define COLOR_PALETTE_COUNT 320


    CRGB camColors[COLOR_PALETTE_COUNT];
    int16_t currentColorMapIndex = 1;


    float dest_2d[320 * 240];

   

}

#endif