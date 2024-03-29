#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "GradientPalette_Structs.h"
#include "GBlur.h"
#include <TeensyThreads.h>

namespace Main
{
    typedef enum {
        UNKNOWN_FIRST = -1,
        TFT_RAW_PIXELS = 0,
        TFT_BIQUBIC_INTERPOLATE = 1,
        USB_VIDEO_STREAM = 2,
        USB_VIDEO_STREAM_RAW = 3,
        USB_ASCII_ART = 4,
        USB_TEMP_VALUES_ASCII = 5,
        UNKNOWN_LAST = 6
    } OUTPUT_TARGET;

    typedef enum {
        START = 0
    } MAIN_THREAD_STATE;

    MAIN_THREAD_STATE mainState = MAIN_THREAD_STATE::START;

    OUTPUT_TARGET outTarget = OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE; 
    void(*CallBack_outTarget_Print)(float);

    void(*CallBack_outTarget_Interpolate)(void);

    void nonInterpolate(void) { }

    #define COLOR_PALETTE_COUNT 320


    CRGB camColors[COLOR_PALETTE_COUNT];
    int16_t currentColorMapIndex = 2;


    float dest_2d[320 * 240];

    GBlur gblur;
    float gblurTemp[32*2*24*2];

}

#endif