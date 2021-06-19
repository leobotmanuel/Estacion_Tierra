// Incluimos las librerías
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

// Creamos el fichero para la SD y un objeto AES para usar las librerís respectivas
File Fichero;
AES aes;

// Difinimos el puerto Serial del GPS
#define GPSSerial Serial1

// Creamos un objeto GPS usando el Serial al que está conectado
Adafruit_GPS GPS(&GPSSerial);

#define GPSECHO false

// Creamos la clave para el cifrado
byte *key = (unsigned char*)"0123456789010123";
unsigned long long int my_iv = 36753562;

// Variables para separar latitud y longitud de los otros valores del paquete recibido
String datos[22];
String datosGPS[2];
String distancia;
String cadena_envio;
String prov;
int contador;
String cadena;

// Creamos un objeto de pantalla para la OLED
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

void setup() {
  // Iniciamos el puerto Serial a 115200 baudios
  Serial.begin(115200);

  // Iniciamos la OLED
  display.begin (SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  // Iniciamos la SD
  if (!SD.begin(2))
  {
    Serial.println("Error al iniciar lector SD");
    return;
  }
  while (!Serial);

  // Iniciamos el GPS
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);

  // Iniciamos LoRa
  if (!LoRa.begin(868E6)) {
    Serial.println("Error al iniciar LoRa");
    while (1);
  }

  // Abrimos el documento .csv y escribimos la cabecera
  Fichero = SD.open("cansat.csv", FILE_WRITE);
  if (Fichero) {
    Fichero.println("clave,temperaturaBME,presionBME,humedadBME,altitudBME,DUV,LatitudGPS,LongitudGPS,velocidadGPS,altitudGPS,CO2,GasesVolatiles,AcelerometroX,AcelerometroY,AcelerometroZ,GiroscopioX,GiroscopioY,GiroscopioZ,MagnetometroX,MagnetometroY,MagnetometroZ,IR");
    Fichero.close();
  }
}

void loop() {
  // Limpiamos las variables de recepción y separación de paquetes
  contador = 0;
  prov = "";
  cadena = "";

  // Comprobar si el GPS lee nuevos datos
  char c = GPS.read();
  if (GPSECHO)
    if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {
    Serial.print(GPS.lastNMEA());
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }

  // Comprobar si LoRa recibe datos
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Si LoRa recibe datos, leemos esos datos y los guardamos
    while (LoRa.available()) {
      cadena = cadena + (char)LoRa.read();
    }

    delay(1000);
    // Desciframos los datos recibidos
    byte iv [N_BLOCK];
    byte cadenadef [150];
    cadena.getBytes(cadenadef, 150);
    byte check [150];
    aes.set_IV(my_iv);
    aes.get_IV(iv);
    aes.do_aes_decrypt(cadenadef, 150, check, key, 128, iv);
    cadena = "";
    cadena = (char *)check;
    Serial.println(cadena);

    // Separamos la cadena y la guardamos como lista de valores
    for (int h = 0; h < cadena.length(); h++) {
      if (cadena[h] != ',') {
        prov = prov + cadena[h];
      } else if (cadena[h] == ',') {
        datos[contador] = prov;
        prov = "";
        contador ++;
      }
    }

    // Si el primer datos es la clave
    if (datos[0] == "1639") {
      // Guardamos en la SD
      Fichero = SD.open("cansat.csv", FILE_WRITE);
      Fichero.println(cadena);
      Fichero.close();

      // Calculamos la distancia entre el CanSat y la estación
      calcularDistancia(datos[6].toFloat(), datos[7].toFloat());
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
    Serial.print(distancia);

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Distancia:");
    display.println(distancia);
    display.display();
  
    Serial.println("m");
    distancia /= 1000;

    float dif = distlat / distancia;

    float alpha = acos(dif);
    float angulo = (alpha * 180) / PI;
    Serial.print(angulo);
    Serial.println("º con respecto al norte");
    
    display.setCursor(0, 15);
    display.print("Ángulo:");
    display.print(angulo);
    display.println(" º");
    display.display();
    display.clearDisplay();

    Serial.print("\n");
  }
}
