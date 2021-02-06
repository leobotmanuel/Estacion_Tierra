#include <SD.h>
#include <SPI.h>
 
File Fichero;
 
void setup()
{
  Serial.begin(9600);
  Serial.print(F("Iniciando SD ..."));
  if (!SD.begin(9))
  {
    Serial.println(F("Error al iniciar"));
    return;
  }
  Serial.println(F("Iniciado correctamente"));


    Fichero = SD.open("calidad.csv", FILE_WRITE);
    if(Fichero) {
      Fichero.println("Tiempo(ms), Humedad_Relativa, Temperatura, PresionBarometrica, AcelerometroX, AcelerometroY, AcelerometroZ, GiroscopioX, GiroscopioY, GiroscopioZ, MagnetometroX, MagnetometroY, MagnetometroZ");
      Serial.println("Archivo escrito, se escribió la cabecera kilométrica del csv...");
      Fichero.close();
      } else {
        Serial.println("Error al crear la cabecera kilométrica!!!");
        }
    }

 
void loop()
{
}
