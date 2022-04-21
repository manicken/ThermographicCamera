#include <Arduino.h>
#include "main.h"
#include "ThermalCamera.h"
#include "GradientPalette_Structs.h"
#include "interpolation.h"

namespace USBSerialStream
{
    uint16_t INTERPOLATED_COLS = 224;
    uint16_t INTERPOLATED_ROWS = 168;

    void print_Palette()
    {
        Serial.printf("imgGP %d\n", COLOR_PALETTE_COUNT*10*3);
        for (int ri=0;ri<10;ri++)
        {
            for (int i=0;i<COLOR_PALETTE_COUNT;i++)
                Serial.write(&Main::camColors[i].arr[1], 3);
        }
    }

    uint32_t frame = 0;
    void print_temperatures()
    {
        Serial.print("frame ");
        Serial.println(frame++);
        Serial.print("imgM ");
        Serial.println(INTERPOLATED_ROWS*INTERPOLATED_COLS*3);

        for (int16_t r=0; r<INTERPOLATED_ROWS; r++) {
            for (int16_t c=INTERPOLATED_COLS-1; c>=0; c--) { // draw cols in reverse order because data from MLX is reversed
                int index = r*INTERPOLATED_COLS + c;
                float t = Main::dest_2d[index];
                uint16_t colorIndex = constrain(map(t, ThermalCamera::minTemp, ThermalCamera::maxTemp, 0, COLOR_PALETTE_COUNT-1), 0, COLOR_PALETTE_COUNT-1);
                Serial.write(&Main::camColors[colorIndex].arr[1], 3);
            }
        }
    }
    
    void print_MinMidMax()
    {
        Serial.print("txtMinT ");
        Serial.print(ThermalCamera::minTemp, 2);
        Serial.print("\ntxtMidT ");
        Serial.print((ThermalCamera::maxTemp-ThermalCamera::minTemp)/2+ThermalCamera::minTemp, 2);
        Serial.print("\ntxtMaxT ");
        Serial.print(ThermalCamera::maxTemp, 2);
        Serial.print("\n");

        // following crash the device
        //Serial.printf("txtMinT %.2f\n", minTemp);
        //Serial.printf("txtMidT %.2f\n", mid);
        //Serial.printf("txtMaxT %.2f\n", maxTemp);
    }
    

    void execInterpolate()
    {
        //interpolate_image(ThermalCamera::frame, 24, 32, Main::dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);

        Main::gblur.calculate(ThermalCamera::frame, Main::gblurTemp);//, 32, 24);
        interpolate_image(Main::gblurTemp, 24*2, 32*2, Main::dest_2d, INTERPOLATED_ROWS/2, INTERPOLATED_COLS/2);
        interpolate_image(Main::dest_2d, INTERPOLATED_ROWS/2, INTERPOLATED_COLS/2, Main::gblurTemp, 24, 32);
        interpolate_image(Main::gblurTemp, 24, 32, Main::dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
    }

    void print_BiqubicInterpolated()
    {
        //execInterpolate();
        print_MinMidMax();
        print_temperatures();
    }

    

    void printTempValues()
    {
        for (uint8_t h=0; h<24; h++) {
            for (uint8_t w=0; w<32; w++) {
                float t = ThermalCamera::frame[h*32 + w];
                Serial.print(t, 1);
                Serial.print(", ");
            }
            Serial.println();
        }
    }

    void printAsASCIIART()
    {
        //Serial.println();
        //Serial.println();
        for (uint8_t h=0; h<24; h++) {
            for (uint8_t w=0; w<32; w++) {
                float t = ThermalCamera::frame[h*32 + w];
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
};