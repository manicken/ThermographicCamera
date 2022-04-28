
#ifndef THERMAL_CAMERA_H
#define THERMAL_CAMERA_H

#include <Arduino.h>
#include <Adafruit_MLX90640.h>

namespace ThermalCamera
{
    #define AVERAGE_FRAME_MAX_READS 32
    #define AVERAGE_FRAME_DEFAULT 8
    #define TC_PIXELCOUNT 32*24
    Adafruit_MLX90640 mlx;
    float minTemp = 15.0f;
    float maxTemp = 20.0f;
    // having two buffers allows to both read from sensor
    // and to do interpolation at the same time
    
    float frame[TC_PIXELCOUNT]; // this is the actual 'public' frame data that should be used

#if AVERAGE_FRAME_MAX_READS > 1
    float *avg_frames; //[AVERAGE_FRAME_READS][TC_PIXELCOUNT];
    int avg_cfi = 0; // average current frame index
    int avg_cc = AVERAGE_FRAME_DEFAULT; // average current count, this makes it possible to change this dynamically

    void updateFrameAndGetMinMaxTemps()
    {
        minTemp = 500.0f;
        maxTemp = -40.0f;
        for (int pi=0;pi<TC_PIXELCOUNT;pi++) {
            float avg = 0;
            for (int fi=0;fi<avg_cc;fi++)
            {
                avg += avg_frames[fi*TC_PIXELCOUNT+pi];
            }
            float t = avg / avg_cc;
            
            frame[pi] = t;
            if (t > maxTemp)
                maxTemp = t;
            if (t < minTemp)
                minTemp = t;
        }
    }

    int readFrame()
    {
        int res = mlx.getFrame(&avg_frames[avg_cfi*TC_PIXELCOUNT]);
        if (res != 0) return res;
        
        if (avg_cfi < (avg_cc-1)) avg_cfi++;
        else avg_cfi = 0;

        return 0;
    }
#else
    float frameTemp[TC_PIXELCOUNT]; // this is the actual read buffer, when a completed frame has been read the contents of this is copied to frame

    void updateFrameAndGetMinMaxTemps()
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

    int readFrame()
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
#if AVERAGE_FRAME_MAX_READS > 1
        avg_frames = (float*)malloc(AVERAGE_FRAME_MAX_READS*TC_PIXELCOUNT*4);
        for (int i=0;i<(TC_PIXELCOUNT*AVERAGE_FRAME_MAX_READS);i++) {
            avg_frames[i] = 0.0f;
        }
#endif
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

    void pixelate(float *src, float *dst, int srcW, int srcH, int scale)
    {
        for (int y=0;y<srcH;y++)
        {
            for (int x=0;x<srcW;x++)
            {
                for (int sy=0;sy<scale;sy++)
                {
                    for (int sx=0;sx<scale;sx++)
                    {
                        dst[y*scale*scale*srcW + x*scale + sy*scale*srcW + sx] = src[y*srcW+x];
                    }
                }
            }
        }
    }
};

#endif