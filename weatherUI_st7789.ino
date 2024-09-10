/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  This example works with the 1.14" TFT breakout
    ----> https://www.adafruit.com/product/4383
  The 1.3" TFT breakout
    ----> https://www.adafruit.com/product/4313
  The 1.47" TFT breakout
    ----> https://www.adafruit.com/product/5393
  The 1.54" TFT breakout
    ----> https://www.adafruit.com/product/3787
  The 1.69" TFT breakout
    ----> https://www.adafruit.com/product/5206
  The 1.9" TFT breakout
    ----> https://www.adafruit.com/product/5394
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311


  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS 10
#define TFT_RST 4  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 8

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out
//#define TFT_BL 6

//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// COLOURS
#define COLOR_SKY 0x96ff

// SCREEN
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 170

void setup(void) {
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));

  // OR use this initializer (uncomment) if using a 1.9" 170x320 TFT:
  tft.init(170, 320);  // Init ST7789 170x320

  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  //tft.setSPISpeed(40000000);
  Serial.println(F("Initialized"));

  tft.setRotation(3);

  tft.fillScreen(COLOR_SKY);

  updateUI();
}

void loop() {
}

void updateUI() {
  // TODO: Fetch weather data

  // TODO: Draw main component
  drawMainComponent();

  // TODO: draw right component
  tft.drawFastVLine(SCREEN_WIDTH / 6 * 5, 0, SCREEN_HEIGHT / 5 * 3, ST77XX_BLACK);

  // TODO: draw bottom component
  tft.drawFastHLine(0, SCREEN_HEIGHT / 5 * 3, SCREEN_WIDTH, ST77XX_BLACK);
}

void drawMainComponent() {
  int x = 0;
  int y = 0;

  int componentWidth = SCREEN_WIDTH / 6 * 5;
  int componentHeight = SCREEN_HEIGHT / 5 * 3;

  tft.drawFastVLine(componentWidth / 2, 0, componentHeight, ST77XX_RED);  // HELPER

  // TODO: create fn to draw sun/cloud/rain icons
  int iconComponentWidth = componentWidth / 2;
  int iconComponentHeight = componentHeight / 5 * 4;

  tft.drawFastHLine(0, iconComponentHeight, iconComponentWidth, ST77XX_RED);  // HELPER

  drawWeatherIcon(componentWidth, componentHeight);

  drawWeatherLocationText(componentWidth, componentHeight, iconComponentHeight);

  tft.drawFastHLine(iconComponentWidth, iconComponentHeight, iconComponentWidth, ST77XX_RED);  // HELPER

  int tempTextWidth = 70;
  int tempTextHeight = 55;

  tft.setCursor(iconComponentWidth + iconComponentWidth / 2 - tempTextWidth / 2, iconComponentHeight / 2 - tempTextHeight / 2);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setTextSize(7);
  tft.print("17");
}

void drawWeatherIcon(int parentWidth, int parentHeight) {
  int iconComponentWidth = parentWidth / 2;
  int iconComponentHeight = parentHeight / 5 * 4;

  tft.drawFastHLine(0, iconComponentHeight, iconComponentWidth, ST77XX_RED);  // HELPER

  int iconPaddingX = iconComponentWidth * .1;
  int iconPaddingY = iconComponentHeight * 0.1;

  int iconRadius = (iconComponentHeight - iconPaddingY) / 2;
  int iconX = iconComponentWidth / 2;
  int iconY = iconComponentHeight / 2;
  tft.drawCircle(iconX, iconY, iconRadius, ST77XX_YELLOW);
}

void drawWeatherLocationText(int parentWidth, int parentHeight, int iconComponentHeight) {
  // TODO: calculate text width dyamically
  int locComponentWidth = parentWidth / 2;
  int locComponentHeight = parentHeight / 5;

  int locationTextComponentWidth = 60;
  int locationTextComponentHeight = 6;

  int locationStartX = locComponentWidth / 2 - locationTextComponentWidth / 2;
  int locationStartY = iconComponentHeight + locComponentHeight / 2 - locationTextComponentHeight / 2;

  tft.setCursor(locationStartX, locationStartY);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setTextSize(1.5);
  tft.print("London, UK");
}

void drawWeatherTemp() {
}
