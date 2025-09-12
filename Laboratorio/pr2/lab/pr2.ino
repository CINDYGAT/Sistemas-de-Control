#include <LiquidCrystal.h>

// Pines LCD
const int rs = 12;
const int en = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

const int fanPin = 10;        // Pin PWM al transistor/ventilador
const int potPin = A0;       // Pin analógico para el potenciómetro

// Inicializar LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variables para el control
int valorPot = 0;           // Valor leído del potenciómetro (0-1023)
int velocidadActual = 0;    // Almacena la velocidad actual (0-255)
int porcentajeActual = 0;   // Almacena el porcentaje actual (0-100%)

void setup() {
  lcd.begin(16, 2);
  pinMode(fanPin, OUTPUT);
  pinMode(potPin, INPUT);
  
  analogWrite(fanPin, 0); // ventilador apagado al inicio
  lcd.print("Sistema Iniciado");
  delay(1000);
  lcd.clear();
}

void loop() {
  // Leer el valor del potenciómetro (0-1023)
  valorPot = analogRead(potPin);
  
  // Determinar la velocidad según los rangos
  if (valorPot >= 0 && valorPot <= 300) {
    // Rango 1: Ventilador apagado (0%)
    setVelocidad(0);
  } 
  else if (valorPot >= 301 && valorPot <= 700) {
    // Rango 2: Velocidad baja (25%)
    setVelocidad(25);
  } 
  else if (valorPot >= 701 && valorPot <= 1023) {
    // Rango 3: Velocidad media (50%)
    setVelocidad(50);
  }
  
  // Mostrar información en el LCD
  actualizarLCD();
  
  // Pequeña pausa para estabilidad
  delay(100);
}

// Función para establecer la velocidad del ventilador
void setVelocidad(int porcentaje) {
  if (porcentaje != porcentajeActual) {
    porcentajeActual = porcentaje;
    // Calcular valor PWM (0-255) basado en el porcentaje
    velocidadActual = map(porcentaje, 0, 100, 0, 255);
    
    // Aplicar el valor PWM al ventilador
    analogWrite(fanPin, velocidadActual);
  }
}

// Función para actualizar la pantalla LCD
void actualizarLCD() {
  lcd.clear();
  
  // Primera línea: Valor del potenciómetro y rango
  lcd.print("Pot:");
  lcd.print(valorPot);
  lcd.print(" R:");
  
  if (valorPot <= 300) {
    lcd.print("1");
  } else if (valorPot <= 700) {
    lcd.print("2");
  } else {
    lcd.print("3");
  }
  
  // Segunda línea: Porcentaje y valor PWM
  lcd.setCursor(0, 1);
  lcd.print("Vel:");
  lcd.print(porcentajeActual);
  lcd.print("% PWM:");
  lcd.print(velocidadActual);
}
