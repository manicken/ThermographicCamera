#include <Arduino.h>
#include "main.h"
#include "interpolation.h"
#include "GradientPalettes.h"
#include "GradientPalette_Structs.h"
#include "GradientPalette_Helpers.h"
#include "Display.h"
#include "ThermalCamera.h"
#include "USBSerialStream.h"
//#include "HTMLColors.h"

#include <Button.h>
//#include <SerialCommands.h>
#include "SerialRemoteControl.h"

#define USE_THREADS

uint32_t t = 0;// used for easy timing stuff
int read_status = 0;


 // forward declarations
void fastLoop();
void setGradientColorMap(int index);
void print_CurrentGradientColorPalette();
void btnTask();
void setInterpolatedSize(int width, int height);
void setOutTarget(int target_id);
void initAndStartThreading();

Button btnUp(5);
Button btnDown(6);

void btnTask()
{
    if (btnDown.pressed() && Main::currentColorMapIndex != (GradientPalettes::Count-1)){
        Main::currentColorMapIndex++;
        //if (currentColorMapIndex == GradientPalettes::Count) currentColorMapIndex = (GradientPalettes::Count-1);
        setGradientColorMap(Main::currentColorMapIndex);
        Serial.println("btnDown");
    }
    if (btnUp.pressed() && Main::currentColorMapIndex != 0){
        Main::currentColorMapIndex--;
        //if (currentColorMapIndex == 0) currentColorMapIndex = (GradientPalettes::Count-1);
        setGradientColorMap(Main::currentColorMapIndex);
    }
}

void print_CurrentGradientColorPalette()
{
    if (Main::outTarget == Main::OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE || Main::outTarget == Main::OUTPUT_TARGET::TFT_RAW_PIXELS)
    {
        Display::printCurrentGradientColorPalette();
    }
    else if (Main::outTarget == Main::OUTPUT_TARGET::USB_VIDEO_STREAM)
    {
        USBSerialStream::print_Palette();
    }
}

void setGradientColorMap(int index)
{
    Main::currentColorMapIndex = index;
    if (index >= GradientPalettes::Count) index = GradientPalettes::Count-1;
    if (index < 0) index = 0;
    uint16_t dataIndex = getDataIndex(index);
    generateGradientColorMap(Main::camColors,  GradientPalettes::Def[index].itemCount, &GradientPalettes::Data[dataIndex], COLOR_PALETTE_COUNT);

    print_CurrentGradientColorPalette();
}

void setInterpolatedSize(int width, int height)
{
    USBSerialStream::INTERPOLATED_COLS = width;
    USBSerialStream::INTERPOLATED_ROWS = height;
}

void setOutTarget(int target_id)
{
    Main::outTarget = (Main::OUTPUT_TARGET)target_id;

    if (Main::outTarget == Main::OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE) {
        Main::CallBack_outTarget_Print = &Display::print_BiqubicInterpolated;
        Main::CallBack_outTarget_Interpolate = &Display::execInterpolate;
    }
    else if (Main::outTarget == Main::OUTPUT_TARGET::USB_VIDEO_STREAM) {
        Main::CallBack_outTarget_Print = &USBSerialStream::print_BiqubicInterpolated;
        Main::CallBack_outTarget_Interpolate = &USBSerialStream::execInterpolate;
    }
    else if (Main::outTarget == Main::OUTPUT_TARGET::USB_TEMP_VALUES_ASCII) {
        Main::CallBack_outTarget_Print = &USBSerialStream::printTempValues;
        Main::CallBack_outTarget_Interpolate = &Main::nonInterpolate;
    }
    else if (Main::outTarget == Main::OUTPUT_TARGET::USB_ASCII_ART) {
        Main::CallBack_outTarget_Print = &USBSerialStream::printAsASCIIART;
        Main::CallBack_outTarget_Interpolate = &Main::nonInterpolate;
    }
    else if (Main::outTarget == Main::OUTPUT_TARGET::TFT_RAW_PIXELS) {
        Main::CallBack_outTarget_Print = &Display::printNonInterpolated;
        Main::CallBack_outTarget_Interpolate = &Main::nonInterpolate;
    }
    else 
    {

    }

    print_CurrentGradientColorPalette();
}
uint32_t somethingTriggeredYield = 0;

#if defined(USE_THREADS)
void(*yieldCB)(void); // using this ensure proper setup of threads before using it
void yield()
{
    somethingTriggeredYield++;
    //Serial.println("yield"); // this will cause a infinite loop
    if (yieldCB != NULL)
        yieldCB();
    //
}
#else
/*void yield()
{
    // this is just a test to see if normal behaviour happends without predefined yield function
}*/
#endif

void setup() {
    Display::Init();
    btnUp.begin();
    btnDown.begin();
    //while (!Serial) delay(10);
    Serial.begin(115200);
    delay(100);
    SerialRemoteControl::Init();
    SerialRemoteControl::cmd_setGradientColorMap::SetCB("setGradientColorMap", &setGradientColorMap, GradientPalettes::Count);
    SerialRemoteControl::cmd_setInterpolatedSize::SetCB("setInterpolatedSize", &setInterpolatedSize, 32, 320, 24, 240);
    SerialRemoteControl::cmd_setOutTarget::SetCB("setOutTarget", &setOutTarget, Main::OUTPUT_TARGET::UNKNOWN_FIRST, Main::OUTPUT_TARGET::UNKNOWN_LAST);
    
    setOutTarget(Main::OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE);
    setGradientColorMap(Main::currentColorMapIndex);

    ThermalCamera::Init(MLX90640_CHESS, MLX90640_ADC_16BIT, MLX90640_16_HZ);

    //loop = loop2;
#if defined(USE_THREADS)
    initAndStartThreading();
#else
    fastLoop();
#endif
    
}


