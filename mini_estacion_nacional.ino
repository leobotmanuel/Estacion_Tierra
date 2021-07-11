// Incluimos las librerías
#include <SPI.h>
#include <LoRa.h>
#include <SD.h>
#include <Wire.h>

// Clave para el cifrado propio
static const int clave = 43;

String datos[26];
String cadena_envio;
String prov;
int contador;
String cadena;


void setup() {
  // Iniciamos el puerto Serial a 115200 baudios
  Serial.begin(115200);

  while (!Serial);

  // Iniciamos LoRa
  if (!LoRa.begin(868E6)) {
    Serial.println("Error al iniciar LoRa");
    while (1);
  }
}

void loop() {
  // Limpiamos las variables de recepción y separación de paquetes
  contador = 0;
  prov = "";
  cadena = "";

  // Comprobar si LoRa recibe datos
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Si LoRa recibe datos, leemos esos datos y los guardamos
    while (LoRa.available()) {
      cadena = cadena + (char)LoRa.read();
    }
      String descifrado = descifrar(cadena);
      if (descifrado[0] == '1'){
        if (descifrado[1] == '6'){
          Serial.println(descifrado);
          }
        }
  }
}


String descifrar(String entrada) {
  String salida;
  for (int i = 0; i < entrada.length(); i++) {
    byte entrada_cifrada = (byte)entrada[i] - clave;
    salida += (char)entrada_cifrada;
  }
  return salida;
}
