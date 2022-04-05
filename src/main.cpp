#include <Arduino.h>
#include <Adafruit_MLX90640.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "interpolation.h"
#include "GradientPalettes.h"
#include "GradientPalette_Structs.h"
#include "GradientPalette_Helpers.h"
#include "Adafruit_ST7789_Helpers.h"
#include "HTMLColors.h"

#include <Button.h>
#include <SerialCommands.h>

#define TFT_SCK       13
#define TFT_MOSI      11
#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8

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

Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

#define COLOR_PALETTE_COUNT 240

uint16_t USB_STREAM_INTERPOLATED_COLS = 224;
uint16_t USB_STREAM_INTERPOLATED_ROWS = 168;

CRGB camColors[COLOR_PALETTE_COUNT];
int16_t currentColorMapIndex = 0;

void print_CurrentGradientColorPalette()
{
    if (outTarget == OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE || outTarget == OUTPUT_TARGET::TFT_RAW_PIXELS)
    {
        // print the colormap name
        tft.fillRect(120, 230, 120, 7, ST77XX_BLACK);
        tft.setTextSize(1);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(120, 230);
        tft.print(GP_Def[currentColorMapIndex].name);

        // draw the temp-range colormap 
        for (int i=0;i<10;i++)
            drawRGBBitmap(tft, 0, 200+i, camColors, 240, 1);
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

Button btnUp(5);
Button btnDown(6);



void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
	sender->GetSerial()->printf("log ERROR: Unrecognized command [%s]\r\n",cmd);
	//sender->GetSerial()->print(cmd);
	//sender->GetSerial()->println("]");
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
    btnUp.begin();
    btnDown.begin();
    //while (!Serial) delay(10);
    Serial.begin(115200);
    delay(100);
    serialCommands.AddCommand(&_cmd_setOutTarget);
    serialCommands.AddCommand(&_cmd_setGradientColorMap);
    serialCommands.AddCommand(&_cmd_setInterpolatedSize);
    serialCommands.SetDefaultHandler(&cmd_unrecognized);

    tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels

    //setColorMode(tft, ST77XX_ColorMode::RGB565);

    tft.setTextWrap(false); // Allow text to run off right edge
    tft.fillScreen(ST77XX_BLACK);
    tft.enableDisplay(true);
    tft.setRotation(2);

    setGradientColorMap(currentColorMapIndex);
  
    

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
    
    mlx.setResolution(MLX90640_ADC_19BIT);
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



void convertToImage(float *src, uint16_t *dest, uint8_t rows, uint8_t cols)
{
    for (uint8_t r=0; r<rows; r++) {
        for (uint8_t c=0; c<cols; c++) {
            int index = r*cols + c;
            float t = src[index];
            uint8_t colorIndex = map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1);
            colorIndex = constrain(colorIndex, 0, COLOR_PALETTE_COUNT-1);
            dest[index] = camColors[colorIndex].toRGB565();
        }
    }
}

void tft_print_temperatures(float *src, int16_t rows, int16_t cols)
{
    tft.startWrite();
    tft.setAddrWindow(0, 0, cols, rows);
    for (int16_t r=0; r<rows; r++) {
        for (int16_t c=cols-1; c>=0; c--) { // draw cols in reverse order because data from MLX is reversed
            int index = r*cols + c;
            float t = src[index];
            uint8_t colorIndex = constrain(map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1), 0, COLOR_PALETTE_COUNT-1);
            uint16_t color = camColors[colorIndex].toRGB565();

            tft.SPI_WRITE16(color);
        }
    }
    
    tft.endWrite();
}

#define INTERPOLATED_COLS 224
#define INTERPOLATED_ROWS 168
float dest_2d[320 * 240];

//uint16_t dest_RGB565_map[INTERPOLATED_ROWS * INTERPOLATED_COLS];

void clearDest()
{
    for (int i=0;i<INTERPOLATED_ROWS*INTERPOLATED_COLS;i++)
        dest_2d[i] = 0.0f;
}

void tft_printBiqubicInterpolated()
{
    //t = millis();
    interpolate_image(frame, 24, 32, dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
    //Serial.print("Interpolation took "); Serial.print(millis()-t); Serial.println(" ms");
    //t = millis();
    tft_print_temperatures(dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS); // this takes almost the same time ~34mS as the following two, but would not require additional ram usage
    //Serial.print("Interpolation draw took "); Serial.print(millis()-t); Serial.println(" ms");
}

void tft_printNonInterpolated()
{
    for (uint8_t h=0; h<24; h++) {
        for (uint8_t w=0; w<32; w++) {
            float t = frame[h*32 + w];
            uint8_t colorIndex = map(t, minTemp, maxTemp, 0, COLOR_PALETTE_COUNT-1);
            colorIndex = constrain(colorIndex, 0, COLOR_PALETTE_COUNT-1);
            tft.fillRect((31-w)*7, h*7, 7, 7, camColors[colorIndex].toRGB565());
        }
    }
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

void tft_printMaxMin()
{
    // clear prev text
    tft.fillRect(0, 180, 240, 14, ST77XX_BLACK);
    //tft.fillRect(180, 180, 60, 14, ST77XX_BLACK);

    // print new text
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(0, 180);
    tft.print(minTemp);
    tft.setCursor(90, 180);
    tft.print((maxTemp-minTemp)/2+minTemp);
    tft.setCursor(180, 180);
    tft.print(maxTemp);
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
    if (outTarget == OUTPUT_TARGET::TFT_BIQUBIC_INTERPOLATE) 
    {
        tft_printMaxMin();
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
        tft_printNonInterpolated();
    }
    else 
    {

    }

    //Serial.print("Redraw took "); Serial.print(millis()-t); Serial.println(" ms");

    fps = (1000/(millis()-fps));
        tft.setTextSize(1);
        tft.fillRect(0, 230, 60, 7, ST77XX_BLACK);
        tft.setCursor(0, 230);
        tft.printf("fps:%d", fps);
}