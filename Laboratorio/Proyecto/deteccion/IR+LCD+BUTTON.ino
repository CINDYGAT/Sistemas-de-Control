#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // ajusta 0x27 si tu módulo usa otra dirección

const int irAnalogPin = A0;   // entrada analógica del módulo IR (AO)
const int irDigitalPin = 6;   // salida digital del módulo (DO), opcional
const int ledIndicador = 13;  // LED indicador en placa

// Botón principal
const int pequeno = 7;

// Umbral inicial para detección (ajusta según tu sensor y pruebas)
int umbral = 300; // 0..1023, valores < umbral => objeto cerca (depende del módulo)

// Para calibración rápida con botón (opcional)
const int btnCalibrar = 8; // cambialo a otro pin distinto de 7

void setup() {
  pinMode(irDigitalPin, INPUT); // solo si dispones de DO
  pinMode(ledIndicador, OUTPUT);
  pinMode(btnCalibrar, INPUT_PULLUP);
  pinMode(pequeno, INPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IR sensor ready");
  delay(800);
  lcd.clear();
}

void loop() {
  int lectura = analogRead(irAnalogPin); // 0..1023
  int estadoDigital = digitalRead(irDigitalPin); // si existe DO
  int estadoBoton = digitalRead(pequeno); // leer el botón

  bool detectado = lectura < umbral; // depende del módulo: prueba invertir > o <

  // Mostrar en LCD
  lcd.setCursor(0,0);
  lcd.print("IR raw:");
  lcd.print(lectura);
  lcd.print("   "); // limpia restos

  lcd.setCursor(0,1);
  if (estadoBoton == HIGH && detectado) {   // botón presionado + objeto detectado
    lcd.print("Objeto detectado ");
    digitalWrite(ledIndicador, HIGH);
  } 
  else if (estadoBoton == HIGH && !detectado) { // botón presionado, sin objeto
    lcd.print("No detectado     ");
    digitalWrite(ledIndicador, LOW);
  } 
  else { // botón no presionado
    lcd.print("Esperando boton  ");
    digitalWrite(ledIndicador, LOW);
  }

  // Mostrar estado DO (si lo tienes) en Serial
  Serial.print("Analog: "); Serial.print(lectura);
  Serial.print("  DO: "); Serial.print(estadoDigital);
  Serial.print("  Detect: "); Serial.println(detectado);

  // Calibración simple: si pulsas btnCalibrar, guarda la lectura actual como umbral
  if (digitalRead(btnCalibrar) == LOW) {
    umbral = lectura;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Umbral guardado:");
    lcd.setCursor(0,1);
    lcd.print(umbral);
    delay(1000);
    lcd.clear();
  }

  delay(200); // frecuencia de muestreo
}
