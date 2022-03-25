#include <Arduino.h>
#include <Adafruit_MLX90640.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_SCK       13
#define TFT_MOSI      11
#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8


Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

// uncomment *one* of the below
//#define PRINT_TEMPERATURES
//#define PRINT_ASCIIART
#define PRINT_TFT

void setup() {
  //while (!Serial) delay(10);
  Serial.begin(115200);
  delay(100);

  tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
  tft.setTextWrap(false); // Allow text to run off right edge
  tft.fillScreen(ST77XX_BLACK);
  tft.enableDisplay(true);
  tft.setRotation(2);
  tft.setCursor(0, 30);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(1);
    tft.println("Hello World!");
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.println("Hello World!");
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(3);
    tft.println("Hello World!");
    tft.setTextColor(ST77XX_BLUE);
    tft.setTextSize(4);
    tft.print(1234.567);
    

  Serial.println("Adafruit MLX90640 Simple Test");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found Adafruit MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);
  
  //mlx.setMode(MLX90640_INTERLEAVED);
  mlx.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");    
  }

  mlx.setResolution(MLX90640_ADC_16BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  mlx.setRefreshRate(MLX90640_16_HZ);
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

float a, b, c, High, Low, Delta, TempC, vdd, Ta, tr, emissivity;
int MINTEMP = 22;
int MAXTEMP = 28;

unsigned int GetColor(float TempC) {
    byte red=0,green=0,blue=0;
  red = constrain(255.0 / (c - b) * TempC - ((b * 255.0) / (c - b)), 0, 255);
  if (TempC < a) {
    green = constrain(255.0 / (a - MINTEMP) * TempC - (255.0 * MINTEMP) / (a - MINTEMP), 0, 255);
  }
  else if (TempC > c) {
    green = constrain(255.0 / (c - MAXTEMP) * TempC - (MAXTEMP * 255.0) / (c - MAXTEMP), 0, 255);
  }
  else {
    green = 255;
  }
  if (TempC > (MAXTEMP )) {
    blue = constrain((TempC - MAXTEMP) * 55.0, 0, 255);
  }
  else {
    blue = constrain(255.0 / (a - b) * TempC - ((a * 255.0) / (a - b)), 0, 255);
  }

  return tft.color565(red, green, blue);
}

void loop() {
  //delay(100);
  unsigned long myTime = millis();
  int read_status = mlx.getFrame(frame);
  if (read_status != 0) {
    Serial.printf("Failed:%d", read_status);
    return;
  }
  Serial.printf("mlx.getFrame time:%d\n",(millis()-myTime));
  Serial.println();
  Serial.println();
  for (uint8_t h=0; h<24; h++) {
    for (uint8_t w=0; w<32; w++) {
      float t = frame[h*32 + w];
#ifdef PRINT_TFT
    
#endif
// keep the following for debugging purposes
#ifdef PRINT_TEMPERATURES
      Serial.print(t, 1);
      Serial.print(", ");
#endif
#ifdef PRINT_ASCIIART
      //char c = '&';
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
#endif
    }
#if defined(PRINT_ASCIIART) || defined(PRINT_TEMPERATURES)
    Serial.println();
#endif
  }
}