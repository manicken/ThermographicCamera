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

uint32_t t = 0;// used for easy timing stuff
int read_status = 0;
uint32_t fps = 0;

void fastLoop(); // forward declaration
Button btnUp(5);
Button btnDown(6);

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

    if (Main::outTarget == Main::OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE) 
        Main::outTargetCb = &Display::print_BiqubicInterpolated;
    else if (Main::outTarget == Main::OUTPUT_TARGET::USB_VIDEO_STREAM)
        Main::outTargetCb = &USBSerialStream::print_BiqubicInterpolated;
    else if (Main::outTarget == Main::OUTPUT_TARGET::USB_TEMP_VALUES_ASCII)
        Main::outTargetCb = &USBSerialStream::printTempValues;
    else if (Main::outTarget == Main::OUTPUT_TARGET::USB_ASCII_ART)
        Main::outTargetCb = &USBSerialStream::printAsASCIIART;
    else if (Main::outTarget == Main::OUTPUT_TARGET::TFT_RAW_PIXELS)
        Main::outTargetCb = &Display::printNonInterpolated;
    else 
    {

    }

    print_CurrentGradientColorPalette();
}

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

    ThermalCamera::Init();

    //loop = loop2;
    fastLoop();
}

// draft for multitasking
int execGetFrame = 0;
int getFrameDone = 0;
void getFrame_Thread()
{
    while(1){
        if (execGetFrame == 0) yield(); // if nothing to do yield to allow other tasks to run
        getFrameDone = 0;
        read_status = ThermalCamera::getFrame(); // at the lowest level @ I2C read there is a yield so other tasks can run
        getFrameDone = 1; // to signal to the 'main' thread that a frame has been read
        execGetFrame = 0;
    }
}
int execDoInterpolation = 0;
int interpolationDone = 0;
void interpolation_Thread()
{
    while(1){
        if (execDoInterpolation == 0) yield(); // if nothing to do yield to allow other tasks to run
        interpolationDone = 0;
        // this is only a placeholder draft for the actual interpolation function
        // interpolation loop start
        // interpolation task (one step)
        // yield() // to allow other threads to run
        // interpolation loop end
        interpolationDone = 1;
        execDoInterpolation = 0;
    }
}

void main_Thread() // this is the main controller
{
    // here the initial exec must be done
    execGetFrame = 1;
    yield(); // this will then allow the getframe to start
    while(1)
    {
        // when a frame read is done, a interpolation is not in progress and a interpolation is not done
        if (getFrameDone == 1 && execDoInterpolation == 0 && interpolationDone == 0) { 
            getFrameDone = 0;
            // copy frame to interpolation task buffer (768 float values = 3072 bytes)
            execDoInterpolation = 1;
            execGetFrame = 1; // start a new frame read
        }
        else if (interpolationDone == 1)
        {
            interpolationDone = 0;
            // write interpolated data to screen or usb stream
            // this write will not use any yield as it should happen fast
            // to minimize screen flicker
        }

        // button/touch read stuff here
        
        yield();
    }
}

void fastLoop() {
    while(1) {
        SerialRemoteControl::ReadSerial();

        if (btnDown.pressed() && Main::currentColorMapIndex != (GradientPalettes::Count-1)){
            Main::currentColorMapIndex++;
            //if (currentColorMapIndex == GradientPalettes::Count) currentColorMapIndex = (GradientPalettes::Count-1);
            setGradientColorMap(Main::currentColorMapIndex);
        }
        if (btnUp.pressed() && Main::currentColorMapIndex != 0){
            Main::currentColorMapIndex--;
            //if (currentColorMapIndex == 0) currentColorMapIndex = (GradientPalettes::Count-1);
            setGradientColorMap(Main::currentColorMapIndex);
        }
        //delay(100);
        fps = millis();
        //t = millis();
        
        read_status = ThermalCamera::getFrame();
        Display::printStatusMsg(read_status);
        if (read_status != 0)
            return;
        //Serial.printf("mlx.getFrame time:%d\n",(millis()-t));

        //t = millis();
        Main::outTargetCb();

        //Serial.print("Redraw took "); Serial.print(millis()-t); Serial.println(" ms");
        Display::printFps(1000.0f/(float)(millis()-fps));

        yield();
    }
}

void loop() {} // this will never be called
