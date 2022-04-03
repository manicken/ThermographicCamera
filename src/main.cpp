#include <Arduino.h>
#include <Adafruit_MLX90640.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "interpolation.h"
#include "GradientPalettes.h"
#include "HTMLColors.h"

#include <Button.h>

#define TFT_SCK       13
#define TFT_MOSI      11
#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8

uint32_t t = 0;// used for easy timing stuff

Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

// uncomment *one* of the below
//#define PRINT_TEMPERATURES
//#define PRINT_ASCIIART
#define PRINT_TFT

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
#define COLOR_PALETTE_COUNT 240

CRGB camColors[COLOR_PALETTE_COUNT];

void setGradientColorMap(int16_t index)
{
    if (index >= GP_Def_Count) index = GP_Def_Count-1;
    if (index < 0) index = 0;
    uint16_t dataIndex = getDataIndex(index);
    generateGradientColorMap(camColors,  GP_Def[index].itemCount, &GP_Data[dataIndex], COLOR_PALETTE_COUNT);

    // print the colormap name
    tft.fillRect(120, 230, 120, 7, ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(120, 230);
    tft.print(GP_Def[index].name);

    // draw the temp-range colormap 
    for (int i=0;i<10;i++)
        drawRGBBitmap(0, 200+i, camColors, 240, 1);
}

void printMLX_current_settings()
{
    Serial.print("Current mode: ");
    if (mlx.getMode() == MLX90640_CHESS) {
        Serial.println("Chess");
    } else {
        Serial.println("Interleave");    
    }
    Serial.print("Current resolution: ");
    mlx90640_resolution_t res = mlx.getResolution();
    switch (res) {
        case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
        case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
        case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
        case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
    }
    Serial.print("Current frame rate: ");
    mlx90640_refreshrate_t rate = mlx.getRefreshRate();
    switch (rate) {
        case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
        case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
        case MLX90640_2_HZ: Serial.println("2 Hz"); break;
        case MLX90640_4_HZ: Serial.println("4 Hz"); break;
        case MLX90640_8_HZ: Serial.println("8 Hz"); break;
        case MLX90640_16_HZ: Serial.println("16 Hz"); break;
        case MLX90640_32_HZ: Serial.println("32 Hz"); break;
        case MLX90640_64_HZ: Serial.println("64 Hz"); break;
    }
}
#define ST77XX_ColorMode_444 0x03
#define ST77XX_ColorMode_565 0x05
#define ST77XX_ColorMode_666 0x06

void setColorMode(uint8_t mode)
{
    tft.sendCommand(ST77XX_COLMOD, &mode, 1);
}




Button btnUp(5);
Button btnDown(6);

int16_t currentColorMapIndex = 0;

void setup() {
    btnUp.begin();
    btnDown.begin();
    //while (!Serial) delay(10);
    Serial.begin(115200);
    delay(100);

    

    tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels

    //setColorMode(ST77XX_ColorMode_565);

    tft.setTextWrap(false); // Allow text to run off right edge
    tft.fillScreen(ST77XX_BLACK);
    tft.enableDisplay(true);
    tft.setRotation(2);

    setGradientColorMap(currentColorMapIndex);
  
    

    Serial.println("Adafruit MLX90640 Simple Test");
    Wire.setClock(1000000);
    if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
        Serial.println("MLX90640 not found!");
        while (1) delay(10);
    }
    Wire.setClock(1000000);
    Serial.println("Found Adafruit MLX90640");

    Serial.print("Serial number: ");
    Serial.print(mlx.serialNumber[0], HEX);
    Serial.print(mlx.serialNumber[1], HEX);
    Serial.println(mlx.serialNumber[2], HEX);
    
    //mlx.setMode(MLX90640_INTERLEAVED);
    mlx.setMode(MLX90640_CHESS);
    
    mlx.setResolution(MLX90640_ADC_19BIT);
    mlx.setRefreshRate(MLX90640_16_HZ);


    printMLX_current_settings();
}

float minTemp = 0.0f;
float maxTemp = 0.0f;

void getMinMaxTemps()
{
    minTemp = 500.0f;
    maxTemp = -40.0f;
    for (uint8_t h=0; h<24; h++) {
        for (uint8_t w=0; w<32; w++) {
            float t = frame[h*32 + w];
            if (t > maxTemp)
                maxTemp = t;
            if (t < minTemp)
                minTemp = t;
        }
    }
}

#define INTERPOLATED_COLS 224
#define INTERPOLATED_ROWS 168
float dest_2d[INTERPOLATED_ROWS * INTERPOLATED_COLS];

uint16_t dest_RGB565_map[INTERPOLATED_ROWS * INTERPOLATED_COLS];

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
}

