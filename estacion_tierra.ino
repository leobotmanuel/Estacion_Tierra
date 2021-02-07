// ARGONAUTEX2021 estacion de tierra

// Declaramos las librerias
#include <SD.h>
#include <SPI.h>
#include <PubSubClient.h>
#include "ThingsBoard.h"
#include <ESP8266WiFi.h>

// Definimos las credenciales de la WiFi
#define WIFI_AP             "gofionet"
#define WIFI_PASSWORD       "Saul21052004/"

// Definimos los parametros de ThingsBoard
#define TEMP_HUM              "GDKULyqnfmTYOsYj03SN"
#define ALTITUD               "GDKULyqnfmTYOsYj03SN"
#define ACELERACION           "GDKULyqnfmTYOsYj03SN"
#define BRUJULA               "GDKULyqnfmTYOsYj03SN"
#define GPS                   "GDKULyqnfmTYOsYj03SN"

#define THINGSBOARD_SERVER  "192.168.1.13"
#define THINGSBOARD_PORT    8080

WiFiClient espClient;
ThingsBoard tb(espClient);
int status = WL_IDLE_STATUS;

// Creamos el fichero de la SD 
File Fichero;
 
void setup()
{
 //Iniciamos el Serial y la WiFi
  Serial.begin(9600);
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
 
  InitWiFi();
 
  Serial.print(F("Iniciando SD ..."));
  if (!SD.begin(9))
  {
    Serial.println(F("Error al iniciar"));
    return;
  }
  Serial.println(F("Iniciado correctamente"));


    Fichero = SD.open("cansat.csv", FILE_WRITE);
    if(Fichero) {
      Fichero.println("Tiempo(ms), Humedad_Relativa, Temperatura, Presion_Barometrica, AcelerometroX, AcelerometroY, AcelerometroZ, GiroscopioX, GiroscopioY, GiroscopioZ, MagnetometroX, MagnetometroY, MagnetometroZ");
      Serial.println("Archivo escrito, se escribió la cabecera del csv...");
      Fichero.close();
      } else {
        Serial.println("Error al crear la cabecera");
        }
    }

 
void loop()
{
  delay(1000);

  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  if (!tb.connected()) {
   
    // Conectar a ThingsBoard
    Serial.print("Conectando a: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" con el token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TEMP_HUM)) {
      Serial.println("Error al conectar");
      return;
    }
  }

 // Mandamos los datos
  Serial.println("Mandando datos...");

  tb.sendTelemetryInt("temperatura", random(1,50));
  tb.sendTelemetryFloat("humedad", random(1,50));

  tb.loop();
}

// Funcion para iniciar la WiFi
void InitWiFi()
{
  Serial.println("Conectando ...");

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado!");
}

// Funcion para reconectar a WiFi
void reconnect() {
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("Conectado!");
  }
}
