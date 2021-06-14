#include <SPI.h>
#include <LoRa.h>
#include <SD.h>
#include <Wire.h>
#include <AES.h>

File Fichero;
AES aes;

byte *key = (unsigned char*)"0123456789010123";
unsigned long long int my_iv = 36753562;

String cadena_envio;

void setup() {
  Serial.begin(115200);

  Wire.setClock(100000);
  Wire.begin();

  if (!SD.begin(2))
  {
    Serial.println("Error al iniciar lector SD");
    return;
  }
  while (!Serial);

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

    Fichero = SD.open("cansat.csv", FILE_WRITE);
    Fichero.println(cadena);
    Fichero.close();

    cadena_envio = "";
    for (int h=0;h<31;h++){
      cadena_envio.concat(cadena[h]);
    }

    Wire.beginTransmission(4);
    Wire.write(cadena_envio.c_str());
    Wire.endTransmission();
    Wire.flush();
    Serial.println("Enviado por wire");
  }
}
