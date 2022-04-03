
#include <Arduino.h>
#include "GradientPalettes.h"

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