void tft_draw_Interpolated(float *src, int16_t rows, int16_t cols)
{
    tft.startWrite();
    tft.setAddrWindow(0, 0, cols, rows);
    for (int16_t r=0; r<rows; r++) {
        for (int16_t c=cols-1; c>=0; c--) { // draw cols in reverse order because data from MLX is reversed
            int index = r*cols + c;
            float t = src[index];
            uint8_t colorIndex = constrain(map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1), 0, COLOR_PALETTE_COUNT-1);
            uint16_t color = camColors[colorIndex].toRGB565();

            tft.SPI_WRITE16(color);
        }
    }
    
    tft.endWrite();
}

void clearDest()
{
    for (int i=0;i<INTERPOLATED_ROWS*INTERPOLATED_COLS;i++)
        dest_2d[i] = 0.0f;
}

void tft_printBiqubicInterpolated()
{
    //t = millis();
    clearDest();
    interpolate_image(frame, 24, 32, dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
    //Serial.print("Interpolation took "); Serial.print(millis()-t); Serial.println(" ms");
    //t = millis();
    tft_draw_Interpolated(dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS); // this takes almost the same time ~34mS as the following two, but would not require additional ram usage
    //convertToImage(dest_2d, dest_RGB565_map, INTERPOLATED_ROWS, INTERPOLATED_COLS);
    //tft.drawRGBBitmap(0,0, dest_RGB565_map, INTERPOLATED_COLS, INTERPOLATED_ROWS);
    //Serial.print("Interpolation draw took "); Serial.print(millis()-t); Serial.println(" ms");
}

void tft_printNonInterpolated()
{
    for (uint8_t h=0; h<24; h++) {
        for (uint8_t w=0; w<32; w++) {
            float t = frame[h*32 + w];
            uint8_t colorIndex = map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1);
            colorIndex = constrain(colorIndex, 0, COLOR_PALETTE_COUNT-1);
            tft.fillRect((32-w)*7, h*7, 7, 7, camColors[colorIndex].toRGB565());
        }
    }
}

void serial_printTempValues()
{
    for (uint8_t h=0; h<24; h++) {
        for (uint8_t w=0; w<32; w++) {
            float t = frame[h*32 + w];
            Serial.print(t, 1);
            Serial.print(", ");
        }
        Serial.println();
    }
}

void serial_printAsASCIIART()
{
    //Serial.println();
    //Serial.println();
    for (uint8_t h=0; h<24; h++) {
        for (uint8_t w=0; w<32; w++) {
            float t = frame[h*32 + w];
            if (t < 20) Serial.print("  ");// c = ' ';
            else if (t < 23) Serial.print("..");// c = '.';
            else if (t < 25) Serial.print("--");// c = '-';
            else if (t < 27) Serial.print("**");// c = '*';
            else if (t < 29) Serial.print("++");// c = '+';
            else if (t < 31) Serial.print("xx");// c = 'x';
            else if (t < 33) Serial.print("%%");// c = '%';
            else if (t < 35) Serial.print("##");// c = '#';
            else if (t < 37) Serial.print("XX");// c = 'X';
            else Serial.print("&&");
        }
        Serial.println();
    }
}

void tft_printMaxMin()
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
int read_status = 0;
uint32_t fps = 0;
void loop() {
    if (btnDown.pressed() && currentColorMapIndex != (GP_Def_Count-1)){
        currentColorMapIndex++;
        //if (currentColorMapIndex == GP_Def_Count) currentColorMapIndex = (GP_Def_Count-1);
        setGradientColorMap(currentColorMapIndex);
    }
    if (btnUp.pressed() && currentColorMapIndex != 0){
        currentColorMapIndex--;
        //if (currentColorMapIndex == 0) currentColorMapIndex = (GP_Def_Count-1);
        setGradientColorMap(currentColorMapIndex);
    }
    //delay(100);
    fps = millis();
    //t = millis();
    if (read_status != 0) tft.fillRect(0, 220, 240, 14, ST77XX_BLACK);
    read_status = mlx.getFrame(frame);
    if (read_status != 0) {
        Serial.printf("Failed:%d", read_status);
        tft.fillRect(0, 220, 240, 14, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_RED);
        tft.setCursor(0, 220);
        tft.printf("MLX Failed:%d", read_status);
        return;
    }
    //Serial.printf("mlx.getFrame time:%d\n",(millis()-t));
    
    getMinMaxTemps();
    
    //t = millis();
    tft_printMaxMin();
    tft_printBiqubicInterpolated();
    //tft_printNonInterpolated();
    //serial_printTempValues();
    //serial_printAsASCIIART();

    //Serial.print("Redraw took "); Serial.print(millis()-t); Serial.println(" ms");

    fps = (1000/(millis()-fps));
    tft.setTextSize(1);
    tft.fillRect(0, 230, 60, 7, ST77XX_BLACK);
    tft.setCursor(0, 230);
    tft.printf("fps:%d", fps);
}