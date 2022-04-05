
#include <Arduino.h>
#include "GradientPalette_Structs.h"
#include <Adafruit_ST7789.h>
/*
 * note this don't take care of clipping
 * or out of boundaries
 * so special care must be taken when using it
 */
void drawRGBBitmap(Adafruit_ST7789 tft, int16_t x, int16_t y, CRGB *pcolors,
                                    int16_t w, int16_t h)
{
    tft.startWrite();
    tft.setAddrWindow(x, y, w, h);
    for (int16_t r=0; r<h; r++) {
        for (int16_t c=0; c<w; c++) {
            int index = r*w + c;
            tft.SPI_WRITE16(pcolors[index].toRGB565());
        }
    }
    tft.endWrite();
}

typedef enum {
    RGB444 = 0x03,
    RGB565 = 0x05,
    RGB666 = 0x06
}ST77XX_ColorMode;

void setColorMode(Adafruit_ST7789 tft, ST77XX_ColorMode mode)
{
    uint8_t _mode = ((uint8_t)mode);
    tft.sendCommand(ST77XX_COLMOD, &_mode, 1);
}