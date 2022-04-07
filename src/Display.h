
#include <Arduino.h>
#include "GradientPalette_Structs.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#define TFT_SCK       13
#define TFT_MOSI      11
#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

void Display_Init()
{
    tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels

    //setColorMode(tft, ST77XX_ColorMode::RGB565);

    tft.setTextWrap(false); // Allow text to run off right edge
    tft.fillScreen(ST77XX_BLACK);
    tft.enableDisplay(true);
    tft.setRotation(2);
}


/*
 * note this don't take care of clipping
 * or out of boundaries
 * so special care must be taken when using it
 */
void drawRGBBitmap(int16_t x, int16_t y, CRGB *pcolors,
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

void setColorMode(ST77XX_ColorMode mode)
{
    uint8_t _mode = ((uint8_t)mode);
    tft.sendCommand(ST77XX_COLMOD, &_mode, 1);
}

void Display_printMaxMin(float minTemp, float maxTemp)
{
    // clear prev text
    tft.fillRect(0, 180, 240, 14, ST77XX_BLACK);
    //tft.fillRect(180, 180, 60, 14, ST77XX_BLACK);

    // print new text
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(0, 180);
    tft.print(minTemp);
    tft.setCursor(90, 180);
    tft.print((maxTemp-minTemp)/2+minTemp);
    tft.setCursor(180, 180);
    tft.print(maxTemp);
}

void Display_print_temperatures(float *src, CRGB *pcolors, int16_t rows, int16_t cols, float minTemp, float maxTemp, uint16_t COLOR_PALETTE_COUNT)
{
    tft.startWrite();
    tft.setAddrWindow(0, 0, cols, rows);
    for (int16_t r=0; r<rows; r++) {
        for (int16_t c=cols-1; c>=0; c--) { // draw cols in reverse order because data from MLX is reversed
            int index = r*cols + c;
            float t = src[index];
            uint8_t colorIndex = constrain(map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1), 0, COLOR_PALETTE_COUNT-1);
            uint16_t color = pcolors[colorIndex].toRGB565();

            tft.SPI_WRITE16(color);
        }
    }
    
    tft.endWrite();
}

void Display_printNonInterpolated(float *src, CRGB *pcolors, float minTemp, float maxTemp, uint16_t COLOR_PALETTE_COUNT)
{
    for (uint8_t h=0; h<24; h++) {
        for (uint8_t w=0; w<32; w++) {
            float t = src[h*32 + w];
            uint8_t colorIndex = map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1);
            colorIndex = constrain(colorIndex, 0, COLOR_PALETTE_COUNT-1);
            tft.fillRect((31-w)*7, h*7, 7, 7, pcolors[colorIndex].toRGB565());
        }
    }
}

void Display_printCurrentGradientColorPalette(CRGB *camColors, const char *name)
{
    // print the colormap name
    tft.fillRect(120, 230, 120, 7, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(120, 230);
    tft.print(name);

    // draw the temp-range colormap 
    for (int i=0;i<10;i++)
        drawRGBBitmap(0, 200+i, camColors, 240, 1);
}

void Display_printFps(uint32_t fps)
{
    tft.setTextSize(1);
    tft.fillRect(0, 230, 60, 7, ST77XX_BLACK);
    tft.setCursor(0, 230);
    tft.printf("fps:%d", fps);
}

void Display_printStatusMsg(int read_status)
{
    tft.fillRect(0, 220, 240, 14, ST77XX_BLACK);
    if (read_status != 0)
    {
        Serial.printf("log Failed:%d", read_status);
        tft.fillRect(0, 220, 240, 14, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_RED);
        tft.setCursor(0, 220);
        tft.printf("MLX Failed:%d", read_status);
    }
}

/* not used in current design but keep it here just in case
 * as this will just consume unnecessary RAM memory
void convertToImage(float *src, uint16_t *dest, uint8_t rows, uint8_t cols)
{
    for (uint8_t r=0; r<rows; r++) {
        for (uint8_t c=0; c<cols; c++) {
            int index = r*cols + c;
            float t = src[index];
            uint8_t colorIndex = map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1);
            colorIndex = constrain(colorIndex, 0, COLOR_PALETTE_COUNT-1);
            dest[index] = camColors[colorIndex].toRGB565();
        }
    }
}*/