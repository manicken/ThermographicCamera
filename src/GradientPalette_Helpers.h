
#include <Arduino.h>
#include "GradientPalettes.h"
#include "GradientPalette_Structs.h"

uint16_t getDataIndex(uint16_t gpDefIndex)
{
    uint16_t index = 0;
    for (int i=0;i<GP_Def_Count;i++)
    {
        if (i == gpDefIndex) return index;
        index += GP_Def[i].itemCount;
    }
    return 0;
}

void fill_gradient_RGB( CRGB* leds,
                   uint16_t startpos, CRGB startcolor,
                   uint16_t endpos,   CRGB endcolor )
{
    // if the points are in the wrong order, straighten them
    if( endpos < startpos ) {
        uint16_t t = endpos;
        CRGB tc = endcolor;
        endcolor = startcolor;
        endpos = startpos;
        startpos = t;
        startcolor = tc;
    }

    int16_t rdistance87;
    int16_t gdistance87;
    int16_t bdistance87;

    rdistance87 = (endcolor.r - startcolor.r) << 7;
    gdistance87 = (endcolor.g - startcolor.g) << 7;
    bdistance87 = (endcolor.b - startcolor.b) << 7;

    uint16_t pixeldistance = endpos - startpos;
    int16_t divisor = pixeldistance ? pixeldistance : 1;

    int16_t rdelta87 = rdistance87 / divisor;
    int16_t gdelta87 = gdistance87 / divisor;
    int16_t bdelta87 = bdistance87 / divisor;

    rdelta87 *= 2;
    gdelta87 *= 2;
    bdelta87 *= 2;

    uint16_t r88 = startcolor.r << 8;
    uint16_t g88 = startcolor.g << 8;
    uint16_t b88 = startcolor.b << 8;
    for( uint16_t i = startpos; i <= endpos; i++) {
        leds[i] = CRGB( r88 >> 8, g88 >> 8, b88 >> 8);
        r88 += rdelta87;
        g88 += gdelta87;
        b88 += bdelta87;
    }
}

void generateGradientColorMap(CRGB* colorMap, uint16_t gradientCount, const GradientPaletteItem* gradients, uint16_t totalRange)
{
    for (int i = 0; i < (gradientCount-1); i++)
    {
        fill_gradient_RGB(
            colorMap, 
            (totalRange * gradients[i].procent) / 100,
            gradients[i].color, 
            (i != (gradientCount-2))?(totalRange * gradients[i+1].procent) / 100:(totalRange-1),
            gradients[i+1].color
        );
    }
}
