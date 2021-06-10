#include <SPI.h>
#include <LoRa.h>
#include <SD.h>
#include <Wire.h>
#include <AES.h>

File Fichero;
AES aes;

byte *key = (unsigned char*)"0123456789010123";
unsigned long long int my_iv = 36753562;

//String cadenad = "vamos a probar, 2516.335541865,525,2.22354103,adfg auid.dadgf,uyktasdf,234.23452,3452.23452562gfwef,efqwkj.";

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
    Serial.println((char *)check);

    Fichero = SD.open("cansat.csv", FILE_WRITE);
    Fichero.println(cadena);
    Fichero.close();

    Wire.beginTransmission(4);
    Wire.write(cadena.c_str());
    Wire.endTransmission();
    Serial.println("Enviado por wire");
  }

}
