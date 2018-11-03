#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "RTClib.h"

const int LEDPin = 13;        // pin para el LED
const int PIRPin = 2;  // pin de entrada (for PIR sensor)
const int boton = 4;

int pirState = LOW;           // de inicio no hay movimiento
int val = 0;                  // estado del pin
int avanzar = 0;
int menucursor = 0;
int botoncursor = 0;
int horaalarma;
int minutoalarma;

RTC_DS1307 reloj;

static const unsigned char PROGMEM alarma [] = {
0x00, 0x78, 0x00, 0xFE, 0x01, 0xFE, 0x03, 0xFF, 0x07, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xBF, 0xFE,
0xCF, 0xFC, 0x47, 0xF8, 0x27, 0xF0, 0x0F, 0xE0, 0x1F, 0xE0, 0x0E, 0xE0, 0x00, 0x60, 0x00, 0xC0
};

static const unsigned char PROGMEM bateria [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xF8, 0xC3, 0x0F, 0x86, 0x0F, 0x8F, 0xC9,
0x8F, 0x89, 0x83, 0x0F, 0xC3, 0x0F, 0xFF, 0xF8, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM rayo [] = {
0x01, 0xF0, 0x01, 0xF0, 0x03, 0xE0, 0x03, 0xC0, 0x07, 0xC0, 0x07, 0xF0, 0x0F, 0xF0, 0x0F, 0xE0,
0x0F, 0xC0, 0x03, 0x80, 0x03, 0x00, 0x07, 0x00, 0x06, 0x00, 0x0C, 0x00, 0x08, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM peligro [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0xC0, 0x00,
0x00, 0x07, 0xE0, 0x00, 0x00, 0x0E, 0x70, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x1C, 0x38, 0x00,
0x00, 0x18, 0x18, 0x00, 0x00, 0x30, 0x0C, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x61, 0x86, 0x00,
0x00, 0xE1, 0x87, 0x00, 0x00, 0xC1, 0x83, 0x00, 0x01, 0x81, 0x81, 0x80, 0x03, 0x81, 0x81, 0xC0,
0x03, 0x01, 0x80, 0xC0, 0x07, 0x01, 0x80, 0xE0, 0x06, 0x01, 0x80, 0x60, 0x0C, 0x01, 0x80, 0x30,
0x1C, 0x01, 0x80, 0x38, 0x18, 0x00, 0x00, 0x18, 0x38, 0x01, 0x80, 0x1C, 0x30, 0x01, 0x80, 0x0C,
0x60, 0x01, 0x80, 0x06, 0xE0, 0x00, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x07,
0x7F, 0xFF, 0xFF, 0xFE, 0x3F, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


Adafruit_SSD1306 display(4);

void setup() 
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  pinMode(LEDPin, OUTPUT); 
  pinMode(PIRPin, INPUT);
  pinMode(boton, INPUT);
  display.clearDisplay();
  Serial.begin(9600);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Iniciando PIR ...................................");
  display.display();
  delay(5000);
  display.clearDisplay();
}

void loop()
{
  botoncursor = digitalRead(boton);
  if (botoncursor == HIGH){
    delay(125);
    avanzar++;
    if (avanzar > 4){
    avanzar = 0;
    }
  }
  val = digitalRead(PIRPin);
  if (val == HIGH)   //si está activado
  { 
    int horas, minutos;
    DateTime now = reloj.now();
    horas = now.hour();
    minutos = now.minute();
    horaalarma = horas;
    minutoalarma = minutos;
    digitalWrite(LEDPin, HIGH);  //LED ON
    aviso();
    if (pirState == LOW)  //si previamente estaba apagado
    {
      Serial.println("Sensor activado");
      pirState = HIGH;
    }
  } 
  else   //si esta desactivado
  {
    digitalWrite(LEDPin, LOW); // LED OFF
    if (pirState == HIGH)  //si previamente estaba encendido
    {
      Serial.println("Sensor parado");
      pirState = LOW;
    }
  }
  menu();
}

void menu(){


  int horas, minutos;
  DateTime now = reloj.now();
  horas = now.hour();
  minutos = now.minute();
  
  switch (avanzar){
    
    case 0: // pantalla principal
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(39, 5);
    display.println(String(horas) + ":" + String(minutos));
    display.setTextSize(0.5);
    display.setCursor(17, 23);
    display.println("Sistema desarmado");
    display.display();
    return;

    case 1:// pantalla alarma
    display.clearDisplay();
    display.setTextSize(0.25);
    display.setCursor(0, 0);
    display.println("Alarma");
    display.setCursor(95, 0);
    display.println(String(horas) + ":" + String(minutos));
    display.drawLine(0, 8, 128, 8, WHITE);
    display.drawBitmap(10, 10,  alarma, 16, 16, 1);
    display.setCursor(35, 10);
    display.println("Ult. deteccion:");
    display.setCursor(60, 20);
    display.println(String(horaalarma) + ":" + String(minutoalarma));
    display.display();
    return;

    case 2: //pantalla bateria
    display.clearDisplay();
    display.setTextSize(0.5);
    display.setCursor(0, 0);
    display.println("Alimentacion");
    display.setCursor(95, 0);
    display.println(String(horas) + ":" + String(minutos));
    display.drawLine(0, 8, 128, 8, WHITE);
    display.drawBitmap(10, 10,  bateria, 16, 16, 1);
    display.setCursor(35, 11);
    display.println("Volts: " + String(avanzar) + "V");
    display.setCursor(35, 19);
    display.println("Porcentaje: " + String(avanzar) + "%");
    display.display();
    return;

    case 3: //pantalla corriente
    display.clearDisplay();
    display.setTextSize(0.5);
    display.setCursor(0, 0);
    display.println("Corriente");
    display.setCursor(95, 0);
    display.println(String(horas) + ":" + String(minutos));
    display.drawLine(0, 8, 128, 8, WHITE);
    display.drawBitmap(10, 11,  rayo, 16, 16, 1);
    display.setCursor(35, 11);
    display.println("Consumo: " + String(avanzar) + "A");
    display.setCursor(35, 19);
    display.println("Coste: " + String(avanzar) + " Euro");
    display.display();
    return;

    case 4: // pantalla informacion
    display.clearDisplay();
    display.setTextSize(0.5);
    display.setCursor(0, 0);
    display.println("Informacion");
    display.setCursor(95, 0);
    display.println(String(horas) + ":" + String(minutos));
    display.drawLine(0, 8, 128, 8, WHITE);
    //display.drawBitmap(10, 11,  rayo, 16, 16, 1);
    display.setCursor(35, 11);
    display.println("Informacion varia");
    //display.setCursor(35, 19);
    //display.println("Coste: " + String(avanzar) + " Euro");
    display.display();
  }
}

void aviso(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(37, 5);
  display.println("Mov. detectado");
  display.setCursor(37, 12);
  display.println("Precaucion");
  display.setCursor(37, 19);
  display.println("Aviso: OK!");
  display.drawBitmap(0, 0,  peligro, 32, 32, 1);
  display.display();
  delay(6000);
  return;
}

