#include <SPI.h>
#include <LoRa.h>
#include <SD.h>
#include <Wire.h>
#include <AES.h>
#include <Adafruit_GPS.h>
#include <math.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

File Fichero;
AES aes;

#define GPSSerial Serial1

Adafruit_GPS GPS(&GPSSerial);

#define GPSECHO false

byte *key = (unsigned char*)"0123456789010123";
unsigned long long int my_iv = 36753562;
String datos[22];
String datosGPS[2];
String distancia;
String cadena_envio;
String prov;
int contador;

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

void setup() {
  Serial.begin(115200);

  Wire.setClock(100000);
  Wire.begin();
  
  display.begin (SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  if (!SD.begin(2))
  {
    Serial.println("Error al iniciar lector SD");
    return;
  }
  while (!Serial);

  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);

  if (!LoRa.begin(868E6)) {
    Serial.println("Error al iniciar LoRa");
    while (1);
  }

  Fichero = SD.open("cansat.csv", FILE_WRITE);
  if (Fichero) {
    Fichero.println("temperaturaBME,presionBME,humedadBME,altitudBME,DUV,LatitudGPS,LongitudGPS,velocidadGPS,altitudGPS,CO2,GasesVolatiles,AcelerometroX,AcelerometroY,AcelerometroZ,GiroscopioX,GiroscopioY,GiroscopioZ,MagnetometroX,MagnetometroY,MagnetometroZ,IR");
    Fichero.close();
  }
}

void loop() {
  contador = 0;
  prov = "";
  char c = GPS.read();
  if (GPSECHO)
    if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {
    Serial.print(GPS.lastNMEA());
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }

  int packetSize = LoRa.parsePacket();
  String cadena;
  if (packetSize) {
    while (LoRa.available()) {
      cadena = cadena + (char)LoRa.read();
    }

    delay(1000);
    byte iv [N_BLOCK];
    //byte plain_p[padedLength];
    byte cadenadef [150];
    cadena.getBytes(cadenadef, 150);
    byte check [150];
    aes.set_IV(my_iv);
    aes.get_IV(iv);
    aes.do_aes_decrypt(cadenadef, 150, check, key, 128, iv);
    cadena = "";
    cadena = (char *)check;
    Serial.println(cadena);

    for (int h = 0; h < cadena.length(); h++) {
      if (cadena[h] != ',') {
        prov = prov + cadena[h];
      } else if (cadena[h] == ',') {
        datos[contador] = prov;
        prov = "";
        contador ++;
      }
    }

    if (datos[0] == "ArgonSat") {

      Fichero = SD.open("cansat.csv", FILE_WRITE);
      Fichero.println(cadena);
      Fichero.close();

      calcularDistancia(datos[6].toFloat(), datos[7].toFloat());

      cadena_envio = "";
      for (int h = 0; h < 31; h++) {
        cadena_envio.concat(cadena[h]);
      }

      //Wire.beginTransmission(4);
      //Wire.write(cadena_envio.c_str());
      //Wire.endTransmission();
      //Wire.flush();
      Serial.println("Enviado por wire");
    }
  }
}

void calcularDistancia(float suLatitud, float suLongitud) {
  if (GPS.fix) {
    float distlat = ((GPS.latitude / 60) - (suLatitud / 60));
    float distlon = ((GPS.longitude / 60) - (suLongitud / 60));

    if (distlat < 0) {
      distlat *= -1;
    }
    if (distlon < 0) {
      distlon *= -1;
    }

    distlat *= 96.225;
    distlon *= 111.3194;

    float distancia = sqrt(pow(distlat, 2) + pow(distlon, 2));
    distancia *= 1000;
    distancia -= 12;
    Serial.print(distancia);

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Distancia:");
    display.println(distancia);
    display.display();
    delay(100);
    display.clearDisplay();
  
    Serial.println("m");
    distancia /= 1000;

    float dif = distlat / distancia;

    float alpha = acos(dif);
    float angulo = (alpha * 180) / PI;
    Serial.print(angulo);
    Serial.println("º con respecto al norte");

    Serial.print("\n");
  }
}
