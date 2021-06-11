#include <Wire.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <WiFi101.h>

String cadena;
bool completo;

#define TOKEN "ASD"

const char* ssid = "gofionet";
const char* password = "Saul21052004/";
const char mqtt_server[] = "192.168.1.13";
const char publishTopic[] = "v1/devices/me/telemetry";

WiFiClient mkr1000Client;
PubSubClient client(mkr1000Client);
long lastData = 0;
int status = WL_IDLE_STATUS;
unsigned long contador_paquetes = 0;

String prov = "";
String datosthingsboard [24];
int contador = 0;

void setup() {
  Wire.setClock(100000);
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if  (cadena != "" && completo == true) {
    Serial.println(cadena);

    for (int h = 0; h < cadena.length(); h++) {
      if (cadena[h] != ',') {
        prov = prov + cadena[h];
      } else if (cadena[h] == ',') {
        datosthingsboard [contador] = prov;
        prov = "";
        contador ++;
      }
    }
    String altitud = String(datosthingsboard[3]);
    String temperatura = String(datosthingsboard[0]);
    String presion = String(datosthingsboard[1]);
    String duv = String(datosthingsboard[4]);

    String payload = "{\"temperaturaBME\":";
    payload += temperatura;
    payload += ",\"presionBME\":";
    payload += presion;
    payload += ",\"altitudGPS\":";
    payload += altitud;
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
    completo = false;
    delay(3000);
  }
}

void receiveEvent(int howMany)
{
  cadena = "";
  while (Wire.available())
  {
    char c = Wire.read();
    cadena.concat(c);
    delay(10);
  }
  completo = true;
  delay(500);
}

void setup_wifi() {

  delay(1000);
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
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 second");
      delay(5000);
    }
  }
}
