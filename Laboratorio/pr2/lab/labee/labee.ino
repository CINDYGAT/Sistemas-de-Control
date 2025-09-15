#include <LiquidCrystal.h>

// Pines LCD
const int rs = 12;
const int en = 13;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

//variables para pwm analogico digital
const int fanPin = 9;       // Salida PWM al ventilador
const int buttonUpPin = 8;  // Botón para aumentar el duty cycle
const int buttonDownPin = 7; // Botón para disminuir el duty cycle

// Pines Puente H (ejemplo para L298N)
const int motorIn1 = 11;   // Entrada 1 puente H
const int motorIn2 = 10;  // Entrada 2 puente H
const int motorEn = 6;    // Enable (PWM para velocidad, opcional)

// Potenciómetro
const int potPin = A0;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variables de control
int dutySteps[] = {0, 25, 50, 75, 100};  // Pasos de duty cycle
int currentStep = 0;
int lastButtonUpState = HIGH;
int lastButtonDownState = HIGH;
unsigned long lastDebounceTimeUp = 0;
unsigned long lastDebounceTimeDown = 0;
const unsigned long debounceDelay = 50;  // Tiempo anti-rebote

//Variable para boton de cambio de modo
const int pinModo = A1;  // botón para cambiar modo

void setup() {
  //modo analogico - digital
  pinMode(fanPin, OUTPUT);
  pinMode(buttonUpPin, INPUT_PULLUP);    // Botón con resistencia pull-up interna
  pinMode(buttonDownPin, INPUT_PULLUP);  // Botón con resistencia pull-up interna
  pinMode(pinModo, INPUT_PULLUP);  
  analogWrite(fanPin, 0);                // Inicializar ventilador apagado

  //modo analogico - analogico
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorEn, OUTPUT);

  // Motor apagado al inicio
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(motorEn, 0);

  //LCD instrucciones
  lcd.begin(16, 2);
  lcd.print("Ventilador OFF");
  lcd.setCursor(0, 1);
  //lcd.print("Duty: 0%");
  lcd.print("Motor OFF");
}

void loop() {
  //deteccion del primer
  if (digitalRead(pinModo) == LOW) {  
    int buttonUpState = digitalRead(buttonUpPin);
    int buttonDownState = digitalRead(buttonDownPin);
    
    // Detección de flanco descendente (presión del botón de subir)
    if (buttonUpState == LOW && lastButtonUpState == HIGH) {
      if ((millis() - lastDebounceTimeUp) > debounceDelay) {
        aumentarDutyCycle();
        lastDebounceTimeUp = millis();
      }
    }
    
    // Detección de flanco descendente (presión del botón de bajar)
    if (buttonDownState == LOW && lastButtonDownState == HIGH) {
      if ((millis() - lastDebounceTimeDown) > debounceDelay) {
        disminuirDutyCycle();
        lastDebounceTimeDown = millis();
      }
    }
    
    lastButtonUpState = buttonUpState;
    lastButtonDownState = buttonDownState;
  }else {
    // Cambiar a modo analógico-analógico
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
}

void aumentarDutyCycle() {
  if (currentStep < 4) {   // límite superior (5 pasos → 100%)
    currentStep++;
  }
  actualizarVentilador();
}

void disminuirDutyCycle() {
  if (currentStep > 0) {   // límite inferior (0 → 0%)
    currentStep--;
  }
  actualizarVentilador();
}

void actualizarVentilador() {
  int porcentaje = dutySteps[currentStep];
  int pwmValue = map(porcentaje, 0, 100, 0, 255);
  
  analogWrite(fanPin, pwmValue);
  
  // Actualizar LCD
  lcd.clear();
  lcd.print("Duty: ");
  lcd.print(porcentaje);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  if (porcentaje == 0) {
    lcd.print("Ventilador OFF");
  } else {
    lcd.print("PWM: ");
    lcd.print(pwmValue);
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
