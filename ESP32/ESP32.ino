#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11

const int LEDPin = 2; // pin para el LED
const int LEDarmado = 18;
const int PIRPin = 15;  // pin de entrada (for PIR sensor)
const int boton = 23; //boton de avanzar pantallas
const int boton2 = 19; // boton armado alarma
const int rele1 = 5;


int pirState = LOW; // de inicio no hay movimiento
int ledState = LOW;
int val = 0;                  // estado del pin
int avanzar = 0;
int menucursor = 0;
int botoncursor = 0;
int horaalarma;
int minutoalarma;
bool armado;
bool unidad = false;
int botonalarma = 0;
int variable_alarma = 0;
int variable_unidad = 0;
unsigned long previousMillis;
unsigned long previousMillisLED = 0;
const long interval = 1000;

RTC_DS1307 reloj;
DHT sensor(DHTPIN, DHTTYPE);

static const unsigned char PROGMEM arranque [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x06, 0x00, 0x10, 0x1F, 0xFF, 0x87, 0xFF, 0xE0, 0x30, 0x00, 0x1F, 0xFF, 0xC0, 0x00,
0x00, 0x00, 0x0F, 0x00, 0x7C, 0x1F, 0xFF, 0x8F, 0xFF, 0xC0, 0x78, 0x00, 0x7F, 0xFF, 0x80, 0x00,
0x00, 0x00, 0x1F, 0x00, 0x7C, 0x1F, 0xFF, 0x1F, 0xFF, 0x80, 0xFC, 0x00, 0xFF, 0xFF, 0x00, 0x00,
0x00, 0x00, 0x1F, 0x80, 0xFC, 0x1F, 0xFE, 0x3F, 0xFF, 0x81, 0xFE, 0x00, 0xFF, 0xFE, 0x00, 0x00,
0x00, 0x00, 0x1F, 0xC0, 0xFC, 0x1E, 0x00, 0x00, 0xF0, 0x03, 0xFE, 0x00, 0xF0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x3F, 0xC1, 0xFE, 0x1E, 0x00, 0x00, 0xF0, 0x03, 0xFF, 0x00, 0xFF, 0xFC, 0x00, 0x00,
0x00, 0x00, 0x3F, 0xE3, 0xFE, 0x1F, 0xFF, 0x80, 0xF0, 0x07, 0xCF, 0x80, 0xFF, 0xFF, 0x80, 0x00,
0x00, 0x00, 0x3F, 0xE3, 0xFE, 0x1F, 0xFF, 0x00, 0xF0, 0x0F, 0xCF, 0x80, 0x7F, 0xFF, 0xC0, 0x00,
0x00, 0x00, 0x3D, 0xF7, 0xCF, 0x1F, 0xFE, 0x00, 0xF0, 0x0F, 0x87, 0xC0, 0x1F, 0xFF, 0xC0, 0x00,
0x00, 0x00, 0x78, 0xFF, 0x8F, 0x1F, 0xFC, 0x00, 0xF0, 0x1F, 0x03, 0xE0, 0x00, 0x01, 0xC0, 0x00,
0x00, 0x00, 0x78, 0x7F, 0x8F, 0x1E, 0x00, 0x00, 0xF0, 0x3E, 0x01, 0xF0, 0x00, 0x03, 0xC0, 0x00,
0x00, 0x00, 0x78, 0x7F, 0x0F, 0x1F, 0xFF, 0x80, 0xF0, 0x3E, 0x7F, 0xF0, 0x3F, 0xFF, 0xC0, 0x00,
0x00, 0x00, 0xF8, 0x3E, 0x0F, 0x9F, 0xFF, 0x80, 0xF0, 0x7C, 0xFF, 0xF8, 0x7F, 0xFF, 0xC0, 0x00,
0x00, 0x00, 0xF0, 0x3E, 0x07, 0x9F, 0xFF, 0x00, 0xF0, 0xF9, 0xFF, 0xFC, 0x7F, 0xFF, 0x80, 0x00,
0x00, 0x00, 0xF0, 0x0C, 0x07, 0x9F, 0xFE, 0x00, 0xF0, 0xF1, 0xFF, 0xFC, 0xFF, 0xFC, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

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

static const unsigned char PROGMEM informacion [] = {
0x07, 0xE0, 0x0C, 0x30, 0x30, 0x0C, 0x20, 0x04, 0x41, 0x82, 0xC1, 0x82, 0x80, 0x01, 0x81, 0x81,
0x81, 0x81, 0x81, 0x81, 0xC1, 0x82, 0x41, 0x82, 0x20, 0x04, 0x30, 0x0C, 0x0C, 0x30, 0x07, 0xE0
};

/* Eliminado temporalmente.
static const unsigned char PROGMEM cactus [] = {
0x00, 0x08, 0x00, 0x22, 0x00, 0x1C, 0x00, 0x5D, 0x14, 0x1C, 0x14, 0x22, 0x14, 0x88, 0x14, 0x80,
0x97, 0x80, 0xD7, 0x00, 0xD4, 0x30, 0x74, 0x30, 0x14, 0x18, 0x14, 0x10, 0x00, 0x00, 0xFF, 0xFE
};

static const unsigned char PROGMEM frio [] = {
0x00, 0x00, 0x03, 0xC0, 0x01, 0x80, 0x04, 0x20, 0x77, 0xEE, 0x35, 0xAC, 0x04, 0x20, 0x1B, 0xD8,
0x1B, 0xD8, 0x04, 0x20, 0x35, 0xAC, 0x77, 0xEE, 0x04, 0x20, 0x01, 0x80, 0x03, 0xC0, 0x00, 0x00
};

*/

static const unsigned char PROGMEM clima [] = {
0x10, 0x00, 0x39, 0x60, 0x28, 0x40, 0x28, 0x60, 0x28, 0x40, 0x20, 0x00, 0x28, 0x00, 0x20, 0x3E,
0x28, 0x1E, 0x2B, 0xEF, 0x6D, 0xF0, 0xD5, 0xFE, 0xB5, 0xFF, 0xD5, 0xFF, 0x6D, 0xFF, 0x33, 0xFC
};


Adafruit_SSD1306 display(4);

void setup() 
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  pinMode(LEDPin, OUTPUT); 
  pinMode(PIRPin, INPUT);
  pinMode(boton, INPUT);
  pinMode(LEDarmado, OUTPUT);
  display.clearDisplay();
  Serial.begin(9600);
  display.setTextColor(WHITE);
  display.drawBitmap(-3, -10,  arranque, 128, 64, 1);
  display.setCursor(0, 25);
  display.println("Iniciando ...");
  armado = false;
  display.display();
  delay(5000);
  display.clearDisplay();
}

