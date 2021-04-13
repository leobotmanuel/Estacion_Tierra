#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <SD.h>
#include <LoRa.h>

#define TFT_CS         2
#define TFT_RST        7
#define TFT_DC         6
#define TFT_MOSI       8
#define TFT_SCLK       9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

File Fichero;

int contador_paquetes = 0;

void setup(void) {

  pinMode(A4, OUTPUT);
  digitalWrite(A4, HIGH);

  tft.initR(INITR_BLACKTAB);

  tft.fillScreen(ST77XX_BLACK);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextWrap(true);
  tft.print("-Distancia del CanSat");

  tft.setCursor(0, 30);
  tft.print("-Altura del CanSat");

  tft.setCursor(0, 60);
  tft.print("-Bateria del CanSat");

  tft.setCursor(0, 90);
  tft.print("-Angulo localizar");

  tft.setCursor(0, 120);
  tft.print("-Num paquete");

   if (!SD.begin(A4))
  {
    return;
  }
  
  if (!LoRa.begin(868E6)) {
    while (1);
  }
  Fichero = SD.open("cansat.csv", FILE_WRITE);
  if (Fichero) {
    Fichero.println("Tiempo(s),AcelerometroX,AcelerometroY,AcelerometroZ,GiroscopioX,GiroscopioY,GiroscopioZ,MagnetometroX,MagnetometroY,MagnetometroZ,PresionGiro(mbar),TemperaturaGiro(degC),AltitudGiro(m),LatitudGPS,LongitudGPS,velocidadGPS,altitudGPS,temperaturaBME,presionBME,humedadBME,altitudBME,CO2,GasesVolatiles,DUV");
    Fichero.close();
  }
  
}

void loop() {
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(0, 135);
  tft.print(contador_paquetes);
  delay(5000);
  int packetSize = LoRa.parsePacket();
  String datos = "";
  if (packetSize) {
    contador_paquetes += 1;
    while (LoRa.available()) {
      datos += (char)LoRa.read();
    }

    Fichero = SD.open("cansat.csv", FILE_WRITE);
    Fichero.println(datos);
    Fichero.close();

  tft.setCursor(0, 150);
  tft.print(contador_paquetes);
  }
  
}
