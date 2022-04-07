#include <Arduino.h>
#include "GradientPalette_Structs.h"
#include "interpolation.h"

uint16_t USB_STREAM_INTERPOLATED_COLS = 224;
uint16_t USB_STREAM_INTERPOLATED_ROWS = 168;

void USBSerial_PrintPalette(CRGB *pColors, int COLOR_PALETTE_COUNT)
{
    Serial.printf("imgGP %d\n", COLOR_PALETTE_COUNT*10*3);
    for (int ri=0;ri<10;ri++)
    {
        for (int i=0;i<COLOR_PALETTE_COUNT;i++)
            Serial.write(&pColors[i].arr[1], 3);
    }
}

void serial_print_temperatures(float *src, CRGB *pColors, int16_t rows, int16_t cols, float minTemp, float maxTemp, uint16_t COLOR_PALETTE_COUNT)
{
    Serial.printf("imgM %d\n", rows*cols*3);

    for (int16_t r=0; r<rows; r++) {
        for (int16_t c=cols-1; c>=0; c--) { // draw cols in reverse order because data from MLX is reversed
            int index = r*cols + c;
            float t = src[index];
            uint8_t colorIndex = constrain(map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1), 0, COLOR_PALETTE_COUNT-1);
            Serial.write(&pColors[colorIndex].arr[1], 3);
        }
    }
}

void serial_USBstream_BiqubicInterpolated(float *frame, float *dest_2d, CRGB *pColors, float minTemp, float maxTemp, uint16_t COLOR_PALETTE_COUNT)
{
    interpolate_image(frame, 24, 32, dest_2d, USB_STREAM_INTERPOLATED_ROWS, USB_STREAM_INTERPOLATED_COLS);
    serial_print_temperatures(dest_2d, pColors, USB_STREAM_INTERPOLATED_ROWS, USB_STREAM_INTERPOLATED_COLS, minTemp, maxTemp, COLOR_PALETTE_COUNT);
}

void serial_USBStream_MinMidMax(float minTemp, float maxTemp)
{
    Serial.print("\ntxtMinT ");
    Serial.print(minTemp, 2);
    Serial.print("\ntxtMidT ");
    Serial.print((maxTemp-minTemp)/2+minTemp, 2);
    Serial.print("\ntxtMaxT ");
    Serial.print(maxTemp, 2);
    Serial.print("\n");

    // following crash the device
    //Serial.printf("txtMinT %.2f\n", minTemp);
    //Serial.printf("txtMidT %.2f\n", mid);
    //Serial.printf("txtMaxT %.2f\n", maxTemp);
}

void serial_printTempValues(float *frame)
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

void serial_printAsASCIIART(float *frame)
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