
#ifndef THERMAL_CAMERA_H
#define THERMAL_CAMERA_H

#include <Arduino.h>
#include <Adafruit_MLX90640.h>

namespace ThermalCamera
{
    Adafruit_MLX90640 mlx;
    float frame[32*24]; // buffer for full frame of temperatures

    float minTemp = 0.0f;
    float maxTemp = 0.0f;

    int getFrame()
    {
        return mlx.getFrame(frame);
    }

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

    void printMLX_current_settings()
    {
        Serial.print("log Current mode: ");
        if (mlx.getMode() == MLX90640_CHESS) {
            Serial.println("log Chess");
        } else {
            Serial.println("log Interleave");    
        }
        Serial.print("log Current resolution: ");
        mlx90640_resolution_t res = mlx.getResolution();
        switch (res) {
            case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
            case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
            case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
            case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
        }
        Serial.print("log Current frame rate: ");
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

    void Init()
    {
        Serial.println("log Adafruit MLX90640 Simple Test");
        Wire.setClock(1000000);
        if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
            Serial.println("log MLX90640 not found!");
            while (1) delay(10);
        }
        Wire.setClock(1000000);
        Serial.println("log Found Adafruit MLX90640");

        Serial.print("log Serial number: ");
        Serial.print(mlx.serialNumber[0], HEX);
        Serial.print(mlx.serialNumber[1], HEX);
        Serial.println(mlx.serialNumber[2], HEX);
        
        //mlx.setMode(MLX90640_INTERLEAVED);
        mlx.setMode(MLX90640_CHESS);
        
        mlx.setResolution(MLX90640_ADC_19BIT);
        mlx.setRefreshRate(MLX90640_16_HZ);

        printMLX_current_settings();
    }
};

#endif