void loop()
{
  botoncursor = digitalRead(boton);
  
  if (botoncursor == HIGH){
    delay(100);
    avanzar++;
    if (avanzar > 4){
    avanzar = 0;
    }
  }
  if (armado == true){
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
 }
  
  menu();
}

void menu(){


  int horas, minutos, temp, hum, sensacion;
  DateTime now = reloj.now();
  horas = now.hour();
  minutos = now.minute();
  temp = sensor.readTemperature(unidad);
  hum = sensor.readHumidity();
  sensacion = sensor.computeHeatIndex(temp, hum, unidad);
  
  switch (avanzar){

    
    case 0: // pantalla principal
    if (millis()/1000 - previousMillis == 30){
      display.ssd1306_command(SSD1306_DISPLAYOFF);
    }
    botonalarma = digitalRead(boton2);
    if (botonalarma == HIGH){
    variable_alarma = !variable_alarma;
    delay(100);
    }
    if (variable_alarma == HIGH){
    delay(100);
    armado = true;
    unsigned long currentMillisLED = millis();
     if (currentMillisLED - previousMillisLED >= interval) {
    // save the last time you blinked the LED
    previousMillisLED = currentMillisLED;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LEDarmado, ledState);
  }
    //avisoarmaralarma();
    }
     else {
    delay(100);
    armado = false;
    digitalWrite(LEDarmado, LOW);
     }
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(39, 5);
    if (horas < 10){
      display.print("0");
    }
    display.print(String(horas));
    display.setCursor(61, 5);
    display.print(":");
    display.setCursor(71, 5);
    if (minutos < 10){
      display.print("0");
    }
    display.print(String(minutos));
    display.setTextSize(0.5);
    if (armado == true){
    display.setCursor(16, 23);
    display.println("Sistema armado");
    }
    else {
    display.setCursor(16, 23);
    display.println("Sistema desarmado");
    }
    display.display();
    return;

    case 1:// pantalla alarma
    display.ssd1306_command(SSD1306_DISPLAYON);
    display.clearDisplay();
    display.setTextSize(0.25);
    display.setCursor(0, 0);
    display.println("Alarma");
    //Funcion reloj
    display.setCursor(99, 0);
    if (horas < 10){
      display.print("0");
    }
    display.println(String(horas));
    display.setCursor(110, 0);
    display.print(":");
    display.setCursor(115, 0);
    if (minutos < 10){
      display.print("0");
    }
    //Funcion reloj final
    display.print(String(minutos));
    display.drawLine(0, 8, 128, 8, WHITE);
    display.drawBitmap(10, 10,  alarma, 16, 16, 1);
    display.setCursor(35, 10);
    display.println("Ult. deteccion");
    display.setCursor(60, 20);
    if (horaalarma < 10){
      display.print("0");
    }
    display.print(String(horaalarma));
    display.setCursor(71, 20);
    display.print(":");
    display.setCursor(76, 20);
    if (minutoalarma < 10){
      display.print("0");
    }
    display.print(String(minutoalarma));
    display.display();
    return;

    case 3: //pantalla bateria
    display.clearDisplay();
    display.setTextSize(0.5);
    display.setCursor(0, 0);
    display.println("Alimentacion");
    display.setCursor(99, 0);
    if (horas < 10){
      display.print("0");
    }
    display.println(String(horas));
    display.setCursor(110, 0);
    display.print(":");
    display.setCursor(115, 0);
    if (minutos < 10){
      display.print("0");
    }
    display.print(String(minutos));
    display.drawLine(0, 8, 128, 8, WHITE);
    display.drawBitmap(10, 10,  bateria, 16, 16, 1);
    display.setCursor(35, 11);
    display.println("Volts: " );
    display.setCursor(35, 19);
    display.println("Porcentaje: ");
    display.display();
    return;

    case 2: //pantalla corriente
    display.clearDisplay();
    display.setTextSize(0.5);
    display.setCursor(0, 0);
    display.println("Corriente");
    display.setCursor(99, 0);
    if (horas < 10){
      display.print("0");
    }
    display.println(String(horas));
    display.setCursor(110, 0);
    display.print(":");
    display.setCursor(115, 0);
    if (minutos < 10){
      display.print("0");
    }
    display.print(String(minutos));
    display.drawLine(0, 8, 128, 8, WHITE);
    display.drawBitmap(10, 11,  rayo, 16, 16, 1);
    display.setCursor(35, 11);
    display.println("Consumo: ");
    display.setCursor(35, 19);
    display.println("Coste: ");
    display.display();
    return;

    case 4: // pantalla clima

      botonalarma = digitalRead(boton2);
    if (botonalarma == HIGH){
    variable_unidad = !variable_unidad;
    delay(100);
    }
    if (variable_unidad == HIGH){
    delay(100);
    unidad = true;
    }
     else {
    delay(100);
    unidad = false;
     }
    
    display.clearDisplay();
    display.setTextSize(0.5);
    display.setCursor(0, 0);
    display.println("Clima");
    display.setCursor(99, 0);
    if (horas < 10){
      display.print("0");
    }
    display.println(String(horas));
    display.setCursor(110, 0);
    display.print(":");
    display.setCursor(115, 0);
    if (minutos < 10){
      display.print("0");
    }
    display.print(String(minutos));
    display.drawLine(0, 8, 128, 8, WHITE);
   /* if (unidad == false){ Aqui empieza
      if (sensacion < 20){
      display.drawBitmap(10, 11,  frio, 16, 16, 1);
    }
    if (sensacion > 24){
       display.drawBitmap(10, 11,  cactus, 16, 16, 1);
     }
    }
     if (unidad == true){
      if (sensacion < 68){
      display.drawBitmap(10, 11,  frio, 16, 16, 1);
    }
    if (sensacion > 75){
       display.drawBitmap(10, 11,  cactus, 16, 16, 1);
     }
    }
    /* Aqui acaba. Eliminado temporalmente
    if (sensacion < 20){
      display.drawBitmap(10, 11,  frio, 16, 16, 1);
    }
    if (sensacion > 24){
       display.drawBitmap(10, 11,  cactus, 16, 16, 1);
    }
    */
    display.drawBitmap(10, 11,  clima, 16, 16, 1);
    display.setCursor(35, 11);
    if (unidad == true){
      display.println("Temp: " + String(temp) + " F");
    }
    else {
      display.println("Temp: " + String(temp) + " C");
    }
    display.setCursor(35, 19);
    display.println("Humedad: " + String(hum) + "%");
    display.display();
    previousMillis = millis()/1000;
  }
}

void aviso(){
  display.ssd1306_command(SSD1306_DISPLAYON);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(37, 5);
  display.println("Mov. detectado");
  display.setCursor(37, 12);
  display.println("Precaucion");
  display.setCursor(37, 19);
  display.println("Avisado Blynk");
  display.drawBitmap(0, 0,  peligro, 32, 32, 1);
  display.display();
  digitalWrite(LEDarmado, LOW);
  delay(3000);
  return;
}

/*void avisoarmaralarma(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Alarma armada en:");
  display.setCursor(50, 7);
  display.println("5s");
  display.display();
  delay(5000);
  return;
}
*/
