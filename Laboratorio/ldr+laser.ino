// Pines de conexión
const int laserPin = 9;      // KY-008 Láser
const int ldrPin = A0;       // Fotoresistencia (analógico)

// Variables de calibración
int umbral;                  // Umbral de detección
int valorSinLaser;           // Valor LDR sin láser
int valorConLaser;           // Valor LDR con láser

// Estado del sistema
bool objetoDetectado = false;

void setup() {
  Serial.begin(9600);
  
  // Configurar pines
  pinMode(laserPin, OUTPUT);
  
  Serial.println("=== SENSOR LASER-LDR ===");
  Serial.println("Calibrando automaticamente...");
  
  // Calibración automática
  calibrarSistema();
  
  Serial.println("Sistema listo!");
  Serial.println("----------------------------");
}

void loop() {
  // Leer valor del LDR
  int valorLDR = analogRead(ldrPin);
  
  // Detectar si hay objeto
  bool hayObjeto = detectarObjeto(valorLDR);
  
  // Mostrar resultados por Serial
  mostrarResultados(valorLDR, hayObjeto);
  
  delay(200); // Pequeña pausa
}

// 🎯 FUNCIÓN: Calibración automática
void calibrarSistema() {
  // 1. Medir luz ambiente (láser APAGADO)
  digitalWrite(laserPin, LOW);
  delay(1000);
  valorSinLaser = promediarLectura(10);
  
  // 2. Medir luz con láser (láser ENCENDIDO)
  digitalWrite(laserPin, HIGH);
  delay(1000);
  valorConLaser = promediarLectura(10);
  
  // 3. Calcular umbral automático (punto medio)
  umbral = (valorSinLaser + valorConLaser) / 2;
  
  // Mostrar valores de calibración
  Serial.println("=== CALIBRACION COMPLETADA ===");
  Serial.print("Sin laser: "); Serial.println(valorSinLaser);
  Serial.print("Con laser: "); Serial.println(valorConLaser);
  Serial.print("Umbral: "); Serial.println(umbral);
  Serial.println();
}

// 🎯 FUNCIÓN: Detectar objeto
bool detectarObjeto(int valorLDR) {
  // Si el valor está más cerca del "sin láser" → OBJETO DETECTADO
  if (abs(valorLDR - valorSinLaser) < abs(valorLDR - valorConLaser)) {
    return true;
  } else {
    return false;
  }
}

// 🎯 FUNCIÓN: Promediar lecturas para mayor precisión
int promediarLectura(int muestras) {
  long suma = 0;
  for (int i = 0; i < muestras; i++) {
    suma += analogRead(ldrPin);
    delay(50);
  }
  return suma / muestras;
}

// 🎯 FUNCIÓN: Mostrar resultados en Monitor Serial
void mostrarResultados(int valorLDR, bool hayObjeto) {
  Serial.print("LDR: ");
  Serial.print(valorLDR);
  Serial.print(" | ");
  
  if (hayObjeto) {
    Serial.print(">>> OBJETO DETECTADO! <<<");
  } else {
    Serial.print("Camino libre");
  }
  
  // Barra visual de intensidad
  Serial.print(" [");
  int barras = map(valorLDR, 0, 1023, 0, 20);
  for (int i = 0; i < barras; i++) {
    Serial.print("|");
  }
  Serial.println("]");
}
