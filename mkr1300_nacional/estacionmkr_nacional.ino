#define MKR

// Incluimos las librerías
#include <SPI.h>
#include <LoRa.h>
#include <SD.h>
#include <Wire.h>

#ifdef MKR
#include <Adafruit_GPS.h>
#include <math.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif


// Clave para el cifrado propio
static const int clave = 43;

// Creamos el fichero para la SD y un objeto AES para usar las librerías respectivas
File Fichero;

#ifdef MKR
// Difinimos el puerto Serial del GPS
#define GPSSerial Serial1

// Creamos un objeto GPS usando el Serial al que está conectado
Adafruit_GPS GPS(&GPSSerial);

// Creamos un objeto de pantalla para la OLED
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

#define GPSECHO false

// Variables para separar latitud y longitud de los otros valores del paquete recibido
String datosGPS[2];
String distancia;
#endif

String datos[26];
String cadena_envio;
String prov;
int contador;
String cadena;


void setup() {
  // Iniciamos el puerto Serial a 115200 baudios
  Serial.begin(115200);

  // Iniciamos la OLED
#ifdef MKR
  display.begin (SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  // Iniciamos el GPS
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);
#endif

  // Iniciamos la SD
  if (!SD.begin(2))
  {
    Serial.println("Error al iniciar lector SD");
    return;
  }
  while (!Serial);

  // Iniciamos LoRa
  if (!LoRa.begin(868E6)) {
    Serial.println("Error al iniciar LoRa");
    while (1);
  }

  // Abrimos el documento .csv y escribimos la cabecera
  Fichero = SD.open("cansat.csv", FILE_WRITE);
  if (Fichero) {
    Fichero.println("clave,temperaturaBME,presionBME,humedadBME,altitudBME,DUV,CO2,GasesVolatiles,AcelerometroX,AcelerometroY,AcelerometroZ,GiroscopioX,GiroscopioY,GiroscopioZ,MagnetometroX,MagnetometroY,MagnetometroZ,presAlt,tempAlt,altAlt,bateria,IR,LatitudGPS,LongitudGPS,velocidadGPS,altitudGPS");
    Fichero.close();
  }
}

void loop() {
  // Limpiamos las variables de recepción y separación de paquetes
  contador = 0;
  prov = "";
  cadena = "";

  // Comprobar si el GPS lee nuevos datos
#ifdef MKR
  char c = GPS.read();
  if (GPSECHO)
    if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {
    //Serial.print(GPS.lastNMEA());
    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }
#endif

  // Comprobar si LoRa recibe datos
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Si LoRa recibe datos, leemos esos datos y los guardamos
    while (LoRa.available()) {
      cadena = cadena + (char)LoRa.read();
    }
    String descifrado = descifrar(cadena);
    delay(1000);

    // Separamos la cadena y la guardamos como lista de valores
    for (int h = 0; h < descifrado.length(); h++) {
      if (descifrado[h] != ',') {
        prov = prov + descifrado[h];
      } else if (descifrado[h] == ',') {
        datos[contador] = prov;
        prov = "";
        contador ++;
      }
    }

    // Si el primer datos es la clave
    if (datos[0] == "1639") {

      Serial.println(descifrado);


      Fichero = SD.open("cansat.csv", FILE_WRITE);
      Fichero.println(descifrado);
      Fichero.close();

      // Calculamos la distancia entre el CanSat y la estación
#ifdef MKR
      calcularDistancia(datos[22].toFloat(), datos[23].toFloat());
#endif
    }
  }
}

void calcularDistancia(float suLatitud, float suLongitud) {
  if (GPS.fix) {
    float distlat = ((GPS.latitude / 60) - (suLatitud / 60));
    float distlon = ((GPS.longitude / 60) - (suLongitud / 60));

    if (distlat < 0) {
      distlat *= -1;
    }
    if (distlon < 0) {
      distlon *= -1;
    }

    distlat *= 96.225;
    distlon *= 111.3194;

    float distancia = sqrt(pow(distlat, 2) + pow(distlon, 2));
    distancia *= 1000;
    //Serial.print(distancia);

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Distancia: ");
    display.print(distancia);
    display.println(" m");

    //Serial.println("m");
    distancia /= 1000;

    float dif = distlat / distancia;

    float alpha = acos(dif);
    float angulo = (alpha * 180) / PI;
    //Serial.print(angulo);
    //Serial.println("º con respecto al norte");

    display.print("Angulo: ");
    display.print(angulo);
    display.println(" deg");
    display.print("Bateria: ");
    display.print(datos[20]);
    display.println(" %");
    display.display();
    display.clearDisplay();

    //Serial.print("\n");
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
