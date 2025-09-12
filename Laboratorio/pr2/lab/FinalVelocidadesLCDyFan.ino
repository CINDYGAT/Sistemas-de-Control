#include <LiquidCrystal.h>

// Pines LCD
const int rs = 12;
const int en = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

const int fanPin = 9;   // Pin PWM al transistor/ventilador

// Botones
const int button1 = 6;  // Botón 1 = 30%
const int button2 = 7;  // Botón 2 = 60%
const int button3 = 8;  // Botón 3 = 90%

int velocidadActual = 0;  // Almacena la velocidad actual (0-255)

// Inicializar LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  lcd.begin(16, 2);
  pinMode(fanPin, OUTPUT);

  // Configurar botones como entradas con resistencia pull-up interna
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);

  analogWrite(fanPin, 0); // ventilador apagado al inicio
  lcd.print("Ventilador OFF");
}

void loop() {
  if (digitalRead(button1) == HIGH) {   // Botón presionado
    setVelocidad(10);
  } 
  else if (digitalRead(button2) == HIGH) {
    setVelocidad(50);
  } 
  else if (digitalRead(button3) == HIGH) {
    setVelocidad(100);
  }
}

// Función para establecer la velocidad del ventilador
void setVelocidad(int porcentaje) {
  // Calcular valor PWM (0-255) basado en el porcentaje
  velocidadActual = map(porcentaje, 0, 100, 0, 255);
  
  // Aplicar el valor PWM al ventilador
  analogWrite(fanPin, velocidadActual);
  
  // Actualizar display LCD
  lcd.clear();
  lcd.print("Duty: ");
  lcd.print(porcentaje);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("PWM: ");
  lcd.print(velocidadActual);
}
