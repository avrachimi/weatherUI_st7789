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
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <utility>

#include "WifiCredentials.h"
#include "Env.h"

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
#define COLOR_CLOUD 0xa514
#define COLOR_RAIN 0x30fd
#define COLOR_BG 0xef5d
#define COLOR_SHADOW 0xc618
#define COLOR_TAG_HOT 0xd1e0
#define COLOR_TAG_COLD 0x029b

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

  initWiFi();

  updateUI();
}

void loop() {
}

void updateUI() {
  int temp;
  int humidity;
  int precipitation;
  std::string weatherText;
  getCurrentWeather(weatherText, temp, humidity, precipitation);

  tft.fillScreen(COLOR_SKY);

  tft.fillScreen(COLOR_BG);
  // TODO: Fetch weather data

  // TODO: Draw main component
  drawMainComponent(String(weatherText.c_str()), temp);

  // TODO: draw right component
  drawWeeklyWeather();

  //tft.drawFastVLine(SCREEN_WIDTH / 6 * 5, 0, SCREEN_HEIGHT / 5 * 3, ST77XX_RED);  // HELPER

  // TODO: draw bottom component
  drawWeatherDetails(humidity, precipitation);

  //tft.drawFastHLine(0, SCREEN_HEIGHT / 5 * 3, SCREEN_WIDTH, ST77XX_RED);  // HELPER
}

void drawMainComponent(String type, int temp) {
  int x = 0;
  int y = 0;

  int componentWidth = SCREEN_WIDTH / 6 * 5;
  int componentHeight = SCREEN_HEIGHT / 5 * 3;

  //tft.drawFastVLine(componentWidth / 2, 0, componentHeight, ST77XX_RED);  // HELPER

  // TODO: create fn to draw sun/cloud/rain icons
  int iconComponentWidth = componentWidth / 2;
  int iconComponentHeight = componentHeight / 5 * 4;

  //tft.drawFastHLine(0, iconComponentHeight, componentWidth, ST77XX_RED);  // HELPER

  drawBox(0, 0, 8, 4, componentWidth / 2, componentHeight, 10, COLOR_SKY);

  drawWeatherIcon(componentWidth, componentHeight, type);

  drawWeatherLocationText(componentWidth, componentHeight, iconComponentHeight);

  //tft.drawFastHLine(iconComponentWidth, iconComponentHeight, iconComponentWidth, ST77XX_RED);  // HELPER

  drawBox(componentWidth / 2, 0, 8, 4, componentWidth / 2, componentHeight, 10, COLOR_SKY);

  drawWeatherTemp(temp, iconComponentWidth, iconComponentHeight);

  drawLowHighTemps(12, 26, componentWidth, componentHeight);
}

