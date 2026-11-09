//"Sensor DHT-22"
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float h;
int tiempoEsperaAnalisis = 3000;  // Modificado el tiempo de análisis a 3000

//"LCD"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
int timeActualizarPantalla = 2000;
unsigned long previousTime = 0;

//"Sensor Ultrasonido"
int trig = 4;
int echo = 3;
int timeInMicro;
int distanceInCm;

//"Servo"
#include <Servo.h>
Servo servo1;
Servo servo2;
int servoAngle;

// "Teclado"
#include <Keypad.h>
const byte ROWS = 4;
const byte COLUMNS = 4;
char keys[ROWS][COLUMNS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowsPins[ROWS] = {12, 11, 10, 9};
byte columnsPins[COLUMNS] = {8, 7, 6, 5};
Keypad teclado = Keypad(makeKeymap(keys), rowsPins, columnsPins, ROWS, COLUMNS);
char key;
String codigoIngresado = "";
String codigo = "123C";

// Funciones
void moveServo(Servo &servo, int start, int end);
void analyzeObject();

void setup() {
  //"Sensor DHT-22"
  dht.begin();
  //"LCD"
  lcd.init();
  lcd.backlight();
  //"Sensor Ultrasonido"
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  //"Servos"
  servo1.attach(13);
  servo2.attach(1);
  servo1.write(180);
  servo2.write(0);
}

void loop() {
  unsigned long currentTime = millis();

  h = dht.readHumidity();   //Se almacena la humedad

  digitalWrite(trig, LOW);
  delayMicroseconds(2);   //solo es para limpiar la senial anterior
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
      lcd.print("Analizando ...");

  timeInMicro = pulseIn(echo, HIGH);
  distanceInCm = timeInMicro / 29 / 2;    //equivalencia de velociadad del sonido 340m/s = 29 microsegundos/cm

  if (distanceInCm <= 5) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Analizando ...");

    if ((currentTime - previousTime) >= tiempoEsperaAnalisis) {
      analyzeObject();
      previousTime = currentTime;
    }
  } 
  else {
    if (currentTime - previousTime >= timeActualizarPantalla) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Humedad: ");
      lcd.print(h);
      lcd.setCursor(0, 1);
      lcd.print("Code: ");
      lcd.print(codigoIngresado);
      previousTime = currentTime;   //Resetear el tiempo
    }
  }

  key = teclado.getKey();
  if (key) {
    if (key != '#') {
      codigoIngresado += key;
    } 
    else {
      if (codigoIngresado == codigo) {
      moveServo(servo1, servo2, 180, 0);
      delay(5000);
      moveServo(servo1, servo2, 180, 0);
      }
      codigoIngresado = "";
    }
  }
}

// Funciones
void moveServo(Servo &servo, int start, int end) {
  if (end > start) {
    for (int angle = start; angle <= end; angle++) {
      servo.write(angle);
      delay(50);
    }
  }
  else {
    for (int angle = start; angle >= end; angle--) {
      servo.write(angle);
      delay(50);
    }
  }
}

void moveServo(Servo &servo1, Servo &servo2, int start, int end) {
  if (end > start) {
    for (int angle = start; angle <= end; angle++) {
      servo1.write(angle);
      servo2.write(angle);
      delay(50);
    }
  }
  else {
    for (int angle = start; angle >= end; angle--) {
      servo1.write(angle);
      servo2.write(angle);
      delay(50);
    }
  }
}

void analyzeObject() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humedad: ");
  lcd.print(h);
  lcd.print("%");

  if (h >= 75) {  // Residuo orgánico condicion
    lcd.setCursor(0, 1);
    lcd.print("Organico");
    moveServo(servo2, 0, 180);
    delay(5000);
    moveServo(servo2, 180, 0);
  } 
  else {    // Residuo inorgánico condicion
    lcd.setCursor(0, 1);
    lcd.print("Inorganico");
    moveServo(servo1, 180, 0);
    delay(5000);
    moveServo(servo1, 0, 180);
    delay(3000);
  }
}
