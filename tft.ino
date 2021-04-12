#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS         2
#define TFT_RST        7
#define TFT_DC         6
#define TFT_MOSI       8
#define TFT_SCLK       9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup() {

  tft.initR(INITR_BLACKTAB);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(0.5);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.print("-Distancia del CanSat");

  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.print("-Altura del CanSat");

  tft.setCursor(0, 60);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.print("-Bateria del CanSat");

  tft.setCursor(0, 90);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.print("-Angulo localizar");

}

void loop() {
  
}