void drawWeatherIcon(int parentWidth, int parentHeight, String type) {
  int iconComponentWidth = parentWidth / 2;
  int iconComponentHeight = parentHeight / 5 * 4;

  //tft.drawFastHLine(0, iconComponentHeight, iconComponentWidth, ST77XX_RED);  // HELPER

  int iconPaddingX = iconComponentWidth * .1;
  int iconPaddingY = iconComponentHeight * 0.1;

  int iconRadius = (iconComponentHeight - iconPaddingY) / 2;
  int iconX = iconComponentWidth / 2;
  int iconY = iconComponentHeight / 2;

  drawIcon(type, iconX, iconY, iconRadius);
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

void drawWeatherTemp(int temp, int parentWidth, int parentHeight) {
  int tempTextWidth = 70;
  int tempTextHeight = 55;

  int tempStartX = parentWidth + parentWidth / 2 - tempTextWidth / 2 - 5;
  int tempStartY = parentHeight / 2 - tempTextHeight / 2;
  tft.setCursor(tempStartX, tempStartY);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setTextSize(7);
  tft.print(temp);

  tft.fillCircle(tempStartX + tempTextWidth + 14, tempStartY + 2, 4, ST77XX_BLACK);
  tft.fillCircle(tempStartX + tempTextWidth + 14, tempStartY + 2, 2, COLOR_SKY);
}

void drawLowHighTemps(int lowTemp, int highTemp, int parentWidth, int parentHeight) {
  int iconComponentWidth = parentWidth / 2;
  int iconComponentHeight = parentHeight / 5 * 4;

  int paddingX = iconComponentWidth * .2;
  int paddingY = iconComponentHeight * .2;

  int textWidth = 24;
  int textHeight = 10;

  int lowStartX = iconComponentWidth + paddingX;
  int highStartX = parentWidth - paddingX - textWidth;

  int highLowHeight = parentHeight / 5;
  int startY = iconComponentHeight;  //+ highLowHeight / 2 - 3;

  int tagPaddingX = 12;
  int tagPaddingY = 6;

  tft.fillRoundRect(lowStartX - tagPaddingX / 2, startY - tagPaddingY / 2, textWidth + tagPaddingX / 2, textHeight + tagPaddingY / 2, 3, COLOR_TAG_COLD);
  tft.fillRoundRect(highStartX - tagPaddingX / 2, startY - tagPaddingY / 2, textWidth + tagPaddingX / 2, textHeight + tagPaddingY / 2, 3, COLOR_TAG_HOT);
  // colorgradient(lowStartX - 10, startY, parentWidth - 10 - lowStartX, textHeight);

  tft.setCursor(lowStartX, startY);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(1.5);
  tft.printf("L:%d", lowTemp);

  tft.setCursor(highStartX, startY);
  tft.printf("H:%d", highTemp);
}

void drawIcon(String type, int centerX, int centerY, int size) {
  if (type == "sun") {
    tft.fillCircle(centerX, centerY, size / 2, ST77XX_YELLOW);

    // left
    for (int i = 4; i <= 10; i += 2) {
      tft.fillCircle(centerX - size / 2 - i, centerY, 3, ST77XX_YELLOW);
    }

    // top
    for (int i = 4; i <= 10; i += 2) {
      tft.fillCircle(centerX, centerY - size / 2 - i, 3, ST77XX_YELLOW);
    }

    // right
    for (int i = 4; i <= 10; i += 2) {
      tft.fillCircle(centerX + size / 2 + i, centerY, 3, ST77XX_YELLOW);
    }

    // bottom
    for (int i = 4; i <= 10; i += 2) {
      tft.fillCircle(centerX, centerY + size / 2 + i, 3, ST77XX_YELLOW);
    }

    // top right
    int degrees = -45;
    double angle_radians = degrees * M_PI / 180;
    int x2 = centerX + (size / 2 + 4) * cos(angle_radians);
    int y2 = centerY + (size / 2 + 4) * sin(angle_radians);
    tft.fillCircle(x2, y2, 3, ST77XX_YELLOW);
    tft.fillCircle(x2 + 2, y2 - 2, 3, ST77XX_YELLOW);

    // bottom right
    degrees = 45;
    angle_radians = degrees * M_PI / 180;
    x2 = centerX + (size / 2 + 4) * cos(angle_radians);
    y2 = centerY + (size / 2 + 4) * sin(angle_radians);
    tft.fillCircle(x2, y2, 3, ST77XX_YELLOW);
    tft.fillCircle(x2 + 2, y2 + 2, 3, ST77XX_YELLOW);

    // bottom left
    degrees = 135;
    angle_radians = degrees * M_PI / 180;
    x2 = centerX + (size / 2 + 4) * cos(angle_radians);
    y2 = centerY + (size / 2 + 4) * sin(angle_radians);
    tft.fillCircle(x2, y2, 3, ST77XX_YELLOW);
    tft.fillCircle(x2 - 2, y2 + 2, 3, ST77XX_YELLOW);

    // top left
    degrees = -135;
    angle_radians = degrees * M_PI / 180;
    x2 = centerX + (size / 2 + 4) * cos(angle_radians);
    y2 = centerY + (size / 2 + 4) * sin(angle_radians);
    tft.fillCircle(x2, y2, 3, ST77XX_YELLOW);
    tft.fillCircle(x2 - 2, y2 - 2, 3, ST77XX_YELLOW);
  } else if (type == "cloud") {
    size *= 1.4;
    tft.fillRoundRect(centerX - size / 16 * 11, centerY + size / 8, size + size / 8, size / 2, size / 4, COLOR_CLOUD);

    // left
    tft.fillCircle(centerX - size / 2, centerY + size / 4, size / 4, COLOR_CLOUD);

    // top
    tft.fillCircle(centerX - size / 4, centerY + size / 16, size / 3, COLOR_CLOUD);

    // right
    tft.fillCircle(centerX + size / 4, centerY + size / 6, size / 5 * 2, COLOR_CLOUD);
  } else if (type == "rain") {
    size *= 1.2;
    tft.fillRoundRect(centerX - size / 16 * 11, centerY + size / 16, size + size / 8, size / 2, size / 4, COLOR_CLOUD);

    // left
    tft.fillCircle(centerX - size / 2, centerY + size / 4, size / 8, COLOR_CLOUD);

    // top
    tft.fillCircle(centerX - size / 4, centerY + size / 32, size / 3, COLOR_CLOUD);

    // right
    tft.fillCircle(centerX + size / 4, centerY + size / 12, size / 5 * 2, COLOR_CLOUD);

    // rain
    for (int i = 0; i <= 6; i += 2) {
      tft.fillCircle(centerX - size / 3 + i, centerY + size / 2 * 1.3 + i, 2, COLOR_RAIN);
      tft.fillCircle(centerX + i, centerY + size / 2 * 1.3 + i, 2, COLOR_RAIN);
      tft.fillCircle(centerX + size / 3 + i, centerY + size / 2 * 1.3 + i, 2, COLOR_RAIN);
    }
  } else if (type == "moon") {
    int sunX = centerX + size / 16 * 6;
    int sunY = centerY - size / 16 * 2;
    int sunSize = size * .45;
    tft.fillCircle(sunX, sunY, sunSize, COLOR_CLOUD);
  } else {  // Partly sunny
    // START SUN
    int sunX = centerX + size / 16 * 6;
    int sunY = centerY - size / 16 * 2;
    int sunSize = size * .45;
    tft.fillCircle(sunX, sunY, sunSize, ST77XX_YELLOW);

    int raySize = 1;
    // top
    for (int i = 0; i <= 5; i++) {
      tft.fillCircle(sunX, sunY - sunSize - i, raySize, ST77XX_YELLOW);
    }

    // right
    for (int i = 0; i <= 5; i++) {
      tft.fillCircle(sunX + sunSize + i, sunY, raySize, ST77XX_YELLOW);
    }

    // top right
    int degrees = -45;
    double angle_radians = degrees * M_PI / 180;
    int x2 = sunX + (sunSize + raySize) * cos(angle_radians);
    int y2 = sunY + (sunSize + raySize) * sin(angle_radians);
    tft.fillCircle(x2, y2, raySize, ST77XX_YELLOW);
    tft.fillCircle(x2 + 2, y2 - 2, raySize, ST77XX_YELLOW);

    // bottom right
    degrees = 45;
    angle_radians = degrees * M_PI / 180;
    x2 = sunX + (sunSize + raySize) * cos(angle_radians);
    y2 = sunY + (sunSize + raySize) * sin(angle_radians);
    tft.fillCircle(x2, y2, raySize, ST77XX_YELLOW);
    tft.fillCircle(x2 + 2, y2 + 2, raySize, ST77XX_YELLOW);

    // top left
    degrees = -135;
    angle_radians = degrees * M_PI / 180;
    x2 = sunX + (sunSize + raySize) * cos(angle_radians);
    y2 = sunY + (sunSize + raySize) * sin(angle_radians);
    tft.fillCircle(x2, y2, raySize, ST77XX_YELLOW);
    tft.fillCircle(x2 - 2, y2 - 2, raySize, ST77XX_YELLOW);

    // START CLOUD
    size *= 1.2;
    tft.fillRoundRect(centerX - size / 16 * 11, centerY + size / 8, size + size / 8, size / 2, size / 4, COLOR_CLOUD);

    // left
    tft.fillCircle(centerX - size / 2, centerY + size / 4, size / 4, COLOR_CLOUD);

    // top
    tft.fillCircle(centerX - size / 4, centerY + size / 16, size / 3, COLOR_CLOUD);

    // right
    tft.fillCircle(centerX + size / 4, centerY + size / 6, size / 5 * 2, COLOR_CLOUD);
    // END CLOUD
  }
}

void drawBox(int16_t startX, int16_t startY, int16_t paddingX, int16_t paddingY, int16_t width, int16_t height, int16_t radius, uint16_t color) {
  tft.fillRoundRect(startX + 1 * paddingX / 2, startY + 2 * paddingY / 2, width - paddingX, height - paddingY, radius, COLOR_SHADOW);
  tft.fillRoundRect(startX + paddingX / 2, startY + paddingY / 2, width - paddingX, height - paddingY, radius, color);
}

void drawWeeklyWeather() {
  int componentWidth = SCREEN_WIDTH;
  int componentHeight = SCREEN_HEIGHT / 5 * 2;

  int startX = 0;
  int startY = SCREEN_HEIGHT / 5 * 3;

  //String[] days = ["Mon", "Tue","Wed","Thu","Fri","Sat"];

  int width = componentWidth / 6;

  for (int i = 0; i < 6; i++) {
    int paddingX = 6;
    int paddingY = 4;

    drawBox(startX + width * i, startY, paddingX, paddingY, width, componentHeight, 10, COLOR_SKY);
  }
}

void drawWeatherDetails(int humidity, int precipitation) {
  int componentWidth = SCREEN_WIDTH / 6;
  int componentHeight = SCREEN_HEIGHT / 5 * 3;

  int startX = SCREEN_WIDTH / 6 * 5;
  int startY = 0;

  int paddingX = 6;
  int paddingY = 4;

  drawBox(startX, startY, paddingX, paddingY, componentWidth, componentHeight, 10, COLOR_SKY);

  int x = startX + 10;

  // drawSingleDetail(textStartX, startY + 6, "Hum.", "85%");
  // drawSingleDetail(textStartX, startY + 56, "Prec.", "35%");
  // drawSingleDetail(textStartX, startY + 112, "Wind", "13km/h");

  // Humidity
  int y = startY + 16;
  tft.setCursor(x, y);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setTextSize(1.5);
  tft.print("Hum.");
  tft.setCursor(x, y + 12);
  tft.setTextSize(2);
  tft.printf("%d%%", humidity);

  tft.drawFastHLine(x + 3, componentHeight / 2, componentWidth * 0.5, ST77XX_BLACK);

  // Precipitation
  y = startY + 60;
  tft.setCursor(x, y);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setTextSize(1.5);
  tft.print("Prec.");
  tft.setCursor(x, y + 12);
  tft.setTextSize(2);
  tft.print(precipitation);
  int len = std::to_string(precipitation).length();
  tft.setCursor(x + len * 13, y + 20);
  tft.setTextSize(1.5);
  tft.print("mm");
}

void drawSingleDetail(int x, int y, String label, String value) {
  tft.setCursor(x, y);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setTextSize(1.5);
  tft.print(label);
  tft.setCursor(x, y + 16);
  tft.setTextSize(2);
  tft.print(value);
}
// UTILS
void colorgradient(int x, int y, int w, int h) {
  uint8_t r, g, b;
  r = 0;   // +27
  g = 20;  // +2
  b = 27;  // -27

  for (int row = 0; row < w - 1; row++) {
    int ratio = row / w;
    tft.drawFastVLine(x + row, y + 1, h, tft.color565(27 * ratio, 20, b - 27 * ratio));
  }
}

void initWiFi() {
  tft.fillScreen(ST77XX_WHITE);
  tft.setCursor(5, 5);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setTextSize(1.5);
  tft.print("Connecting to Wifi...");

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi ..");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);

    if (tries > 3) break;
    tries++;
  }

  tft.fillScreen(ST77XX_GREEN);
  tft.setCursor(5, 5);
  tft.print("Wifi Connected!");
  tft.setCursor(5, 15);
  tft.print(WiFi.localIP());

  Serial.println(WiFi.localIP());
  delay(500);
}

