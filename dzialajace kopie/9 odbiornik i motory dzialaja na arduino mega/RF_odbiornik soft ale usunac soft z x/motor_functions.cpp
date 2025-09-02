#include <Arduino.h>
#include "motor_functions.h"

// Jedyna definicja globalnych prędkości kół
float vl = 0.0f;
float vr = 0.0f;


void leftWheelDirection(float speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, myMin(speed * 255 + BONUS_A, 255));
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, myMin(-speed * 255 + BONUS_A, 255));
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
    analogWrite(ENB, myMin(speed * 255 + BONUS_B, 255));
  } else if (speed < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, myMin(-speed * 255 + BONUS_B, 255));
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
}



inline float myMin(float a, float b) {
  return (a < b) ? a : b;
}

void wheelSpeed(float x, float y) {
  // vl = 0.0f;
  // vr = 0.0f;
  if (x > 0.2 || x < -0.2 || y > 0.2 || y < -0.2){
    if (x >= 0 && y >= 0){
        vl = myMin(1.0, x + y);
        vr = y-x;
    } else if (x < 0 && y >= 0){
        vl = y + x;
        vr = myMin(1.0, - x + y);
    }
    else if (x < 0 && y < 0){
        vl = - myMin(1.0, - x - y);
        vr = y - x;
    } else if (x >= 0 && y < 0){
        vl = y + x;
        vr = - myMin(1.0, x - y);
    }
  }
  Serial.print("x: " + String(x) + ", y: " + String(y) + ", ");
  Serial.println("vr: " + String(vr) + ", vl: " + String(vl));
}

void vectorDriveMotors2(){
  leftWheelDirection(vl);
  rightWheelDirection(vr);
}