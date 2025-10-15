// SISTEMA MEZCLADORA - MEZCLA->SUMINISTRO
// Bomba controlada por PUENTE H (L298N) y Stepper A4988
// Fecha: 14/10/2025
// MODIFICADO: Control de stepper por tamaño de recipiente y altura específica

#include <Servo.h>

// ===== PINES MOTOR MEZCLA (L298N) =====
const int IN1_MEZCLA = 4;
const int IN2_MEZCLA = 6;
const int ENA_MEZCLA = 5; // PWM para velocidad de mezcla

// ===== PINES PARA LA BOMBA (USANDO EL PUENTE H) =====
const int IN1_BOMBA = 11;    // Cambiado de IN1_SUMINISTRO a IN1_BOMBA
const int IN2_BOMBA = 12;    // Cambiado de IN2_SUMINISTRO a IN2_BOMBA  
const int ENA_BOMBA = 9;     // Cambiado de ENA_SUMINISTRO a ENA_BOMBA

// ===== PINES BOTONES =====
const int botonPequeno = 7;
const int botonMediano = 8;
const int botonGrande = 10;  // MOVIDO de pin 9 a 10 (9 ahora es ENA_BOMBA)
const int botonParar = 13;   // MOVIDO a pin 13

// ===== ULTRASÓNICO =====
const int trigPin = 2;
const int echoPin = 3;

// ===== PINES SERVOS =====
const int pinServo2 = A1;
const int pinServo3 = A2;
Servo servo2, servo3;

// ===== PINES STEPPER A4988 =====
const int dirPin  = A3;
const int stepPin = A4;

// ===== VARIABLES DE CONTROL =====
bool dosificando = false;
unsigned long tiempoInicioDosificacion = 0;
unsigned long tiempoDosificacion = 0;

int velocidadBaseMezcla = 200;
unsigned long tiempoInicioMezcla = 0;
unsigned long tiempoFinMezcla = 0;
unsigned long tiempoMezcla = 0;
bool mezclando = false;

bool suministroActivo = false;
bool suministroCompletado = false;

// ===== ULTRASÓNICO =====
float distancia = 0;

// ===== CONTROL STEPPER =====
unsigned long lastStepMicros = 0;
unsigned long stepInterval = 0; // microsegundos entre pasos; 0 = parado

// ===== CONTROL BOMBEO =====
bool bombaActiva = false;

// ===== ESTADOS =====
enum EstadoSistema { REPOSO, DOSIFICANDO, MEZCLANDO, SUMINISTRANDO, COMPLETADO };
EstadoSistema estadoActual = REPOSO;

// ===== CONFIGURACIÓN DOSIFICACION =====
struct ConfiguracionDosificacion {
  int tiempoDosificacion;   // s - tiempo que bomba y servos están activos
  int anguloServo;          // grados (para servos 2 y 3)
  int tiempoMezcla;         // s - tiempo de mezcla después de dosificación
  String nombre;
};

ConfiguracionDosificacion configPequeno = {5, 60, 30, "PEQUEÑO"};    // 5s dosificación, 30s mezcla
ConfiguracionDosificacion configMediano = {10, 90, 60, "MEDIANO"};   // 10s dosificación, 60s mezcla
ConfiguracionDosificacion configGrande = {15, 120, 90, "GRANDE"};    // 15s dosificación, 90s mezcla

// ===== SERIAL INPUT =====
String inputString = "";
boolean stringComplete = false;

// ===== VARIABLES PARA CONTROL DE STEPPER POR TAMAÑO =====
String tamanoActual = "";
float alturaObjetivo = 0.0;
const float UMBRAL_PEQUENO = 5.0;   // 5 cm para recipiente pequeño
const float UMBRAL_MEDIANO = 10.0;  // 10 cm para recipiente mediano  
const float UMBRAL_GRANDE = 15.0;   // 15 cm para recipiente grande

