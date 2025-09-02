#include <Arduino.h>
#include "motor_functions.h"

// Jedyna definicja globalnych prędkości kół
float vl = 0.0f;
float vr = 0.0f;


void leftWheelDirection(float speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  analogWrite(ENA, myMin(speed * 255 + BONUS_A_FWD, 255));
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  analogWrite(ENA, myMin(-speed * 255 + BONUS_A_REV, 255));
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

void rightWheelDirection(float speed) {
  if (speed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  analogWrite(ENB, myMin(speed * 255 + BONUS_B_FWD, 255));
  } else if (speed < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  analogWrite(ENB, myMin(-speed * 255 + BONUS_B_REV, 255));
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
}


#define FX 0.2f 
inline float myMin(float a, float b) {
  return (a < b) ? a : b;
}

void wheelSpeed(float x, float y) {
  if (x > 0.2 || x < -0.2 || y > 0.2 || y < -0.2){
    // Płynne odwracanie skrętu wokół y=0, aby uniknąć szarpnięcia przy zmianie kierunku
    float xScaled = x * FX;

    // Wyznacz współczynnik miksu skrętu: +1 dla FWD, -1 dla REV, liniowo w oknie wokół 0
    float w = TURN_BLEND_WIDTH;
    float turnMult;
    if (y >= w) {
      turnMult = 1.0f;
    } else if (y <= -w) {
      turnMult = -1.0f;
    } else {
      // liniowe przejście w zakresie (-w, w)
      turnMult = y / w; // w -> +1, -w -> -1, 0 -> 0 (brak nagłej zmiany kierunku)
    }

    float turn = xScaled * turnMult;

    float left = y + turn;
    float right = y - turn;

    // Normalizacja, jeśli jedna z wartości przekracza zakres [-1, 1]
    float absL = (left < 0.0f) ? -left : left;
    float absR = (right < 0.0f) ? -right : right;
    float maxMag = (absL > absR) ? absL : absR;
    if (maxMag > 1.0f) {
      left /= maxMag;
      right /= maxMag;
    }

    vl = left;
    vr = right;
  }
  Serial.print("x: " + String(x) + ", y: " + String(y) + ", ");
  Serial.println("vr: " + String(vr) + ", vl: " + String(vl));
}

void vectorDriveMotors2(){
  leftWheelDirection(vl);
  rightWheelDirection(vr);
}