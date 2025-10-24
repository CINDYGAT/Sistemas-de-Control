#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuración de la LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== PINES PARA BOTONES FÍSICOS =====
const int botonPequeno = 2;
const int botonMediano = 3;
const int botonGrande = 4;

// ====== Pines para los 3 sensores IR =====
const int sensorAlto = A0;
const int sensorMedio = A1;
const int sensorBajo = A2;

// === Umbral para detección IR ====
int UMBRAL_DETECCION = 500; // Este valor puede necesitar ajuste

// Estado de selección del recipiente
String tipoRecipienteSeleccionado = "";

// Para evitar rebotes
unsigned long ultimoTiempoBoton = 0;
const int TIEMPO_DEBOUNCE = 200;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  // Configurar pines de botones
  pinMode(botonPequeno, INPUT_PULLUP);
  pinMode(botonMediano, INPUT_PULLUP);
  pinMode(botonGrande, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  lcd.print("Sistema Listo");
  lcd.setCursor(0, 1);
  lcd.print("Calibrando...");
  
  // Calibrar sensores - mostrar valores iniciales
  delay(2000);
  calibrarSensores();
  lcd.clear();
}

void calibrarSensores() {
  lcd.clear();
  lcd.print("Calibrando IR...");
  
  int valorAlto = analogRead(sensorAlto);
  int valorMedio = analogRead(sensorMedio);
  int valorBajo = analogRead(sensorBajo);
  
  Serial.println("=== VALORES DE CALIBRACION ===");
  Serial.print("Sensor Alto (A0): ");
  Serial.println(valorAlto);
  Serial.print("Sensor Medio (A1): ");
  Serial.println(valorMedio);
  Serial.print("Sensor Bajo (A2): ");
  Serial.println(valorBajo);
  Serial.println("=============================");
  
  // Ajustar umbral automáticamente (puedes modificar esto)
  UMBRAL_DETECCION = (valorAlto + valorMedio + valorBajo) / 6; // Empírico
  
  lcd.clear();
  lcd.print("Umbral: ");
  lcd.print(UMBRAL_DETECCION);
  delay(2000);
}

void loop() {
  // Leer los botones físicos con anti-rebote
  if (millis() - ultimoTiempoBoton > TIEMPO_DEBOUNCE) {
    leerBotones();
  }
}

void leerBotones() {
  int lecturaPequeno = digitalRead(botonPequeno);
  int lecturaMediano = digitalRead(botonMediano);
  int lecturaGrande = digitalRead(botonGrande);

  // Detectar flancos de bajada (botón presionado)
  if (lecturaPequeno == LOW) {
    ultimoTiempoBoton = millis();
    tipoRecipienteSeleccionado = "Pequeño";
    mostrarSeleccion("Recipiente Pequeño");
    comprobarRecipiente(tipoRecipienteSeleccionado);
  }

  if (lecturaMediano == LOW) {
    ultimoTiempoBoton = millis();
    tipoRecipienteSeleccionado = "Mediano";
    mostrarSeleccion("Recipiente Mediano");
    comprobarRecipiente(tipoRecipienteSeleccionado);
  }

  if (lecturaGrande == LOW) {
    ultimoTiempoBoton = millis();
    tipoRecipienteSeleccionado = "Grande";
    mostrarSeleccion("Recipiente Grande");
    comprobarRecipiente(tipoRecipienteSeleccionado);
  }
}

void mostrarSeleccion(String mensaje) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Seleccionaste:");
  lcd.setCursor(0, 1);
  lcd.print(mensaje);
  delay(2000);
  lcd.clear();
}

void comprobarRecipiente(String tipoRecipiente) {
  // Leer múltiples veces para mayor estabilidad
  int valorAlto = 0, valorMedio = 0, valorBajo = 0;
  
  for(int i = 0; i < 5; i++) {
    valorAlto += analogRead(sensorAlto);
    valorMedio += analogRead(sensorMedio);
    valorBajo += analogRead(sensorBajo);
    delay(10);
  }
  
  valorAlto /= 5;
  valorMedio /= 5;
  valorBajo /= 5;

  // Convertir a detección binaria
  bool detectaAlto = (valorAlto < UMBRAL_DETECCION);
  bool detectaMedio = (valorMedio < UMBRAL_DETECCION);
  bool detectaBajo = (valorBajo < UMBRAL_DETECCION);

  String resultado = "No cumple";

  // MOSTRAR INFORMACIÓN DE DEPURACIÓN
  Serial.print("IR - Bajo:");
  Serial.print(valorBajo);
  Serial.print(" Medio:");
  Serial.print(valorMedio);
  Serial.print(" Alto:");
  Serial.print(valorAlto);
  Serial.print(" | Binario - B:");
  Serial.print(detectaBajo);
  Serial.print(" M:");
  Serial.print(detectaMedio);
  Serial.print(" A:");
  Serial.print(detectaAlto);
  Serial.print(" | Seleccionado: ");
  Serial.println(tipoRecipiente);

  // Condiciones más flexibles
  if (tipoRecipiente == "Pequeño") {
    if (detectaBajo && !detectaMedio && !detectaAlto) {
      resultado = "Pequeño OK";
    } else if (detectaBajo) {
      resultado = "Pequeno OK";
    }
  }  
  else if (tipoRecipiente == "Mediano") {
    if (detectaBajo && detectaMedio && !detectaAlto) {
      resultado = "Mediano OK";
    } else if (detectaBajo && detectaMedio) {
      resultado = "Mediano OK";
    }
  } 
  else if (tipoRecipiente == "Grande") {
    if (detectaBajo && detectaMedio && detectaAlto) {
      resultado = "Grande OK";
    }
  }

  // Mostrar el resultado en LCD
  lcd.setCursor(0, 0);
  lcd.print("Verificando...");
  lcd.setCursor(0, 1);
  lcd.print(resultado);

  delay(3000);
  lcd.clear();
}
