#include <Arduino.h>
#include "motor_functions.h"

// Definicje pinów (kopiowane z głównego pliku)
#define IN1 9
#define IN2 8
#define IN3 7
#define IN4 6
#define ENA 10
#define ENB 5

// Definicje stałych (kopiowane z głównego pliku)
#define MAX_MOTOR_POWER 0.3
#define BONUS_A 0
#define BONUS_B 20

// Podfunkcje sterowania motorami
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  // Ograniczenie wartości do zakresu 0-255 (konwersja z 0.0-1.0)
  int maxPwm = MAX_MOTOR_POWER * 255;
  if (leftSpeed > maxPwm) leftSpeed = maxPwm;
  if (leftSpeed < 0) leftSpeed = 0;
  if (rightSpeed > maxPwm) rightSpeed = maxPwm;
  if (rightSpeed < 0) rightSpeed = 0;
  
  // Dodanie bonusów kompensacyjnych gdy silniki pracują
  if (leftSpeed > 0) leftSpeed += BONUS_A;
  if (rightSpeed > 0) rightSpeed += BONUS_B;
  
  // Zabezpieczenie przed przekroczeniem maksymalnego PWM po dodaniu bonusu
  if (leftSpeed > 255) leftSpeed = 255;
  if (rightSpeed > 255) rightSpeed = 255;
  
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

void driveForward(float speed, float turn) {
  forward();
  int baseSpeed = (MAX_MOTOR_POWER * 255) * speed;
  
  if (turn > 0) {
    // Skręt w prawo - zmniejsz prędkość prawego silnika
    setMotorSpeeds(baseSpeed, baseSpeed * (1 - turn));
  } else if (turn < 0) {
    // Skręt w lewo - zmniejsz prędkość lewego silnika
    setMotorSpeeds(baseSpeed * (1 + turn), baseSpeed);
  } else {
    // Jazda prosto
    setMotorSpeeds(baseSpeed, baseSpeed);
  }
}

void driveReverse(float speed, float turn) {
  reverse();
  int baseSpeed = (MAX_MOTOR_POWER * 255) * speed;
  
  if (turn > 0) {
    // Skręt w prawo podczas cofania
    setMotorSpeeds(baseSpeed, baseSpeed * (1 - turn));
  } else if (turn < 0) {
    // Skręt w lewo podczas cofania
    setMotorSpeeds(baseSpeed * (1 + turn), baseSpeed);
  } else {
    // Cofanie prosto
    setMotorSpeeds(baseSpeed, baseSpeed);
  }
}

void turnInPlace(float turnValue) {
  forward(); // Kierunek silników do obrotu w miejscu
  
  if (turnValue > 0) {
    // Obrót w prawo - lewy silnik do przodu, prawy stop
    setMotorSpeeds(0, (MAX_MOTOR_POWER * 255) * turnValue);
  } else {
    // Obrót w lewo - prawy silnik do przodu, lewy stop
    setMotorSpeeds((MAX_MOTOR_POWER * 255) * (-turnValue), 0);
  }
}

void stopMotors() {
  setMotorSpeeds(0, 0);
}

// Funkcja do obliczania płynnego sterowania wektorowego
void vectorDriveMotors(float xVal, float yVal) {
  // Obliczenie promienia (siły) bez sqrt - używamy magnitude^2 dla porównania
  float magnitudeSquared = xVal * xVal + yVal * yVal;
  
  // Definicja martwej strefy (squared dla porównania)
  const float deadZoneSquared = 0.15 * 0.15;
  
  if (magnitudeSquared < deadZoneSquared) {
    // Joystick w martwej strefie - stop
    stopMotors();
    return;
  }
  
  // Obliczenie rzeczywistej magnitude
  float magnitude = magnitudeSquared; // Aproksymacja dla uproszczenia
  if (magnitude > 1.0) magnitude = 1.0;
  
  // Obliczenie prędkości dla każdego silnika na podstawie wektora
  float leftMotorSpeed = 0;
  float rightMotorSpeed = 0;
  
  // Algorytm miksowania - płynne przejście między ruchem a obrotem
  // yVal kontroluje ruch do przodu/tyłu, xVal kontroluje skręcanie
  
  // Składowa ruchu do przodu/tyłu
  float forwardComponent = yVal;
  
  // Składowa skręcania (różnicowa prędkość kół)
  float turnComponent = xVal;
  
  // Obliczenie prędkości każdego silnika
  leftMotorSpeed = forwardComponent + turnComponent;
  rightMotorSpeed = forwardComponent - turnComponent;
  
  // Ograniczenie do zakresu -1.0 do 1.0
  if (leftMotorSpeed > 1.0) leftMotorSpeed = 1.0;
  if (leftMotorSpeed < -1.0) leftMotorSpeed = -1.0;
  if (rightMotorSpeed > 1.0) rightMotorSpeed = 1.0;
  if (rightMotorSpeed < -1.0) rightMotorSpeed = -1.0;
  
  // Konwersja na wartości PWM i ustawienie kierunku
  setMotorWithDirection(ENA, leftMotorSpeed);
  setMotorWithDirection(ENB, rightMotorSpeed);
}

// Funkcja do ustawiania silnika z kierunkiem (do przodu/tyłu)
void setMotorWithDirection(int enablePin, float speed) {
  int pwmValue = (speed >= 0) ? (speed * MAX_MOTOR_POWER * 255) : (-speed * MAX_MOTOR_POWER * 255);
  
  // Dodanie bonusów kompensacyjnych gdy silniki pracują
  if (pwmValue > 0) {
    if (enablePin == ENA) {
      pwmValue += BONUS_A;  // Bonus dla silnika A (lewy)
    } else if (enablePin == ENB) {
      pwmValue += BONUS_B;  // Bonus dla silnika B (prawy)
    }
  }
  
  // Zabezpieczenie przed przekroczeniem maksymalnego PWM po dodaniu bonusu
  if (pwmValue > 255) pwmValue = 255;
  
  if (enablePin == ENA) {
    // Lewy silnik (IN1, IN2)
    if (speed > 0) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    } else if (speed < 0) {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    } else {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
    }
  } else if (enablePin == ENB) {
    // Prawy silnik (IN3, IN4)
    if (speed > 0) {
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    } else if (speed < 0) {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    } else {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    }
  }
  
  analogWrite(enablePin, pwmValue);
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void reverse() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
