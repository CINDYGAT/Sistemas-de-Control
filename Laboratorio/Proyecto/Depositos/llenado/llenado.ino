#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuración LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pines del sensor ultrasónico HC-SR04
const int trigPin = 7;
const int echoPin = 6;

// Pines de los LEDs
const int ledVerde = 9;
const int ledRojo = 10;

// Dimensiones del depósito (30x30x30 cm)
const float alturaDeposito = 30.0; // cm
float nivelActual = 0.0;

// Variables para el control del parpadeo
unsigned long tiempoAnterior = 0;
bool estadoLedRojo = false;
const int intervaloParpadeo = 500; // 500 ms para parpadeo

// Umbrales de nivel (en cm desde el SENSOR)
const float NIVEL_ALTO = 10.0;    // 30-20 cm de material (0-10 cm desde sensor)
const float NIVEL_MEDIO = 15.0;   // 21-15 cm de material (10-15 cm desde sensor) 
const float NIVEL_BAJO = 20.0;    // 16-10 cm de material (15-20 cm desde sensor)

void setup() {
  Serial.begin(9600);
  
  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  
  // Configurar pines
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledRojo, OUTPUT);
  
  // Apagar LEDs al inicio
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledRojo, LOW);
  
  // Mensaje inicial
  lcd.setCursor(0, 0);
  lcd.print("Sistema Nivel   ");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Medir nivel
  float distancia = medirDistancia();
  
  // Calcular nivel de material (altura del material en el depósito)
  // Si distancia = 5cm -> material está a 25cm de altura (30-5)
  nivelActual = alturaDeposito - distancia;
  
  // Controlar LEDs según el nivel
  controlarLEDs(nivelActual);
  
  // Mostrar información en LCD
  mostrarEnLCD(distancia, nivelActual);
  
  // Mostrar información en Serial
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.print(" cm | Nivel material: ");
  Serial.print(nivelActual);
  Serial.print(" cm | Estado: ");
  
  if (nivelActual >= 20) {
    Serial.println("ALTO (Verde)");
  } else if (nivelActual >= 15) {
    Serial.println("MEDIO (Rojo Parpadeo)");
  } else if (nivelActual >= 10) {
    Serial.println("BAJO (Rojo Fijo)");
  } else {
    Serial.println("VACIO");
  }
  
  delay(1000); // Medir cada segundo
}

// Función para medir distancia con sensor ultrasónico
float medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duracion = pulseIn(echoPin, HIGH);
  float distancia = duracion * 0.034 / 2;
  
  // Filtrar lecturas erróneas
  if (distancia > alturaDeposito || distancia < 0) {
    distancia = alturaDeposito; // Si hay error, asumir vacío
  }
  
  return distancia;
}

// Función para controlar los LEDs según el nivel
void controlarLEDs(float nivel) {
  if (nivel >= 17.0) {
    // Nivel ALTO: LED verde encendido, rojo apagado
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledRojo, LOW);
  } 
  else if (nivel >= 8.0) {
    // Nivel MEDIO: LED verde apagado, rojo parpadeando
    digitalWrite(ledVerde, LOW);
    
    // Control de parpadeo
    if (millis() - tiempoAnterior >= intervaloParpadeo) {
      tiempoAnterior = millis();
      estadoLedRojo = !estadoLedRojo;
      digitalWrite(ledRojo, estadoLedRojo);
    }
  } 
  else if (nivel >= 0.0) {
    // Nivel BAJO: LED verde apagado, rojo encendido fijo
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledRojo, HIGH);
  } 
  else {
    // VACÍO: Ambos LEDs apagados
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledRojo, LOW);
  }
}

// Función para mostrar información en LCD
void mostrarEnLCD(float distancia, float nivel) {
  lcd.setCursor(0, 0);
  lcd.print("Nivel: ");
  lcd.print(nivel, 1);
  lcd.print(" cm  ");
  
  lcd.setCursor(0, 1);
  
  if (nivel >= 17.0) {
    lcd.print("ALTO     Verde ON ");
  } else if (nivel >= 8.0) {
    lcd.print("MEDIO  Rojo PARP ");
  } else if (nivel >= 0.0) {
    lcd.print("BAJO    Rojo FIJO");
  } else {
    lcd.print("VACIO           ");
  }
}