void getCurrentWeather(std::string& weatherText, int& temp, int& humidity, int& precipitation) {
  // if not succesfull, values with be visibly wrong
  weatherText = "sun";
  temp = 0;
  humidity = -1;
  precipitation = -1;

  if (WiFi.status() == WL_CONNECTED) {
    String response;
    response = httpGETRequest("https://weatherapi-com.p.rapidapi.com/current.json?q=London+UK");
    Serial.println(response);
    JSONVar myObject = JSON.parse(response);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    // Serial.print("JSON object = ");
    // Serial.println(myObject);

    temp = int(myObject["current"]["temp_c"]);
    humidity = int(myObject["current"]["humidity"]);
    precipitation = int(myObject["current"]["precip_mm"]);
    int cloud = int(myObject["current"]["cloud"]);
    int isDay = int(myObject["current"]["is_day"]);

    Serial.println(humidity);
    Serial.println(precipitation);
    Serial.println(cloud);

    if (isDay == 0) {
      weatherText = "moon";
    } else if (precipitation >= 5) {
      weatherText = "rain";
    } else if (cloud > 50) {
      weatherText = "cloud";
    } else if (cloud > 20) {
      weatherText = "";  // partly cloudy
    }

    Serial.println(String(weatherText.c_str()));
    // Serial.println(temp);
    Serial.println("Curr weather updated.");

    // myObject.keys() can be used to get an array of all the keys in the object
    // JSONVar keys = myObject[0].keys();

    // for (int i = 0; i < keys.length(); i++) {
    //   JSONVar value = myObject[0][keys[i]];
    //   Serial.print(keys[i]);
    //   Serial.print(" = ");
    //   Serial.println(value);
    // }
  } else {
    Serial.println("WiFi Disconnected");
  }
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(serverName);

  http.addHeader("x-rapidapi-key", RAPID_API_KEY);

  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    // Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}