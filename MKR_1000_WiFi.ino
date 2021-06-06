//Incluimos las librerias
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_ST7789.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi101.h>

//Definimos los pines de la tft
#define TFT_CS         2
#define TFT_RST        7
#define TFT_DC         6
#define TFT_MOSI       8
#define TFT_SCLK       9

//Definimos variables para el envio de datos
#define TOKEN          "ASD"

const char* ssid = "gofionet";
const char* password = "Saul21052004/";
const char mqtt_server[] = "192.168.1.13";
const char publishTopic[] = "v1/devices/me/telemetry";

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
WiFiClient mkr1000Client;
PubSubClient client(mkr1000Client);
long lastData = 0;

int status = WL_IDLE_STATUS;
int contador_paquetes = 0;

//Variables de datos
boolean flagRx = false;
char buf[40];
String datos;
String prov = "";
String datosthingsboard [24];
int contador = 0;

void setup(void) {
  Serial.begin(9600);
  while(!Serial);
  
  //Preparamos para recibir datos por wire
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  setup_wifi();
  client.setServer(mqtt_server, 1883);


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

}

void receiveEvent(int howMany)
{
int i;
  for (i=0; i<howMany; i++)
  {
    buf[i] = Wire.read();
  }
  buf[i] = '\0';
  }
  
     // print the integer


void loop() {
datos = String(buf);
Serial.println(datos);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  for (int h = 0; h < datos.length(); h++) {
    if (datos[h] != ',') {
      prov = prov + datos[h];
    } else if (datos[h] == ',') {
      datosthingsboard [contador] = prov;
      prov = "";
      contador ++;
    }
  }

  // Mandamos los datos
  String latitud = String(datosthingsboard[0]);
  String longitud = String(datosthingsboard[1]);
  String velocidad = String(datosthingsboard[2]);
  String altitud = String(datosthingsboard[3]);
  String temperatura = String(datosthingsboard[4]);
  String presion = String(datosthingsboard[5]);
  String humedad = String(datosthingsboard[6]);
  String CO2 = String(datosthingsboard[7]);
  String gases = String(datosthingsboard[8]);
  String duv = String(datosthingsboard[9]);

  String payload = "{\"temperaturaBME\":";
  payload += temperatura;
  payload += ",\"humedadBME\":";
  payload += humedad;
  payload += ",\"LatitudGPS\":";
  payload += latitud;
  payload += ",\"LongitudGPS\":";
  payload += longitud;
  payload += ",\"velocidadGPS\":";
  payload += velocidad;
  payload += ",\"altitudGPS\":";
  payload += altitud;
  payload += ",\"CO2\":";
  payload += CO2;
  payload += ",\"GasesVolatiles\":";
  payload += gases;
  payload += ",\"DUV\":";
  payload += duv;
  payload += "}";

  char attributes[1000];
  long now = millis();

  if (now - lastData > 5000) {

    lastData = now;
    payload.toCharArray(attributes, 1000);
    client.publish(publishTopic, attributes);
    Serial.println(attributes);
  }
  
/*
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
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(0, 135);
  tft.print(contador_paquetes);
  */
  delay(3000);

}

//Funcion de recepcion de datos por wire


void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  Serial.print("Sigo funcionando...");

  while ( WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void reconnect() {

  while (!client.connected()) {

    Serial.print("Attempting MQTT connection ....");

    if (client.connect("ClientID", TOKEN, NULL)) {

      Serial.println("Connected to MQTT Broker");
    }

    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 second");
      delay(5000);
    }


  }

}
