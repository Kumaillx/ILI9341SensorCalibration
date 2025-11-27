// RandomDisplay_ILI9341_HSPI.ino
// Works on ESP32 using HSPI (SCK=14, MISO=12, MOSI=13 by default)

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// --- PIN CONFIG (change if needed) ---
#define HSPI_SCK   14  // HSPI SCLK
#define HSPI_MISO  12  // HSPI MISO (not required for writes)
#define HSPI_MOSI  13  // HSPI MOSI
#define TFT_CS     15  // Chip Select (CS)
#define TFT_DC     2   // Data/Command
#define TFT_RST    4   // Reset (set -1 if tied to 3.3V)

// --- create TFT object (use default SPI object which we initialize with custom pins) ---
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

unsigned long lastSwitch = 0;
int mode = 0;
const int NUM_MODES = 5;

void setup() {
  Serial.begin(115200);
  delay(100);

  // initialize HSPI with chosen pins
  // SPI.begin(SCK, MISO, MOSI, SS)
  SPI.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, -1);

  // init tft
  tft.begin();

  // rotate to landscape for better visuals (0-3)
  tft.setRotation(1);

  // seed random with some noise - analogRead on an unconnected pin is common
  randomSeed(analogRead(33));

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setCursor(2, 2);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Random Display: HSPI");
  delay(600);
  lastSwitch = millis();
}

uint16_t randomColor() {
  // 16-bit color: 5 bits red, 6 green, 5 blue
  uint8_t r = random(0, 32);
  uint8_t g = random(0, 64);
  uint8_t b = random(0, 32);
  return (r << 11) | (g << 5) | b;
}

void drawRandomPixels(int count) {
  int w = tft.width(), h = tft.height();
  while (count--) {
    int x = random(0, w);
    int y = random(0, h);
    tft.drawPixel(x, y, randomColor());
  }
}

void drawRandomRects(int count) {
  int w = tft.width(), h = tft.height();
  while (count--) {
    int x = random(0, w);
    int y = random(0, h);
    int rw = random(1, w / 4);
    int rh = random(1, h / 4);
    if (random(0, 2))
      tft.fillRect(x, y, rw, rh, randomColor());
    else
      tft.drawRect(x, y, rw, rh, randomColor());
  }
}

void drawRandomLines(int count) {
  int w = tft.width(), h = tft.height();
  while (count--) {
    int x1 = random(0, w), y1 = random(0, h);
    int x2 = random(0, w), y2 = random(0, h);
    tft.drawLine(x1, y1, x2, y2, randomColor());
  }
}

void drawNoiseFull() {
  // fill the whole screen with random pixels — decent stress test
  int w = tft.width(), h = tft.height();
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      tft.drawPixel(x, y, randomColor());
    }
  }
}

void drawRandomCircles(int count) {
  int w = tft.width(), h = tft.height();
  while (count--) {
    int x = random(0, w), y = random(0, h);
    int r = random(1, min(w, h) / 6);
    if (random(0, 2))
      tft.fillCircle(x, y, r, randomColor());
    else
      tft.drawCircle(x, y, r, randomColor());
  }
}

void showModeTitle(int m) {
  tft.fillRect(0, 0, 120, 14, ILI9341_BLACK); // clear small area
  tft.setCursor(2, 2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  switch (m) {
    case 0: tft.print("Mode 0: Random pixels"); break;
    case 1: tft.print("Mode 1: Random rects"); break;
    case 2: tft.print("Mode 2: Random lines"); break;
    case 3: tft.print("Mode 3: Full noise"); break;
    case 4: tft.print("Mode 4: Circles & text"); break;
  }
}

void loop() {
  // simple automatic mode switch every 6 seconds
  if (millis() - lastSwitch > 6000) {
    mode = (mode + 1) % NUM_MODES;
    lastSwitch = millis();
    tft.fillScreen(ILI9341_BLACK);
    showModeTitle(mode);
  }

  switch (mode) {
    case 0:
      drawRandomPixels(4000); // many pixels per loop
      break;
    case 1:
      drawRandomRects(50);
      break;
    case 2:
      drawRandomLines(200);
      break;
    case 3:
      drawNoiseFull(); // slow but looks cool
      break;
    case 4:
      drawRandomCircles(80);
      // overlay some random text
      tft.setTextSize(random(1, 3));
      tft.setCursor(random(0, tft.width()-40), random(16, tft.height()-10));
      tft.setTextColor(randomColor());
      tft.print("ESP32");
      break;
  }

  // tiny pause so the display updates smoothly
  delay(100);
}
