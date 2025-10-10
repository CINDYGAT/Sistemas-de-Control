// ============ SISTEMA DE 3 DEPÓSITOS CON SENSORES ULTRASÓNICOS ============

//===================== Pines del sensor ultrasónico HC-SR04 =================
// ------------ Ultrasonico 1 (Agua) --------------------
const int trigPin1 = 7;
const int echoPin1 = 6;
// ------------ Ultrasonico 2 (Cal) --------------------
const int trigPin2 = 5;
const int echoPin2 = 4;
// ------------ Ultrasonico 3 (Arena) -------------------
const int trigPin3 = 2;
const int echoPin3 = 3;

// Pines de los LEDs
//---- Sensor 1 (Agua) -----
const int ledVerde1 = 9;
const int ledAnaranjado1 = A1;
const int ledRojo1 = 8;
//---- Sensor 2 (Cal) -----
const int ledVerde2 = 12;
const int ledAnaranjado2 = A2;
const int ledRojo2 = 13;
//---- Sensor 3 (Arena) -----
const int ledVerde3 = 10;
const int ledAnaranjado3 = A3;
const int ledRojo3 = 11;

// Dimensiones del depósito (30x30x30 cm)
const float alturaDeposito = 30.0; // cm

// Variables para niveles actuales
float nivel1 = 0.0;
float nivel2 = 0.0;
float nivel3 = 0.0;

// ============================ SETUP =================================
void setup() {
  Serial.begin(9600);

  // Configurar pines de sensores ultrasónicos
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  
  // Configurar pines de LEDs
  pinMode(ledVerde1, OUTPUT);
  pinMode(ledAnaranjado1, OUTPUT);
  pinMode(ledRojo1, OUTPUT);

  pinMode(ledVerde2, OUTPUT);
  pinMode(ledAnaranjado2, OUTPUT);
  pinMode(ledRojo2, OUTPUT);

  pinMode(ledVerde3, OUTPUT);
  pinMode(ledAnaranjado3, OUTPUT);
  pinMode(ledRojo3, OUTPUT);
  
  apagarTodosLosLEDs();
  
  Serial.println("Sistema de 3 depósitos iniciado");
  Serial.println("=================================");
}

// ============================= LOOP =================================
void loop() {
  // Medir niveles
  nivel1 = alturaDeposito - medirDistancia(trigPin1, echoPin1);
  nivel2 = alturaDeposito - medirDistancia(trigPin2, echoPin2);
  nivel3 = alturaDeposito - medirDistancia(trigPin3, echoPin3);
  
  // Controlar LEDs
  controlarLEDsDeposito(nivel1, ledVerde1, ledAnaranjado1, ledRojo1);
  controlarLEDsDeposito(nivel2, ledVerde2, ledAnaranjado2, ledRojo2);
  controlarLEDsDeposito(nivel3, ledVerde3, ledAnaranjado3, ledRojo3);
  
  // Mostrar información
  mostrarInfoSerial();
  
  delay(1000);
}

// ========================== FUNCIONES ================================

// Medir distancia con ultrasonido
float medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duracion = pulseIn(echoPin, HIGH);
  float distancia = duracion * 0.034 / 2;
  
  if (distancia > alturaDeposito || distancia < 0) {
    distancia = alturaDeposito; // Si hay error, asumir vacío
  }
  
  return distancia;
}

// Control de LEDs por depósito
void controlarLEDsDeposito(float nivel, int ledVerde, int ledAnaranjado, int ledRojo) {
  if (nivel >= 17.0) {
    // Nivel alto
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledAnaranjado, LOW);
    digitalWrite(ledRojo, LOW);
  } 
  else if (nivel >= 8.0) {
    // Nivel medio
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAnaranjado, HIGH);
    digitalWrite(ledRojo, LOW);
  } 
  else if (nivel >= 0.0) {
    // Nivel bajo
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAnaranjado, LOW);
    digitalWrite(ledRojo, HIGH);
  } 
  else {
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAnaranjado, LOW);
    digitalWrite(ledRojo, LOW);
  }
}

// Apagar LEDs al inicio
void apagarTodosLosLEDs() {
  digitalWrite(ledVerde1, LOW);
  digitalWrite(ledAnaranjado1, LOW);
  digitalWrite(ledRojo1, LOW);

  digitalWrite(ledVerde2, LOW);
  digitalWrite(ledAnaranjado2, LOW);
  digitalWrite(ledRojo2, LOW);

  digitalWrite(ledVerde3, LOW);
  digitalWrite(ledAnaranjado3, LOW);
  digitalWrite(ledRojo3, LOW);
}

// Mostrar información por Serial
void mostrarInfoSerial() {
  Serial.print("Deposito 1: ");
  Serial.print(nivel1);
  Serial.print("cm | ");
  Serial.print(obtenerEstado(nivel1));
  
  Serial.print(" || Deposito 2: ");
  Serial.print(nivel2);
  Serial.print("cm | ");
  Serial.print(obtenerEstado(nivel2));
  
  Serial.print(" || Deposito 3: ");
  Serial.print(nivel3);
  Serial.print("cm | ");
  Serial.println(obtenerEstado(nivel3));
}

// Retorna estado en texto
String obtenerEstado(float nivel) {
  if (nivel >= 17.0) return "ALTO (Verde)";
  else if (nivel >= 8.0) return "MEDIO (Anaranjado)";
  else if (nivel >= 0.0) return "BAJO (Rojo)";
  else return "VACIO";
}