// ===== SETUP =====
void setup() {
  Serial.begin(9600);
  inputString.reserve(200);

  // PINS mezcla
  pinMode(IN1_MEZCLA, OUTPUT);
  pinMode(IN2_MEZCLA, OUTPUT);
  pinMode(ENA_MEZCLA, OUTPUT);
  apagarMotorMezcla();

  // PINS bomba - AHORA PARA AGUA DURANTE DOSIFICACIÓN
  pinMode(IN1_BOMBA, OUTPUT);
  pinMode(IN2_BOMBA, OUTPUT);
  pinMode(ENA_BOMBA, OUTPUT);
  apagarBomba();

  // PINS botones (actualizados)
  pinMode(botonPequeno, INPUT_PULLUP);
  pinMode(botonMediano, INPUT_PULLUP);
  pinMode(botonGrande, INPUT_PULLUP);
  pinMode(botonParar, INPUT_PULLUP);

  // Ultrasonico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Stepper
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, HIGH); // Dirección por defecto

  // Servos
  servo2.attach(pinServo2);
  servo3.attach(pinServo3);
  cerrarServos();

  Serial.println("SISTEMA MEZCLADORA - CONTROL DE STEPPER POR TAMAÑO Y ALTURA");
  Serial.println("Umbrales: Pequeño(5cm), Mediano(10cm), Grande(15cm)");
  Serial.println("Comandos: P (Pequeña), M (Mediana), G (Grande), S (Stop)");
}

// ===== LOOP =====
void loop() {
  if (stringComplete) {
    procesarComando(inputString);
    inputString = "";
    stringComplete = false;
  }

  leerBotones();
  maquinaDeEstados();

  // Generar pasos del stepper si está activo en suministro
  if (estadoActual == SUMINISTRANDO && stepInterval > 0) {
    generarPasos();
  }

  delay(5);
}

// ===== MAQUINA DE ESTADOS =====
void maquinaDeEstados() {
  switch (estadoActual) {
    case REPOSO: 
      break;
    
    case DOSIFICANDO: 
      controlarDosificacion(); 
      break;
      
    case MEZCLANDO: 
      controlarMezcla(); 
      break;
      
    case SUMINISTRANDO: 
      controlarSuministro(); 
      break;
      
    case COMPLETADO:
      if (millis() - tiempoFinMezcla > 5000) {
        estadoActual = REPOSO;
        Serial.println("Proceso completamente finalizado. Sistema en reposo.");
      }
      break;
  }
}

// ===== CONTROL DOSIFICACIÓN =====
void controlarDosificacion() {
  unsigned long ahora = millis();
  unsigned long tiempoTranscurrido = (ahora - tiempoInicioDosificacion) / 1000UL;
  
  if (dosificando) {
    // Mostrar progreso cada 2 segundos
    static unsigned long ultimoMensaje = 0;
    if (ahora - ultimoMensaje >= 2000) {
      ultimoMensaje = ahora;
      Serial.print("Dosificación: ");
      Serial.print(tiempoTranscurrido);
      Serial.print("/");
      Serial.print(tiempoDosificacion);
      Serial.println("s");
    }
    
    if (tiempoTranscurrido >= tiempoDosificacion) {
      // FIN DE DOSIFICACIÓN - Apagar bomba y cerrar servos
      dosificando = false;
      apagarBomba();
      cerrarServos();
      
      Serial.println("DOSIFICACIÓN COMPLETADA - iniciando mezcla");

      // INICIAR MEZCLA
      estadoActual = MEZCLANDO;
      tiempoInicioMezcla = ahora;
      tiempoFinMezcla = tiempoInicioMezcla + (tiempoMezcla * 1000UL);
      mezclando = true;

      // Encender motor de mezcla
      analogWrite(ENA_MEZCLA, velocidadBaseMezcla);
      digitalWrite(IN1_MEZCLA, LOW);
      digitalWrite(IN2_MEZCLA, HIGH);
    }
  }
}

