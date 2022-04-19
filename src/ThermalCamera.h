
#ifndef THERMAL_CAMERA_H
#define THERMAL_CAMERA_H

#include <Arduino.h>
#include <Adafruit_MLX90640.h>

namespace ThermalCamera
{
    #define AVERAGE_FRAME_READS 1
    #define TC_PIXELCOUNT 32*24
    Adafruit_MLX90640 mlx;
    float minTemp = 15.0f;
    float maxTemp = 20.0f;
    // having two buffers allows to both read from sensor
    // and to do interpolation at the same time
    float frameTemp[TC_PIXELCOUNT]; // this is the actual read buffer, when a completed frame has been read the contents of this is copied to frame
    float frame[TC_PIXELCOUNT]; // this contain the last completed read frame and is used by the interpolation

#if AVERAGE_FRAME_READS > 1
    float frames[AVERAGE_FRAME_READS][TC_PIXELCOUNT];

    void average()
    {
        for (int di=0;di<TC_PIXELCOUNT;di++)
        {
            float avg = 0;
            for (int i=0;i<AVERAGE_FRAME_READS;i++)
            {
                avg += frames[i][di];
            }
            frame[di] = avg / AVERAGE_FRAME_READS;
        }
    }

    int getFrame()
    {
        int res = 0;
        for (int i=0;i<AVERAGE_FRAME_READS;i++)
        {
            res = mlx.getFrame(frames[i]);
            if (res != 0) return res;
        }
        average();
        getMinMaxTemps();
        return 0;
    }
#else
    void copyFromFrameTempAndGetMinMaxTemps()
    {
        minTemp = 500.0f;
        maxTemp = -40.0f;
        for (int i=0;i<TC_PIXELCOUNT;i++) {
            frame[i] = frameTemp[i];
            float t = frame[i];
            if (t > maxTemp)
                maxTemp = t;
            if (t < minTemp)
                minTemp = t;
        }
    }

    int getFrame()
    {
        //int res = mlx.getFrame(frameTemp);
        //if (res != 0) return res; // some error has occured
        
        return mlx.getFrame(frameTemp);
    }
    
#endif

    

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

    void Init(mlx90640_mode_t mode, mlx90640_resolution_t resolution, mlx90640_refreshrate_t refreshrate)
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
        mlx.setMode(mode);
        
        mlx.setResolution(resolution);
        mlx.setRefreshRate(refreshrate);

        printMLX_current_settings();
    }
};

#endif