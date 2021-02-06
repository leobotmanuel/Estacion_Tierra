#include <SPI.h>
#include <SD.h>

int inicializarSD(int pinNum) {
  #define  SDPin pinNum
  Serial.begin(9600);
  Serial.println("Inicializando...");
  if (¡SD.begin(SDPin)) {
    Serial.println("Algo va mal...!");
    return 1;
  }
  Serial.println("Tarjeta SD inicializada!");
  return 0;
}

int escribirEnLaSD(char nombreDelDocumento, char contenido]) {
  File archivo;
  archivo = SD.open(nombreDelDocumento, FILE_WRITE);
  if (archivo) {
    archivo.print(contenido);
    Serial.println("Escribiendo en el archivo...");
    Serial.println("Contenido del archivo: ");
    while (archivo.available()) {
      Serial.write(archivo.read());
    }
    archivo.close();
    Serial.println("Terminado, cerrando archivo ");
    Serial.print(nombreDelDocumento);
    return 1;
  }
  else {
    Serial.println("No se pudo abrir el archivo ");
    Serial.print(nombreDelDocumento);
    archivo.close();
    return 0;
  }
}

void setup() {
  inicializarSD(10);
  escribirEnLaSD("test.txt", "Probando las funciones...");
}