unsigned int execGetFrame = 0;
unsigned int getFrameDone = 0;
uint32_t frameReadStartTime = 0;
uint32_t frameReadTime = 0;
int getFrame_Thread_Id = 0;
void getFrame_Thread()
{
    while(1){
        if (execGetFrame == 0) threads.yield(); // if nothing to do yield to allow other tasks to run
        else
        {
            frameReadStartTime = millis();
            getFrameDone = 0;
            //Serial.println("get frame start!");
            ThermalCamera::getFrame(); // at the lowest level @ I2C read there is a yield so other tasks can run
            //Display::printStatusMsg(read_status);
            //Serial.println("get frame done!");
            getFrameDone = 1; // to signal to the 'main' thread that a frame has been read
            execGetFrame = 0;
            frameReadTime = millis() - frameReadStartTime;
            //Serial.printf("frameReadTime:%d\n", millis() - frameReadTime);
        }
    }
}
unsigned int execDoInterpolation = 0;
unsigned int interpolationDone = 0;
uint32_t interpolationStartTime = 0;
uint32_t interpolationTime = 0;
int interpolation_Thread_Id = 0;
void interpolation_Thread()
{
    while(1){
        if (execDoInterpolation == 0) threads.yield(); // if nothing to do yield to allow other tasks to run
        else
        {
            interpolationStartTime = millis();
            interpolationDone = 0;
            //Serial.println("interpolation started");
            // this is only a placeholder draft for the actual interpolation function
            // interpolation loop start
            // interpolation task (one step)
            // yield() // to allow other threads to run
            // interpolation loop end
            Main::CallBack_outTarget_Interpolate();
            interpolationDone = 1;
            execDoInterpolation = 0;
            interpolationTime = millis() - interpolationStartTime;
            //Serial.printf("interpolationTime:%d\n", millis() - interpolationTime);
        }
    }
}
uint32_t fpsStartTime = 0;
uint32_t fpsTime = 0;
uint32_t outputStartTime = 0;
uint32_t outputTime = 0;
void main_Thread() // this is the main controller
{
    fpsStartTime = millis();
    // here the initial exec must be done
    execGetFrame = 1;
    threads.yield(); // this will then allow the getframe to start
    while(1)
    {
        
        // when a frame read is done, a interpolation is not in progress and a interpolation is not done
        if (getFrameDone == 1 && execDoInterpolation == 0 && interpolationDone == 0) { 
            getFrameDone = 0;
            //Serial.println("get frame done");
            // copy frame to interpolation task buffer (768 float values = 3072 bytes)
            ThermalCamera::copyFromFrameTempAndGetMinMaxTemps();
            execDoInterpolation = 1;
            execGetFrame = 1; // start a new frame read
           
        }
        else if (interpolationDone == 1)// && getFrameDone == 1) // 
        {
            //Serial.println("interpolation done");
            interpolationDone = 0;
            outputStartTime = millis();
            Main::CallBack_outTarget_Print();
            outputTime = millis()-outputStartTime;
            // write interpolated data to screen or usb stream
            //Serial.printf("fRTi:%d, ipTi:%d, outTi:%d", frameReadTime, interpolationTime, outputTime);
            //Serial.printf(", ipSU: %d, gFStU: %d\n",threads.getStackUsed(interpolation_Thread_Id), threads.getStackUsed(getFrame_Thread_Id));
            fpsTime = millis() - fpsStartTime;
            Display::printFps(1000.0f/(float)fpsTime);
            fpsStartTime = millis();
        }

        // button/touch read stuff here
        SerialRemoteControl::ReadSerial();
        btnTask();
        
        threads.yield();
    }
}

void initAndStartThreading(){
    //threads.setSliceMicros(5);
    //threads.setSliceMillis(1);
    threads.setDefaultStackSize(4096);
    getFrame_Thread_Id = threads.addThread(getFrame_Thread);
    interpolation_Thread_Id = threads.addThread(interpolation_Thread);
#if defined(USE_THREADS)
    yieldCB = &threads.yield;
#endif
    //threads.addThread(main_Thread);
    main_Thread();
}


void fastLoop() {
    while(1) {
        SerialRemoteControl::ReadSerial();
        btnTask();
        
        //delay(100);
        fpsTime = millis();
        //t = millis();
        
        read_status = ThermalCamera::getFrame();
        Display::printStatusMsg(read_status);
        if (read_status != 0)
            return;
        //Serial.printf("mlx.getFrame time:%d\n",(millis()-t));
        ThermalCamera::copyFromFrameTempAndGetMinMaxTemps();
        //t = millis();
        Main::CallBack_outTarget_Interpolate();
        Main::CallBack_outTarget_Print();

        //Serial.print("Redraw took "); Serial.print(millis()-t); Serial.println(" ms");
        Display::printFps(1000.0f/(float)(millis()-fpsTime));

        yield();
    }
}

void loop() {} // this will never be called
