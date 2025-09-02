#include <Arduino.h>
#include "motor_functions.h"

// Jedyna definicja globalnych prędkości kół
float vl = 0.0f;
float vr = 0.0f;



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



void vheelSpeed(float x, float y) {
  vl = 0.0f;
  vr = 0.0f;
  if (x > 0.2 || x < -0.2 || y > 0.2 || y < -0.2){
    if (x > 0 && y > 0){
        vl = y;
        vr = y - x;
    } else if (x < 0 && y > 0){
        vl = y + x;
        vr = y;
    } else if (x < 0 && y < 0){
        vl = y - x;
        vr = y;
    } else if (x > 0 && y < 0){
        vl = y;
        vr = y + x;
    } else if (x == 0 && y > 0){
        vl = y;
        vr = y;
    } else if (x == 0 && y < 0){
        vl = y;
        vr = y;
    } else if (y == 0 && x > 0){
        vl = x;
        vr = -x;
    } else if (y == 0 && x < 0){
        vl = -x;
        vr = x;
    }  
  }
  Serial.println("vr: " + String(vr) + ", vl: " + String(vl));
}

void vectorDriveMotors2(){

  if (vr > 0 && vl > 0){
    forward();
    analogWrite(ENA, vl * 255);
    analogWrite(ENB, vr * 255);
  } else if (vr < 0 && vl < 0){
    reverse();
    analogWrite(ENA, -vl * 255);
    analogWrite(ENB, -vr * 255);
  } else if (vr > 0 && vl < 0){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, -vl * 255);
    analogWrite(ENB, vr * 255);
  } else if (vr < 0 && vl > 0){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, vl * 255);
    analogWrite(ENB, -vr * 255);
  } else {
    analogWrite(ENA,0.0f);
    analogWrite(ENB, 0.0f);
  }
}