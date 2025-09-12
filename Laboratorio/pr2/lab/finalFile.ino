#include <LiquidCrystal.h>

// Definir pines de conexión del LCD
const int rs = 12;
const int en = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

// Definir pines de los botones (con resistencia pulldown)
const int boton30 = 6;    // Pin digital 6 - 30% velocidad
const int boton60 = 7;    // Pin digital 7 - 60% velocidad
const int boton90 = 8;    // Pin digital 8 - 90% velocidad

const int fanPin = 9;     // Pin PWM conectado al MOSFET/transistor

// Inicializar la librería LiquidCrystal
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variables para el control de botones
int velocidadActual = 0;  // Almacena la velocidad actual (0-255)
int ultimoEstado30 = LOW;
int ultimoEstado60 = LOW;
int ultimoEstado90 = LOW;

void setup() {
  // Configurar pines de botones como entrada
  pinMode(boton30, INPUT);
  pinMode(boton60, INPUT);
  pinMode(boton90, INPUT);
  
  // Configurar pin del ventilador como salida
  pinMode(fanPin, OUTPUT);
  analogWrite(fanPin, 0); // Iniciar con ventilador apagado
  
  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.print("Ventilador OFF");
  lcd.setCursor(0, 1);
  lcd.print("Esperando comando");
}

void loop() {
  // Leer el estado actual de los botones
  int estadoActual30 = digitalRead(boton30);
  int estadoActual60 = digitalRead(boton60);
  int estadoActual90 = digitalRead(boton90);
  
  // Verificar si el botón del 30% fue presionado
  if (estadoActual30 == HIGH && ultimoEstado30 == LOW) {
    setVelocidad(10);
    ultimoEstado30 = HIGH;
  } else if (estadoActual30 == LOW) {
    ultimoEstado30 = LOW;
  }
  
  // Verificar si el botón del 60% fue presionado
  if (estadoActual60 == HIGH && ultimoEstado60 == LOW) {
    setVelocidad(65);
    ultimoEstado60 = HIGH;
  } else if (estadoActual60 == LOW) {
    ultimoEstado60 = LOW;
  }
  
  // Verificar si el botón del 90% fue presionado
  if (estadoActual90 == HIGH && ultimoEstado90 == LOW) {
    setVelocidad(100);
    ultimoEstado90 = HIGH;
  } else if (estadoActual90 == LOW) {
    ultimoEstado90 = LOW;
  }
  
  // Pequeña pausa para evitar rebotes
  delay(50);
}

// Función para establecer la velocidad del ventilador
void setVelocidad(int porcentaje) {
  // Calcular valor PWM (0-255) basado en el porcentaje
  velocidadActual = map(porcentaje, 0, 100, 0, 255);
  
  // Aplicar el valor PWM al ventilador
  analogWrite(fanPin, velocidadActual);
  
  // Actualizar display LCD
  lcd.clear();
  lcd.print("Velocidad: ");
  lcd.print(porcentaje);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("PWM: ");
  lcd.print(velocidadActual);
}
