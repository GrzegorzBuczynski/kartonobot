
#include "motor_functions.h"

// Motor Connections (ENA & ENB must use PWM pins)
#define IN1 9
#define IN2 8
#define IN3 7
#define IN4 6
#define ENA 10
#define ENB 5
#define XPIN A0
#define YPIN A1
#define SWPIN A2

// Parametr maksymalnej mocy silników (0.0-1.0)
// Zmniejsz tę wartość aby ograniczyć maksymalną prędkość
// #define MAX_MOTOR_POWER 1.0  // Zakres: 0.0-1.0 (1.0 = pełna moc, 0.5 = połowa mocy)
// #define MAX_MOTOR_POWER 0.8   // 80% mocy  
// #define MAX_MOTOR_POWER 0.6   // 60% mocy
// #define MAX_MOTOR_POWER 0.5   // 50% mocy (połowa prędkości)
#define MAX_MOTOR_POWER 0.3   // 30% mocy (tryb powolny)
// #define MAX_MOTOR_POWER 0.1   // 10% mocy (tryb bardzo powolny)

// Kompensacja różnic w charakterystykach silników
// Wartość 0 = brak kompensacji, wartość 50 = dodanie 50 do PWM
#define BONUS_A 0    // Kompensacja dla silnika A (lewy - ENA)
#define BONUS_B 20   // Kompensacja dla silnika B (prawy - ENB)

void setup() {
  // Inicjalizacja komunikacji szeregowej
  Serial.begin(9600);
  Serial.println("L298N Motor Controller - Start");

  // Set motor connections as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(XPIN, INPUT);
  pinMode(YPIN, INPUT);
  pinMode(SWPIN, INPUT_PULLUP);

  // Start with motors off
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void loop() {
  float xVal = (analogRead(XPIN) - 512.0) / 512.0;  // Normalizacja do zakresu -1.0 do +1.0
  float yVal = (analogRead(YPIN) - 512.0) / 512.0;  // Normalizacja do zakresu -1.0 do +1.0
  int sVal = digitalRead(SWPIN);

  // Logowanie wartości analogowych do portu szeregowego
  Serial.print("X: ");
  Serial.print(xVal, 3);  // 3 cyfry po przecinku
  Serial.print(" | Y: ");
  Serial.print(yVal, 3);
  Serial.print(" | SW: ");
  Serial.print(sVal);
  Serial.println();  // Nowa linia
  
  // Opóźnienie dla czytelności logów (opcjonalne)
  delay(300);  // Odkomentuj jeśli logi są za szybkie
  
  // Aktywne sterowanie motorami
  vectorDriveMotors(xVal, yVal);
}
