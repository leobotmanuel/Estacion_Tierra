#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <math.h>

Adafruit_BME280 bme;
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

const float R_gases = 8.31432;
const float u_aire = 28.96;
const float gravedad = 9.81;
float presion_nivel_mar = 1013.25;
float presion_cansat;
float temperatura_k;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 Esto esta furulando"));
 
  if (!bme.begin()) {
    Serial.println(F("No se ha encontrado el sensor BME280"));
    while (1) delay(10);
  }
}
 

void loop() {
  sensors_event_t temp_event, pressure_event, humidity_event;
  bme_temp->getEvent(&temp_event);
  bme_pressure->getEvent(&pressure_event);
  bme_humidity->getEvent(&humidity_event);
  
  Serial.print(F("Temperatura: "));
  Serial.print(temp_event.temperature);
  Serial.println(" *C");
 
  Serial.print(F("Humedad: "));
  Serial.print(humidity_event.relative_humidity);
  Serial.println(" %");
 
  Serial.print(F("Presion: "));
  Serial.print(pressure_event.pressure);
  Serial.println(" hPa");
  
  Serial.print(F("Altitud: "));
  Serial.print(bme.readAltitude(presion_nivel_mar));
  Serial.println(" m");

  Serial.println(" ");
  
  delay(2000);
}
