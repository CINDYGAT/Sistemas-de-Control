// --- Pines ---
const int trigPin = 5;
const int echoPin = 6;
const int dirPin  = 8;
const int stepPin = 9;
// (Opcional) si tu driver tiene ENABLE, conéctalo y descomenta:
// const int enPin   = 4;

// --- Control de pasos (no bloqueante) ---
unsigned long lastStepMicros = 0;   // última vez que generamos un paso
unsigned long stepInterval   = 0;   // us entre pasos; 0 = parado

// --- Medición de distancia (no bloqueante) ---
unsigned long lastMeasureMillis = 0;
const unsigned long measureEveryMs = 50;  // frecuencia de medición (50 ms)
float distancia = 0.0;

// --- Utilidad: mide distancia con HC-SR04 ---
float medirDistanciaCm() {
  // Disparo
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Lectura con timeout para evitar bloqueos (por ej. 25 ms)
  unsigned long dur = pulseIn(echoPin, HIGH, 25000UL);
  if (dur == 0) {
    // sin eco: devuelve algo grande para "no hay obstáculo"
    return 999.0;
  }
  return (dur * 0.0343f) / 2.0f;
}

// --- Configura dirección y velocidad según distancia ---
void actualizarVelocidadSegunDistancia(float d) {
  // Define aquí tus zonas y velocidades (intervalo = us por paso)
  // Nota: el pulso mínimo HIGH del step suele ser 1–2 us (A4988/DRV8825).
  if (d >= 50) {                 // lejos: rápido hacia adelante
    digitalWrite(dirPin, HIGH);
    stepInterval = 800;         // ~714 pasos/s
  } else if (d <= 25 && d >= 10) {
    digitalWrite(dirPin, HIGH);
    stepInterval = 3000;         // ~143 pasos/s
  } else if (d <= 10 && d >= 6) {
    digitalWrite(dirPin, HIGH);
    stepInterval = 7000;         // ~111 pasos/s
  } else if (d <= 5) {           // muy cerca: gira al revés
    digitalWrite(dirPin, HIGH);
    stepInterval = 0;         // ~333 pasos/s
  } else {
    stepInterval = 0;            // fuera de rangos: parar
  }
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  // pinMode(enPin, OUTPUT); digitalWrite(enPin, LOW); // habilitar driver (activo en LOW en A4988)

  digitalWrite(stepPin, LOW);
  Serial.begin(9600);
}

void loop() {
  // 1) Medir distancia cada X ms (no bloquear)
  unsigned long nowMs = millis();
  if (nowMs - lastMeasureMillis >= measureEveryMs) {
    lastMeasureMillis = nowMs;
    distancia = medirDistanciaCm();
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");

    // Actualizar velocidad/dirección según la distancia medida
    actualizarVelocidadSegunDistancia(distancia);
  }

  // 2) Generar pasos continuamente según stepInterval (no bloquear)
  if (stepInterval > 0) {
    unsigned long nowUs = micros();
    if (nowUs - lastStepMicros >= stepInterval) {
      lastStepMicros = nowUs;

      // Un pulso "step": HIGH breve y volver a LOW
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(2);   // respeta el ancho mínimo de pulso del driver
      digitalWrite(stepPin, LOW);
    }
  }
  // Si stepInterval == 0, el motor se queda parado sin bloquear el loop.
}