// ===== CONTROL MEZCLA =====
void controlarMezcla() {
  unsigned long ahora = millis();
  unsigned long tiempoTranscurrido = (ahora - tiempoInicioMezcla) / 1000UL;
  unsigned long tiempoRestante = (tiempoFinMezcla - ahora) / 1000UL;
  
  // Mostrar progreso cada 5 segundos
  static unsigned long ultimoMensaje = 0;
  if (ahora - ultimoMensaje >= 5000) {
    ultimoMensaje = ahora;
    Serial.print("Mezcla: ");
    Serial.print(tiempoTranscurrido);
    Serial.print("/");
    Serial.print(tiempoMezcla);
    Serial.print("s - Restante: ");
    Serial.print(tiempoRestante);
    Serial.println("s");
  }
  
  if (ahora >= tiempoFinMezcla) {
    mezclando = false;
    apagarMotorMezcla();
    Serial.println("MEZCLA COMPLETADA - iniciando suministro");
    iniciarSuministro();
  }
}

// ===== INICIAR SUMINISTRO =====
void iniciarSuministro() {
  Serial.println("=== INICIANDO SUMINISTRO DE MEZCLA ===");

  // Configurar stepper según el tamaño del recipiente
  configurarStepperPorTamano();

  suministroActivo = true;
  suministroCompletado = false;
  estadoActual = SUMINISTRANDO;
  
  Serial.print("Stepper configurado para recipiente ");
  Serial.println(tamanoActual);
}

// ===== CONFIGURAR STEPPER SEGÚN TAMAÑO =====
void configurarStepperPorTamano() {
  if (tamanoActual == "PEQUEÑO") {
    alturaObjetivo = UMBRAL_PEQUENO;
    stepInterval = 3000; // Velocidad media para pequeño
  } else if (tamanoActual == "MEDIANO") {
    alturaObjetivo = UMBRAL_MEDIANO;
    stepInterval = 2000; // Velocidad más rápida para mediano
  } else if (tamanoActual == "GRANDE") {
    alturaObjetivo = UMBRAL_GRANDE;
    stepInterval = 1500; // Velocidad más rápida para grande
  }
  
  digitalWrite(dirPin, HIGH); // Dirección de salida
  Serial.print("Altura objetivo: ");
  Serial.print(alturaObjetivo);
  Serial.println(" cm");
}

// ===== CONTROL SUMINISTRO CON LÓGICA POR TAMAÑO =====
void controlarSuministro() {
  // Medir distancia continuamente
  medirDistancia();
  
  // Aplicar lógica de control según el tamaño del recipiente
  controlarStepperPorAltura();
}

// ===== LÓGICA DE CONTROL DE STEPPER POR ALTURA Y TAMAÑO =====
void controlarStepperPorAltura() {
  // Usar switch para mejor organización del código
  if (tamanoActual == "PEQUEÑO") {
    controlarStepperPequeno();
  } else if (tamanoActual == "MEDIANO") {
    controlarStepperMediano();
  } else if (tamanoActual == "GRANDE") {
    controlarStepperGrande();
  }
}

void controlarStepperPequeno() {
  if (distancia >= UMBRAL_PEQUENO) {
    // Si mide 5cm o más, PARAR stepper
    stepInterval = 0;
    Serial.println("Recipiente PEQUEÑO lleno (5cm) - Stepper DETENIDO");
    detenerSuministro();
  } else {
    // Si mide menos de 5cm, ACTIVAR/MANTENER stepper
    if (stepInterval == 0) {
      stepInterval = 3000; // Reactivar si estaba detenido
      Serial.println("Recipiente PEQUEÑO - Stepper ACTIVADO");
    }
  }
}

