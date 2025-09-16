#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Dirección I2C del módulo LCD (puede ser 0x27 o 0x3F)
// Si no funciona con 0x27, prueba con 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();        // Inicializa el LCD
  lcd.backlight();   // Enciende la luz de fondo

  lcd.setCursor(0,0);
  lcd.print("Hola Cindy!");
  lcd.setCursor(0,1);
  lcd.print("LCD I2C listo");
}

void loop() {
  // Puedes hacer que parpadee el mensaje
  lcd.noBacklight(); // Apaga luz
  delay(500);
  lcd.backlight();   // Enciende luz
  delay(500);
}
