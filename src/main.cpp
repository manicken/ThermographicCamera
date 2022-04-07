#include <Arduino.h>

#include "interpolation.h"
#include "GradientPalettes.h"
#include "GradientPalette_Structs.h"
#include "GradientPalette_Helpers.h"
#include "Display.h"
#include "ThermalCamera.h"
#include "USBSerialStream.h"
#include "HTMLColors.h"

#include <Button.h>
//#include <SerialCommands.h>
#include "SerialRemoteControl.h"


uint32_t t = 0;// used for easy timing stuff

typedef enum {
    UNKNOWN_FIRST = -1,
    TFT_RAW_PIXELS = 0,
    TFT_BIQUBIC_INTERPOLATE = 1,
    USB_VIDEO_STREAM = 2,
    USB_ASCII_ART = 3,
    USB_TEMP_VALUES_ASCII = 4,
    UNKNOWN_LAST = 5
}OUTPUT_TARGET;

OUTPUT_TARGET outTarget = OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE; 

#define COLOR_PALETTE_COUNT 240

CRGB camColors[COLOR_PALETTE_COUNT];
int16_t currentColorMapIndex = 0;

#define INTERPOLATED_COLS 224
#define INTERPOLATED_ROWS 168
float dest_2d[320 * 240];

void print_CurrentGradientColorPalette()
{
    if (outTarget == OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE || outTarget == OUTPUT_TARGET::TFT_RAW_PIXELS)
    {
        Display_printCurrentGradientColorPalette(camColors, GP_Def[currentColorMapIndex].name);
    }
    else if (outTarget == OUTPUT_TARGET::USB_VIDEO_STREAM)
    {
        USBSerial_PrintPalette(camColors, COLOR_PALETTE_COUNT);
    }
}

void setGradientColorMap(int index)
{
    currentColorMapIndex = index;
    if (index >= GP_Def_Count) index = GP_Def_Count-1;
    if (index < 0) index = 0;
    uint16_t dataIndex = getDataIndex(index);
    generateGradientColorMap(camColors,  GP_Def[index].itemCount, &GP_Data[dataIndex], COLOR_PALETTE_COUNT);

    print_CurrentGradientColorPalette();
}

void setInterpolatedSize(int width, int height)
{
    USB_STREAM_INTERPOLATED_COLS = width;
    USB_STREAM_INTERPOLATED_ROWS = height;
}

void setOutTarget(int target_id)
{
    outTarget = (OUTPUT_TARGET)target_id;
    print_CurrentGradientColorPalette();
}

Button btnUp(5);
Button btnDown(6);

void setup() {
    Display_Init();
    btnUp.begin();
    btnDown.begin();
    //while (!Serial) delay(10);
    Serial.begin(115200);
    delay(100);
    SerialRemoteControl::Init();
    SerialRemoteControl::cmd_setGradientColorMap::SetCB("setGradientColorMap", setGradientColorMap, GP_Def_Count);
    SerialRemoteControl::cmd_setInterpolatedSize::SetCB("setInterpolatedSize", setInterpolatedSize, 32, 320, 24, 240);
    SerialRemoteControl::cmd_setOutTarget::SetCB("setOutTarget", setOutTarget, OUTPUT_TARGET::UNKNOWN_FIRST, OUTPUT_TARGET::UNKNOWN_LAST);
    
    setGradientColorMap(currentColorMapIndex);

    ThermalCamera_Init();
}

void tft_printBiqubicInterpolated()
{
    //t = millis();
    interpolate_image(frame, 24, 32, dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
    //Serial.print("Interpolation took "); Serial.print(millis()-t); Serial.println(" ms");
    //t = millis();
    Display_print_temperatures(dest_2d, camColors, INTERPOLATED_ROWS, INTERPOLATED_COLS, minTemp, maxTemp, COLOR_PALETTE_COUNT); // this takes almost the same time ~34mS as the following two, but would not require additional ram usage
    //Serial.print("Interpolation draw took "); Serial.print(millis()-t); Serial.println(" ms");
}

int read_status = 0;
uint32_t fps = 0;
void loop() {
    SerialRemoteControl::ReadSerial();

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
    
    read_status = ThermalCamera_getFrame();
    Display_printStatusMsg(read_status);
    if (read_status != 0)
        return;
    //Serial.printf("mlx.getFrame time:%d\n",(millis()-t));
    
    getMinMaxTemps();
    
    //t = millis();
    if (outTarget == OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE) 
    {
        Display_printMaxMin(minTemp, maxTemp);
        tft_printBiqubicInterpolated();
    }
    else if (outTarget == OUTPUT_TARGET::USB_VIDEO_STREAM)
    {
        serial_USBStream_MinMidMax(minTemp, maxTemp);
        serial_USBstream_BiqubicInterpolated(frame, dest_2d, camColors, minTemp, maxTemp, COLOR_PALETTE_COUNT);
    }
    else if (outTarget == OUTPUT_TARGET::USB_TEMP_VALUES_ASCII)
    {
        serial_printTempValues(frame);
    }
    else if (outTarget == OUTPUT_TARGET::USB_ASCII_ART)
    {
        serial_printAsASCIIART(frame);
    }
    else if (outTarget == OUTPUT_TARGET::TFT_RAW_PIXELS)
    {
        Display_printNonInterpolated(frame, camColors, minTemp, maxTemp, COLOR_PALETTE_COUNT);
    }
    else 
    {

    }

    //Serial.print("Redraw took "); Serial.print(millis()-t); Serial.println(" ms");
    Display_printFps((1000/(millis()-fps)));
}