void controlarStepperMediano() {
  if (distancia >= UMBRAL_MEDIANO) {
    // Si mide 10cm o más, PARAR stepper
    stepInterval = 0;
    Serial.println("Recipiente MEDIANO lleno (10cm) - Stepper DETENIDO");
    detenerSuministro();
  } else {
    // Si mide menos de 10cm, ACTIVAR/MANTENER stepper
    if (stepInterval == 0) {
      stepInterval = 2000; // Reactivar si estaba detenido
      Serial.println("Recipiente MEDIANO - Stepper ACTIVADO");
    }
  }
}

void controlarStepperGrande() {
  if (distancia >= UMBRAL_GRANDE) {
    // Si mide 15cm o más, PARAR stepper
    stepInterval = 0;
    Serial.println("Recipiente GRANDE lleno (15cm) - Stepper DETENIDO");
    detenerSuministro();
  } else {
    // Si mide menos de 15cm, ACTIVAR/MANTENER stepper
    if (stepInterval == 0) {
      stepInterval = 1500; // Reactivar si estaba detenido
      Serial.println("Recipiente GRANDE - Stepper ACTIVADO");
    }
  }
}

// ===== DETENER SUMINISTRO =====
void detenerSuministro() {
  // Detener stepper
  stepInterval = 0;
  digitalWrite(stepPin, LOW);

  suministroActivo = false;
  suministroCompletado = true;
  estadoActual = COMPLETADO;
  Serial.println("=== SUMINISTRO COMPLETADO ===");
}

// ===== ULTRASÓNICO =====
void medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long dur = pulseIn(echoPin, HIGH, 25000UL);
  distancia = (dur == 0) ? 999.0 : (dur * 0.0343) / 2.0;

  // Mostrar medición cada 2 segundos
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();
    Serial.print("Altura medida: ");
    Serial.print(distancia);
    Serial.print(" cm | Objetivo: ");
    Serial.print(alturaObjetivo);
    Serial.println(" cm");
  }
}

// ===== STEPPER =====
void generarPasos() {
  unsigned long nowUs = micros();
  if (stepInterval > 0 && (nowUs - lastStepMicros) >= stepInterval) {
    lastStepMicros = nowUs;
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2);
    digitalWrite(stepPin, LOW);
  }
}

// ===== SERVOS =====
void abrirServomotores(int angulo) {
  servo2.write(angulo);
  servo3.write(angulo);
  Serial.print("Servos abiertos a ");
  Serial.print(angulo);
  Serial.println(" grados");
}

void cerrarServos() {
  servo2.write(0);
  servo3.write(0);
  Serial.println("Servos cerrados");
}

// ===== BOMBA DE AGUA =====
void encenderBomba() {
  digitalWrite(IN1_BOMBA, HIGH);
  digitalWrite(IN2_BOMBA, LOW);
  analogWrite(ENA_BOMBA, 255); // Velocidad máxima
  bombaActiva = true;
  Serial.println("Bomba de agua ENCENDIDA");
}

void apagarBomba() {
  digitalWrite(IN1_BOMBA, LOW);
  digitalWrite(IN2_BOMBA, LOW);
  analogWrite(ENA_BOMBA, 0);
  bombaActiva = false;
  Serial.println("Bomba de agua APAGADA");
}

// ===== MOTORES MEZCLA =====
void apagarMotorMezcla() {
  digitalWrite(IN1_MEZCLA, LOW);
  digitalWrite(IN2_MEZCLA, LOW);
  analogWrite(ENA_MEZCLA, 0);
}

