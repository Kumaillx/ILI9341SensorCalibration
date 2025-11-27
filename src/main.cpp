#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// ==== SPI PINS (HSPI) ====
#define HSPI_SCK   14
#define HSPI_MISO  12
#define HSPI_MOSI  13

// ==== TFT PINS ====
#define TFT_CS     15
#define TFT_DC     2
#define TFT_RST    4

// ==== TOUCH PINS ====
#define TOUCH_CS   5
#define TOUCH_IRQ  27   // optional

// TFT object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Touch object
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ); // IRQ optional

void setup() {
  Serial.begin(115200);
  delay(300);

  // Start HSPI on custom pins
  SPI.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, -1);

  // Initialize TFT on HSPI
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  // Initialize touch controller on same HSPI bus
  touch.begin();
  touch.setRotation(1);

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("TFT + Touch Ready!");
}

void loop() {
  // Check for touch
  if (touch.touched()) {
    TS_Point p = touch.getPoint();

    // Convert raw touch to screen coordinates
    int x = map(p.x, 3912, 364, 0, tft.width());
    int y = map(p.y, 3840, 227, 0, tft.height());

    Serial.printf("Touch → X: %d  |  Y: %d\n", x, y);

    // Visual feedback: draw small circle
    tft.fillCircle(x, y, 4, ILI9341_RED);
  }
}
