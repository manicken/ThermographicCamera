/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_SCK       13
#define TFT_MOSI      11
#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8


Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
//Adafruit_ST7789 tft = Adafruit_ST7789(&SPI1, TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
    //while (!Serial) delay(10);
    Serial.begin(9600);
    //Serial.print("Hello! Adafruit ST77XX rotation test");

  
    tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels

    // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
    // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
    // may end up with a black screen some times, or all the time.
    //tft.setSPISpeed(20000000);

  
    Serial.println("init");

    tft.setTextWrap(false); // Allow text to run off right edge
    tft.fillScreen(ST77XX_BLACK);

    Serial.println("This is a test of the rotation capabilities of the TFT library!");
    Serial.println("Press <SEND> (or type a character) to advance");
    tft.enableDisplay(true);
}



void rotateText() {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

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
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();
  
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFillcircle(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.fillCircle(10, 30, 10, ST77XX_YELLOW);

    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateDrawcircle(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.drawCircle(10, 30, 10, ST77XX_YELLOW);
 
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();
  
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFillrect(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.fillRect(10, 20, 10, 20, ST77XX_GREEN);
 
    //while (!Serial.available());
   // Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateDrawrect(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
   // Serial.println(tft.getRotation(), DEC);

    tft.drawRect(10, 20, 10, 20, ST77XX_GREEN);
 
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFastline(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.drawFastHLine(0, 20, tft.width(), ST77XX_RED);
    tft.drawFastVLine(20, 0, tft.height(), ST77XX_BLUE);

    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateLine(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.drawLine(tft.width()/2, tft.height()/2, 0, 0, ST77XX_RED);
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotatePixel(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
   // Serial.println(tft.getRotation(), DEC);

    tft.drawPixel(10,20, ST77XX_WHITE);
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateTriangle(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.drawTriangle(20, 10, 10, 30, 30, 30, ST77XX_GREEN);
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFillTriangle(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.fillTriangle(20, 10, 10, 30, 30, 30, ST77XX_RED);
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateRoundRect(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.drawRoundRect(20, 10, 25, 15, 5, ST77XX_BLUE);
   // while (!Serial.available());
   // Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFillRoundRect(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.fillRoundRect(20, 10, 25, 15, 5, ST77XX_CYAN);
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateChar(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.drawChar(25, 15, 'A', ST77XX_WHITE, ST77XX_WHITE, 1);
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateString(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST77XX_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.setCursor(8, 25);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Adafruit Industries");
    //while (!Serial.available());
    //Serial.read();  Serial.read();  Serial.read();

    tft.setRotation(tft.getRotation()+1);
  }
}


void loop(void) {
  rotateLine();
  rotateText();
  rotatePixel();
  rotateFastline();
  rotateDrawrect();
  rotateFillrect();
  rotateDrawcircle();
  rotateFillcircle();
  rotateTriangle();
  rotateFillTriangle();
  rotateRoundRect();
  rotateFillRoundRect();
  rotateChar();
  rotateString();
  
}