// ===== BOTONES =====
void leerBotones() {
  static int lastBotonPequeno = HIGH;
  static int lastBotonMediano = HIGH;
  static int lastBotonGrande = HIGH;
  static int lastBotonParar = HIGH;

  int lecturaPequeno = digitalRead(botonPequeno);
  int lecturaMediano = digitalRead(botonMediano);
  int lecturaGrande = digitalRead(botonGrande);
  int lecturaParar = digitalRead(botonParar);

  // Botón PEQUEÑO (7)
  if (lecturaPequeno == LOW && lastBotonPequeno == HIGH) { 
    delay(50); 
    if (digitalRead(botonPequeno) == LOW && estadoActual == REPOSO) 
      iniciarProcesoPorTamano("PEQUEÑO"); 
  }
  
  // Botón MEDIANO (8)
  if (lecturaMediano == LOW && lastBotonMediano == HIGH) { 
    delay(50); 
    if (digitalRead(botonMediano) == LOW && estadoActual == REPOSO) 
      iniciarProcesoPorTamano("MEDIANO"); 
  }
  
  // Botón GRANDE (10)
  if (lecturaGrande == LOW && lastBotonGrande == HIGH) { 
    delay(50); 
    if (digitalRead(botonGrande) == LOW && estadoActual == REPOSO) 
      iniciarProcesoPorTamano("GRANDE"); 
  }
  
  // Botón PARAR (13) - DETIENE INMEDIATAMENTE
  if (lecturaParar == LOW && lastBotonParar == HIGH) { 
    delay(50); 
    if (digitalRead(botonParar) == LOW) {
      stepInterval = 0; // Detener stepper inmediatamente
      detenerProceso();
    }
  }

  lastBotonPequeno = lecturaPequeno;
  lastBotonMediano = lecturaMediano;
  lastBotonGrande = lecturaGrande;
  lastBotonParar = lecturaParar;
}

// ===== SERIAL =====
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') stringComplete = true;
  }
}

void procesarComando(String comando) {
  comando.toUpperCase();
  if (comando.startsWith("P") && estadoActual == REPOSO) 
    iniciarProcesoPorTamano("PEQUEÑO");
  else if (comando.startsWith("M") && estadoActual == REPOSO) 
    iniciarProcesoPorTamano("MEDIANO");
  else if (comando.startsWith("G") && estadoActual == REPOSO) 
    iniciarProcesoPorTamano("GRANDE");
  else if (comando.startsWith("S")) {
    stepInterval = 0; // Detener stepper inmediatamente
    detenerProceso();
  }
  else 
    Serial.println("Comando no reconocido o sistema no en reposo");
}

// ===== INICIAR / DETENER PROCESO =====
void iniciarProcesoPorTamano(String tamano) {
  ConfiguracionDosificacion cfg;
  if (tamano == "PEQUEÑO") cfg = configPequeno;
  else if (tamano == "MEDIANO") cfg = configMediano;
  else if (tamano == "GRANDE") cfg = configGrande;
  else { 
    Serial.println("Tamaño no reconocido"); 
    return; 
  }

  // Guardar el tamaño actual para el control del stepper
  tamanoActual = tamano;

  // Configurar dosificación
  dosificando = true;
  tiempoDosificacion = cfg.tiempoDosificacion;
  tiempoInicioDosificacion = millis();
  tiempoMezcla = cfg.tiempoMezcla;

  // INICIAR DOSIFICACIÓN: Abrir servos Y encender bomba
  abrirServomotores(cfg.anguloServo);
  encenderBomba();

  // Cambiar estado
  estadoActual = DOSIFICANDO;
  
  Serial.print("=== INICIANDO PROCESO ");
  Serial.print(cfg.nombre);
  Serial.print(" ===");
  Serial.print(" Dosificación: ");
  Serial.print(tiempoDosificacion);
  Serial.print("s, Mezcla: ");
  Serial.print(tiempoMezcla);
  Serial.println("s");
}

void detenerProceso() {
  // Detener todo inmediatamente
  dosificando = false;
  mezclando = false;
  suministroActivo = false;
  suministroCompletado = false;

  apagarMotorMezcla();
  apagarBomba();
  stepInterval = 0; // Asegurar que stepper esté detenido
  cerrarServos();

  estadoActual = REPOSO;
  Serial.println("=== PROCESO DETENIDO MANUALMENTE ===");
}
