#include <Arduino.h>

#include "interpolation.h"
#include "GradientPalettes.h"
#include "GradientPalette_Structs.h"
#include "GradientPalette_Helpers.h"
#include "Display.h"
#include "ThermalCamera.h"
#include "HTMLColors.h"

#include <Button.h>
#include <SerialCommands.h>

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

uint16_t USB_STREAM_INTERPOLATED_COLS = 224;
uint16_t USB_STREAM_INTERPOLATED_ROWS = 168;

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
        Serial.printf("imgGP %d\n", COLOR_PALETTE_COUNT*10*3);
        for (int ri=0;ri<10;ri++)
        {
            for (int i=0;i<COLOR_PALETTE_COUNT;i++)
                Serial.write(&camColors[i].arr[1], 3);
        }
    }
}

void setGradientColorMap(int16_t index)
{
    if (index >= GP_Def_Count) index = GP_Def_Count-1;
    if (index < 0) index = 0;
    uint16_t dataIndex = getDataIndex(index);
    generateGradientColorMap(camColors,  GP_Def[index].itemCount, &GP_Data[dataIndex], COLOR_PALETTE_COUNT);

    print_CurrentGradientColorPalette();
}

Button btnUp(5);
Button btnDown(6);

void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
	sender->GetSerial()->printf("log ERROR: Unrecognized command [%s]\r\n",cmd);
}

void cmd_setOutTarget(SerialCommands* sender)
{
    char* target_id_str = sender->Next();
	if (target_id_str == NULL)
	{
		sender->GetSerial()->println("log ERROR_NO_TARGET_ID");
		return;
	}
	int target_id = atoi(target_id_str);
    if (target_id >= OUTPUT_TARGET::UNKNOWN_LAST || target_id <= OUTPUT_TARGET::UNKNOWN_FIRST)
    {
		sender->GetSerial()->printf("log ERROR_UNKNOWN_TARGET_ID [%d]\r\n", target_id);
		return;
	}
    outTarget = (OUTPUT_TARGET)target_id;

    print_CurrentGradientColorPalette();
}
void cmd_setGradientColorMap(SerialCommands* sender)
{
    char* colormapIndex_str = sender->Next();
	if (colormapIndex_str == NULL)
	{
		sender->GetSerial()->printf("log ERROR_NO_COLORMAP_INDEX");
		return;
	}
	int colormapIndex = atoi(colormapIndex_str);
    if (colormapIndex < 0 || colormapIndex >= GP_Def_Count)
    {
		sender->GetSerial()->printf("log ERROR_UNKNOWN_COLORMAP_INDEX [%d]\r\n", colormapIndex);
		return;
	}
    currentColorMapIndex = colormapIndex;
    setGradientColorMap(currentColorMapIndex);
}

void cmd_setInterpolatedSize(SerialCommands* sender)
{
    char* width_str = sender->Next();
	if (width_str == NULL)
	{
		sender->GetSerial()->printf("log ERROR_NO_INTERPOLATE_SIZE");
		return;
	}
	int width = atoi(width_str);
    if (width < 32 || width > 320)
    {
		sender->GetSerial()->printf("log ERROR_INTERPOLATE_WIDTH [%d]\r\n", width);
		return;
	}
    char* height_str = sender->Next();
	if (height_str == NULL)
	{
		sender->GetSerial()->printf("log ERROR_NO_INTERPOLATE_HEIGHT");
		return;
	}
	int height = atoi(height_str);
    if (height < 24 || height > 240)
    {
		sender->GetSerial()->printf("log ERROR_INTERPOLATE_HEIGHT [%d]\r\n", height);
		return;
	}
    USB_STREAM_INTERPOLATED_COLS = width;
    USB_STREAM_INTERPOLATED_ROWS = height;
}

char serial_command_buffer_[64];
SerialCommands serialCommands(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\n", " ");

SerialCommand _cmd_setOutTarget("setOutTarget", cmd_setOutTarget);
SerialCommand _cmd_setGradientColorMap("setGradientColorMap", cmd_setGradientColorMap);
SerialCommand _cmd_setInterpolatedSize("setInterpolatedSize", cmd_setInterpolatedSize);

void setup() {
    Display_Init();

    btnUp.begin();
    btnDown.begin();
    //while (!Serial) delay(10);
    Serial.begin(115200);
    delay(100);
    serialCommands.AddCommand(&_cmd_setOutTarget);
    serialCommands.AddCommand(&_cmd_setGradientColorMap);
    serialCommands.AddCommand(&_cmd_setInterpolatedSize);
    serialCommands.SetDefaultHandler(&cmd_unrecognized);

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

void serial_print_temperatures(float *src, int16_t rows, int16_t cols)
{
    Serial.printf("imgM %d\n", rows*cols*3);

    for (int16_t r=0; r<rows; r++) {
        for (int16_t c=cols-1; c>=0; c--) { // draw cols in reverse order because data from MLX is reversed
            int index = r*cols + c;
            float t = src[index];
            uint8_t colorIndex = constrain(map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1), 0, COLOR_PALETTE_COUNT-1);
            Serial.write(&camColors[colorIndex].arr[1], 3);
        }
    }
}

void serial_USBstream_BiqubicInterpolated()
{
    interpolate_image(frame, 24, 32, dest_2d, USB_STREAM_INTERPOLATED_ROWS, USB_STREAM_INTERPOLATED_COLS);
    serial_print_temperatures(dest_2d, USB_STREAM_INTERPOLATED_ROWS, USB_STREAM_INTERPOLATED_COLS);
}

void serial_USBStream_MinMidMax()
{
    float mid = (maxTemp-minTemp)/2+minTemp;
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

int read_status = 0;
uint32_t fps = 0;
void loop() {
    serialCommands.ReadSerial();

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
        serial_USBStream_MinMidMax();
        serial_USBstream_BiqubicInterpolated();
    }
    else if (outTarget == OUTPUT_TARGET::USB_TEMP_VALUES_ASCII)
    {
        serial_printTempValues();
    }
    else if (outTarget == OUTPUT_TARGET::USB_ASCII_ART)
    {
        serial_printAsASCIIART();
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