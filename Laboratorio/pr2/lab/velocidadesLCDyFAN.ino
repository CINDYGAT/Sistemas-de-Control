#include <LiquidCrystal.h>

// Definir pines de conexión (ajusta según tu conexión)
const int rs = 12;    // Pin RS
const int en = 11;    // Pin Enable
const int d4 = 5;     // Pin D4
const int d5 = 4;     // Pin D5
const int d6 = 3;     // Pin D6
const int d7 = 2;     // Pin D7

const int fanPin = 9; // Pin PWM conectado a la base del BD139 (con resistencia)

// Inicializar la librería LiquidCrystal
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variables para el control del tiempo
unsigned long startTime;
int stage = 0; // etapa actual

void setup() {
  // Inicializar LCD con 16 columnas y 2 filas
  lcd.begin(16, 2);
  pinMode(fanPin, OUTPUT);
  analogWrite(fanPin, 0); // 255 = velocidad máxima (100%)
  lcd.print("Ventilador OFF");

  startTime = millis(); // Guardamos el tiempo inicial
  // Mostrar mensaje inicial
  //lcd.print("Hola Arduino!");
  delay(2000);
  
  // Limpiar pantalla
  lcd.clear();
}

void loop() {
  unsigned long elapsed = (millis() - startTime) / 1000; // segundos transcurridos

  // Cambiar de etapa cada 8 segundos
  if (elapsed >= 8 && stage == 0) {
    analogWrite(fanPin, 64); // 25% de 255 ≈ 64
    lcd.clear();
    lcd.print("Velocidad: 25%");
    stage = 1;
  }
  else if (elapsed >= 16 && stage == 1) {
    analogWrite(fanPin, 128); // 50%
    lcd.clear();
    lcd.print("Velocidad: 50%");
    stage = 2;
  }
  else if (elapsed >= 24 && stage == 2) {
    analogWrite(fanPin, 191); // 75%
    lcd.clear();
    lcd.print("Velocidad: 75%");
    stage = 3;
  }
  else if (elapsed >= 32 && stage == 3) {
    analogWrite(fanPin, 255); // 100%
    lcd.clear();
    lcd.print("Velocidad: 100%");
    stage = 4;
  }

  // Mostrar contador de tiempo en la segunda línea
  lcd.setCursor(0, 1);
  lcd.print("Tiempo: ");
  lcd.print(elapsed);
  lcd.print("s   "); // espacios extra para borrar restos

  // Posicionar cursor en primera línea
  //lcd.setCursor(0, 0);
  //lcd.print("Linea 1: ");
  //lcd.print(millis() / 1000); // Mostrar segundos
  
  // Posicionar cursor en segunda línea
  //lcd.setCursor(0, 1);
  //lcd.print("Linea 2: Arduino");
  
  //delay(1000); // Esperar 1 segundo
}
