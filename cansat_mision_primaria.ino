#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <math.h>

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

const float R = 8.31432;
const float u = 0.0289644;
const float g = 9.81;
float presion_mar = 101.325;
float presion_cansat = 0;
float t = 0;


void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 Esto esta furulando"));
 
  if (!bme.begin()) {
    Serial.println(F("No se ha encontrado el sensor BME280"));
    while (1) delay(10);
  }
  
  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();
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

  presion_cansat = pressure_event.pressure * 100;
  t = temp_event.temperature + 273;

  float k = (R*t)/(u*g);
  float h = k * (log(presion_mar) - log(presion_cansat));
  Serial.println(h);

}
