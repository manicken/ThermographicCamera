#include <Arduino.h>
#include <Adafruit_MLX90640.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "interpolation.h"
#include "GradientPalettes.h"
#include "HTMLColors.h"

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

#define COLOR_WHITE    0xFFFFFF
#define COLOR_GOLD     0xFFD700
#define COLOR_ORANGE   0xFFA500
#define COLOR_MAGENTA_RED 0xCC0077
#define COLOR_DARKBLUE 0x20008C
#define COLOR_BLACK    0x000000
const uint32_t IronBowPaletteBaseColors[] = {
    COLOR_WHITE, COLOR_GOLD, COLOR_ORANGE, COLOR_MAGENTA_RED, COLOR_DARKBLUE, COLOR_BLACK
}; // above have root multipliers 3,3,7,5,2 => total 20
// 240/20 = 12
// this will contain one less item the the above as every item here just defines the steps until the next color
const uint8_t IronBowPaletteBase_Steps[] = {
    12*3,12*3,12*7,12*5,12*2
}

// interpolate base colors, pseudo code
// want 240 color map
// BASECOLORS_COUNT = 8
// 240-BASECOLORS_COUNT = 232
// 232/(BASECOLORS_COUNT-1) = ~33 steps between each
// 33*(BASECOLORS_COUNT-1) = 231
// 231+BASECOLORS_COUNT = 239 total colors returned
// or better just define how many steps between the base colors
// then above calculations would not be necessary
// and would also make it possible to do specific color maps
// with fewer base colors


//the colors we will be using
const uint16_t camColors[] = {0x480F,
0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
0xF080,0xF060,0xF040,0xF020,0xF800,};

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

void setup() {
    //while (!Serial) delay(10);
    Serial.begin(115200);
    delay(100);

    tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
    tft.setTextWrap(false); // Allow text to run off right edge
    tft.fillScreen(ST77XX_BLACK);
    tft.enableDisplay(true);
    tft.setRotation(2);
  
    for (int i=0;i<10;i++)
        tft.drawRGBBitmap(0, 200+i, &camColors[5], 240, 1);

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
    
    mlx.setResolution(MLX90640_ADC_16BIT);
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
            uint8_t colorIndex = map(t, minTemp, maxTemp, 0, 255);
            colorIndex = constrain(colorIndex, 0, 255);
            dest[index] = camColors[colorIndex];
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
            uint8_t colorIndex = constrain(map(t, minTemp, maxTemp, 0, 255), 0, 255);
            uint16_t color = camColors[colorIndex];

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
            uint8_t colorIndex = map(t, minTemp, maxTemp, 0, 255);
            colorIndex = constrain(colorIndex, 0, 255);
            tft.fillRect((32-w)*7, h*7, 7, 7, camColors[colorIndex]);
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
    tft.fillRect(0, 180, 84, 14, ST77XX_BLACK);
    tft.fillRect(180, 180, 60, 14, ST77XX_BLACK);

    // print new text
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(0, 180);
    tft.print(minTemp);
    tft.setCursor(180, 180);
    tft.print(maxTemp);
}
int read_status = 0;
uint32_t fps = 0;
void loop() {
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
    tft.fillRect(0, 230, 60, 14, ST77XX_BLACK);
    tft.setCursor(0, 230);
    tft.printf("fps:%d", fps);
}