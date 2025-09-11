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

void setup() {
  // Inicializar LCD con 16 columnas y 2 filas
  lcd.begin(16, 2);
  pinMode(fanPin, OUTPUT);
  analogWrite(fanPin, 155); // 255 = velocidad máxima (100%)

  // Mostrar mensaje inicial
  lcd.print("Hola Arduino!");
  delay(2000);
  
  // Limpiar pantalla
  lcd.clear();
}

void loop() {
  // Posicionar cursor en primera línea
  lcd.setCursor(0, 0);
  lcd.print("Linea 1: ");
  lcd.print(millis() / 1000); // Mostrar segundos
  
  // Posicionar cursor en segunda línea
  lcd.setCursor(0, 1);
  lcd.print("Linea 2: Arduino");
  
  delay(1000); // Esperar 1 segundo
}
