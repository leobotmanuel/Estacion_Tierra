// Incluimos las librerías
#include <LoRa.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definimos los pines del modulo LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// Pines para la OLED
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaramos el ancho de banda del LoRa
#define BANDA 868E6

// Declaramos las variables
int angulo1 = 170;
int angulo2 = 180;

// Contador de paquetes LoRa
int counter = 0;

// Creamos un objeto de pantalla para la OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setup()
{
  // Iniciamos el Serial a 115200 baudios y el LoRa
  Serial.begin(115200);
  iniciar_Lora();

  // Reseteamos la OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Iniciamos la OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("CONTROL VUELO");
  display.display();
}

void loop()
{
  // Leemos el Joystick
  int joy = analogRead(A1);

  if (joy <= 2770)
  {
    angulo1 = map(joy, 0, 2749, 80, 170);
  } else {
    angulo1 = 170;
  }
  if (joy >= 2770)
  {
    angulo2 = map(joy, 2749, 4095, 180, 90);
  } else {
    angulo2 = 180;
  }

  String ang1 = String(angulo1);
  String ang2 = String(angulo2);

  Serial.println(joy);
  Serial.println(ang1);
  Serial.println(ang2);


  String cadena;
  cadena.concat("cva");
  cadena.concat(",");
  cadena.concat(ang1);
  cadena.concat(",");
  cadena.concat(ang2);
  cadena.concat(",");

  // Envio de paquetes
  Serial.print("Mandando paquete: ");
  Serial.println(counter);
  LoRa.beginPacket();
  LoRa.print(cadena);
  LoRa.endPacket();

  // Imprimimos los datos del control de vuelo en la OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("CONTROL DE VUELO");
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.print("Angulo1: ");
  display.setCursor(60, 20);
  display.print(ang1);
  display.setCursor(0, 30);
  display.print("Angulo2: ");
  display.setCursor(60, 30);
  display.print(ang2);
  display.setCursor(0, 50);
  display.print("Enviando: ");
  display.setCursor(60, 50);
  display.print(counter);
  display.display();

  // Esperamos 5 segundos
  counter += 1;
  delay(1000);
}

void iniciar_Lora()
{
  //Pines SPI de LoRa
  SPI.begin(SCK, MISO, MOSI, SS);

  //Pines LoRa
  LoRa.setPins(SS, RST, DIO0);

  //Iniciamos LoRa
  if (!LoRa.begin(BANDA))
  {
    Serial.println("Error al inicial LoRa!");
    while (1);
  }

  Serial.println("LoRa iniciada correctamente!");
  delay(2000);
}
