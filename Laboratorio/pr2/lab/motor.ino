#include <LiquidCrystal.h>

// Pines LCD
const int rs = 12;
const int en = 13;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

// Pines Puente H (ejemplo para L298N)
const int motorIn1 = 11;   // Entrada 1 puente H
const int motorIn2 = 10;  // Entrada 2 puente H
const int motorEn = 6;    // Enable (PWM para velocidad, opcional)

// Potenciómetro
const int potPin = A0;

// Inicializar LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  lcd.begin(16, 2);

  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorEn, OUTPUT);

  // Motor apagado al inicio
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(motorEn, 0);

  lcd.print("Motor OFF");
}

void loop() {
  int valorPot = analogRead(potPin); // 0–1023
  lcd.setCursor(0, 1);
  lcd.print("Pot: ");
  lcd.print(valorPot);

  if (valorPot <= 300) {
    detenerMotor();
    lcd.setCursor(0, 0);
    lcd.print("Motor OFF     ");
  }
  else if (valorPot > 300 && valorPot <= 700) {
    moverMotorAdelante();
    lcd.setCursor(0, 0);
    lcd.print("Motor FWD     ");
  }
  else if (valorPot > 700) {
    moverMotorAtras();
    lcd.setCursor(0, 0);
    lcd.print("Motor REV     ");
  }
}

// Funciones para controlar el motor
void detenerMotor() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(motorEn, 0); // velocidad 0
}

void moverMotorAdelante() {
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  analogWrite(motorEn, 200); // puedes ajustar velocidad
}

void moverMotorAtras() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
  analogWrite(motorEn, 200); // puedes ajustar velocidad
}
