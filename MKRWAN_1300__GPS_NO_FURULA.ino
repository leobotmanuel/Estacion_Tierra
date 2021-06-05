//Incluír librerías
//#include <AES.h>
//#include <Adafruit_GPS.h>
//#include "./printf.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <SD.h>
#include <LoRa.h>
#include <Wire.h>

//#define GPSSerial Serial2
//#define GPSECHO false

//AES aes ;
File Fichero;
//Adafruit_GPS GPS(&GPSSerial);

//Cosas para el cifrado
uint32_t timer = millis();
int contador_paquetes = 0;
//String cadena = "vamos a probar, 2516.335541865,525,2.22354103,adfg auid.dadgf,uyktasdf,234.23452,3452.23452562gfwef,efqwkj.";
byte *key = (unsigned char*)"0123456789010123";
//real iv = iv x2 ex: 01234567 = 0123456701234567
unsigned long long int my_iv = 36753562;
String prov = "";
String datos [24];
int contador = 0;

//void valores_GPS();

void setup ()
{
  Wire.begin();
  if (!SD.begin(3))
  {
    return;
  }

  
  Serial.begin (9600);
  while (!Serial);
  
  if (!LoRa.begin(868E6)) {
    Serial.println("Error!!!");
    while (1);
  }
  //SD
  Fichero = SD.open("cansat.csv", FILE_WRITE);
  if (Fichero) {
    Fichero.println("Tiempo(s),AcelerometroX,AcelerometroY,AcelerometroZ,GiroscopioX,GiroscopioY,GiroscopioZ,MagnetometroX,MagnetometroY,MagnetometroZ,PresionGiro(mbar),TemperaturaGiro(degC),AltitudGiro(m),LatitudGPS,LongitudGPS,velocidadGPS,altitudGPS,temperaturaBME,presionBME,humedadBME,altitudBME,CO2,GasesVolatiles,DUV");
    Fichero.close();
  }

  
  //GPS
  //Serial.println("Adafruit GPS library basic parsing test!");
  //GPS.begin(9600);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  //GPS.sendCommand(PGCMD_ANTENNA);
  //delay(1000);
  //GPSSerial.println(PMTK_Q_RELEASE);
  //printf_begin();
  //delay(1000);
}

void loop ()
{
  //Serial.println("loop");
 
  //Recibir por LoRa
  int packetSize = LoRa.parsePacket();
  String cadena = "";
  if (packetSize) {
    contador_paquetes ++;
    while (LoRa.available()) {
      cadena += (char)LoRa.read();
      //Serial.println((char)LoRa.read());
    }
    Serial.println(cadena);
  }
  
  //Pasar datos al otro MKR (el de la WiFi)
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(cadena.c_str());              // sends one byte  datos.c_str()
  Wire.endTransmission();    // stop transmitting

  //Tamaño de la cadena y cambiar tipo de la cadena (se necesita para descifrar)
  /*int plainLength = cadena.length();
  int padedLength = plainLength + N_BLOCK - plainLength % N_BLOCK;
  Serial.print("tamaño: ");
  Serial.println(plainLength);
  Serial.println();
  const char *plain_ptr = cadena.c_str();
  printf(plain_ptr);
  //Descifrar
  String cadenaDescifrada = prekey_test (plain_ptr, padedLength, plainLength) ;
*/
  
  //Guardar en SD
  Fichero = SD.open("cansat.csv", FILE_WRITE);
  Fichero.println(cadena);
  Fichero.close();/*
  
  //Obtener los valores del GPS
  //valores_GPS();


  //Separar la  cadena recibida por LoRa (Para aislar latitud y longitud)
  for (int h = 0; h < cadena.length(); h++) {
    if (cadena[h] != ',') {
      prov = prov + datos[h];
    } else if (datos[h] == ",") {
      datos [contador] = prov;
      prov = "";
      contador ++;
    }
  }*/
}
/*
  //Obtener nuestra latitud y la del satélite
  float nuestraLatitud = GPS.lat;
  float nuestraLongitud = GPS.lon;
  String latitud = datos[13];
  String longitud = datos[14];
  float suLatitud = latitud.toFloat();
  float suLongitud = longitud.toFloat();
  suLongitud += 180;
  suLatitud += 90;
  nuestraLongitud += 180;
  nuestraLatitud += 90;
  Serial.print(suLongitud);
  Serial.println("º (Longitud");
  Serial.print(suLatitud);
  Serial.println("º (Latitud)");

  //Diferencia entre las longitudes
  float distlat = (nuestraLatitud - suLatitud);
  float distlon = (nuestraLongitud - suLongitud);
  if (distlat < 0){
    distlat *= -1;
  }
  if (distlon < 0){
    distlon *= -1;
  }
  Serial.print(distlat);
  Serial.println("º");
  Serial.print(distlon);
  Serial.println("º");

  //Pasar a kilómetros
  distlat *= 111.3194;
  distlon *= 111.3194;
  
  //Calcular distancia
  float distancia = sqrt(pow(distlat, 2) + pow(distlon, 2));
  distancia *= 1000;
  Serial.print(distancia);
  Serial.println("m");
  distancia /= 1000;
  Serial.print(distlat);
  Serial.println("km");
  Serial.print(distlon);
  Serial.println("km");

  //Calcular ángulo con respecto al norte (trigonometría)
  float dif = distlat/distancia;

  float alpha = acos(dif);
  float angulo = (alpha*180)/PI;
  Serial.print(angulo);
  Serial.println("º con respecto al norte");  
  
  Serial.print("\n");
}
*/
/*
String prekey (int bits, const char *plain_ptr, int plainLength, int padedLength)
{
  Serial.print("Cadena original: ");
  Serial.println(plain_ptr);
  Serial.println();

  aes.iv_inc();
  byte iv [N_BLOCK] ;
  byte plain_p[padedLength];
  byte cipher [padedLength] ;
  byte check [padedLength] ;
  unsigned long ms = micros ();
  //aes.set_IV(my_iv);
  //aes.get_IV(iv);
  //aes.do_aes_encrypt((unsigned char*)plain_ptr, plainLength, cipher, key, bits, iv);

  //Serial.print("Cadena encriptada: ");
  //Serial.println((char *)cipher);
  //Serial.println();

  //Serial.print("Encryption took: ");
  //Serial.println(micros() - ms);
  //Serial.println();

  //ms = micros ();
  
  aes.set_IV(my_iv);
  aes.get_IV(iv);
  aes.do_aes_decrypt((unsigned char*)plain_ptr, padedLength, check, key, bits, iv);

  Serial.print("Cadena desencriptada: ");
  Serial.println((char *)check);
  Serial.println();

  Serial.print("Decryption took: ");
  Serial.println(micros() - ms);
  Serial.println();

  Serial.print("\n============================================================\n");
  return (char *)check;
  
}


String prekey_test (const char *plain_ptr, int plainLength, int padedLength)
{
  String resultado = prekey (128, plain_ptr, plainLength, padedLength) ;
  return resultado;
}
*/

/*void valores_GPS () {
  char c = GPS.read();
  if (GPSECHO)
    if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {
    Serial.print(GPS.lastNMEA());
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
  if (millis() - timer > 2000) {
    timer = millis();
    Serial.print("\nTime: ");
    if (GPS.hour < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
  }
}*/
