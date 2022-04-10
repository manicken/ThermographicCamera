
#include <Arduino.h>
#include "main.h"
#include "ThermalCamera.h"
#include "GradientPalette_Structs.h"
#include "interpolation.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

namespace Display
{
    uint16_t INTERPOLATED_COLS = 224;
    uint16_t INTERPOLATED_ROWS = 168;

    #define TFT_SCK       13
    #define TFT_MOSI      11
    #define TFT_CS        10
    #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
    #define TFT_DC         8

    Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

    void Init()
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

    void printMaxMin()
    {
        // clear prev text
        tft.fillRect(0, 180, 240, 14, ST77XX_BLACK);
        //tft.fillRect(180, 180, 60, 14, ST77XX_BLACK);

        // print new text
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(0, 180);
        tft.print(ThermalCamera::minTemp);
        tft.setCursor(90, 180);
        tft.print((ThermalCamera::maxTemp-ThermalCamera::minTemp)/2+ThermalCamera::minTemp);
        tft.setCursor(180, 180);
        tft.print(ThermalCamera::maxTemp);
    }

    void print_temperatures()
    {
        tft.startWrite();
        tft.setAddrWindow(0, 0, INTERPOLATED_COLS, INTERPOLATED_ROWS);
        for (int16_t r=0; r<INTERPOLATED_ROWS; r++) {
            for (int16_t c=INTERPOLATED_COLS-1; c>=0; c--) { // draw cols in reverse order because data from MLX is reversed
                int index = r*INTERPOLATED_COLS + c;
                float t = Main::dest_2d[index];
                uint8_t colorIndex = constrain(map(t, ThermalCamera::minTemp, ThermalCamera::maxTemp, 0, COLOR_PALETTE_COUNT-1), 0, COLOR_PALETTE_COUNT-1);
                uint16_t color = Main::camColors[colorIndex].toRGB565();

                tft.SPI_WRITE16(color);
            }
        }
        
        tft.endWrite();
    }

    void printNonInterpolated()
    {
        for (uint8_t h=0; h<24; h++) {
            for (uint8_t w=0; w<32; w++) {
                float t = ThermalCamera::frame[h*32 + w];
                uint8_t colorIndex = map(t, ThermalCamera::minTemp, ThermalCamera::maxTemp, 0, COLOR_PALETTE_COUNT-1);
                colorIndex = constrain(colorIndex, 0, COLOR_PALETTE_COUNT-1);
                tft.fillRect((31-w)*7, h*7, 7, 7, Main::camColors[colorIndex].toRGB565());
            }
        }
    }

    void printCurrentGradientColorPalette()
    {
        // print the colormap name
        tft.fillRect(120, 230, 100, 7, ST77XX_BLACK);
        tft.setTextSize(1);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(120, 230);
        tft.print(GradientPalettes::Def[Main::currentColorMapIndex].name);

        // draw the temp-range colormap 
        for (int i=0;i<10;i++)
            drawRGBBitmap(0, 200+i, Main::camColors, 240, 1);
    }

    void printFps(uint32_t fps)
    {
        tft.setTextSize(1);
        tft.fillRect(0, 230, 60, 7, ST77XX_BLACK);
        tft.setCursor(0, 230);
        tft.printf("fps:%d", fps);
    }

    void printStatusMsg(int read_status)
    {
        //tft.fillRect(0, 220, 240, 14, ST77XX_BLACK);
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

    void print_BiqubicInterpolated()
    {
        printMaxMin();
        //t = millis();
        interpolate_image(ThermalCamera::frame, 24, 32, Main::dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
        //Serial.print("Interpolation took "); Serial.print(millis()-t); Serial.println(" ms");
        //t = millis();
        print_temperatures(); // this takes almost the same time ~34mS as the following two, but would not require additional ram usage
        //Serial.print("Interpolation draw took "); Serial.print(millis()-t); Serial.println(" ms");
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

}