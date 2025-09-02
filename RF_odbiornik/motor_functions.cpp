#include <Arduino.h>
#include "motor_functions.h"

// Jedyna definicja globalnych prędkości kół
float vl = 0.0f;
float vr = 0.0f;



void driveLeftWeel(float speed) {
  if (speed > DZ) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  analogWrite(ENA, myMin(speed * 255 + BONUS_A_FWD, 255));
  } else if (speed < -DZ) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  analogWrite(ENA, myMin(-speed * 255 + BONUS_A_REV, 255));
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

void driveRightWeel(float speed) {
  if (speed > DZ) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  analogWrite(ENB, myMin(speed * 255 + BONUS_B_FWD, 255));
  } else if (speed < -DZ) {
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

float trimMAXPOW(float value) {
  float speed = value;
  if (speed > DZ) {
      speed = DZ + (speed - DZ) * MAXPOW;
  } else if (speed < -DZ) {
      speed = -DZ + (speed + DZ) * MAXPOW;
  } else {
      speed = 0.0f;
  }
  return speed;
}

float wheelSpeed(float x, float y) {
  float speed = 0.0f;
  if (x >= 0 && y >= 0)
  {
    speed = AX * (x + y - 1) + (-x + 0.5 * y + 0.5);
  }
  if (x >= 0 && y < 0) {
    speed = (1 - AX) * x + (AX - 0.5) * y + (AX - 1.5);
  } else {
    speed = y;
  }

  return speed;
}



void vectorDriveMotors2(float x, float y){
  
  driveRightWeel(wheelSpeed(x, y));
  driveLeftWeel(wheelSpeed(-x, y));
  
  // Serial.println("vr: " + String(wheelSpeed(x, y)) + ", vl: " + String(wheelSpeed(-x, y)));
  // driveRightWeel(0);
  // driveLeftWeel(0);
}

void parseMessage(char* buf) {
    if (parseMessage((char*)buf, &xVal, &yVal, &sVal)) {
      updateTargets();
    } else {
        // Serial.println("Parse error");
    }
}

unsigned long GetTick() {
  if (defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM))
    return millis();
  return GetTickCount();
}

void updateTargets(State input, State& target, unsigned long& lastUpdate) {
  if (input.x > DZ || input.x < -DZ)
    {target.x = input.x;
    lastUpdate = GetTick();}
  if (input.y > DZ || input.y < -DZ)
    {target.y = input.y;
    lastUpdate = GetTick();}
}

void updateXY() {
    if (now - lastXYUpdate > MIN_UPDATE_INTERVAL) {
        lastXYUpdate = now;
        if (targetX > x){
            ax += 1;
            if (ax < 0)
                x = -DZ + (ax + 1) * DX;
            else if (ax == 0)
                x = 0.0f;
            else
                x = DZ + (ax - 1) * DX;
        }
        if (targetX < x){
            ax -= 1;
            if (ax < 0)
                x = -DZ + (ax + 1) * DX;
            else if (ax == 0)
                x = 0.0f;
            else
                x = DZ + (ax - 1) * DX;
        }
        if (targetY > y){
            ay += 1;
            if (ay < 0)
                y = -DZ + (ay + 1) * DY;
            else if (ay == 0)
                y = 0.0f;
            else
                y = DZ + (ay - 1) * DY;
        }
        if (targetY < y) {
            ay -= 1;
            if (ay < 0)
                y = -DZ + (ay + 1) * DY;
            else if (ay == 0)
                y = 0.0f;
            else
                y = DZ + (ay - 1) * DY;
        }
        if (x > 1.0f)
            x = 1.0f;
        if (x < -1.0f)
            x = -1.0f;
        if (y > 1.0f)
            y = 1.0f;
        if (y < -1.0f)
            y = -1.0f;
    }
}


void runMotorControl(State& input, State& target, State& current, State& power, unsigned long lastUpdate) {
    updateTargets(input, target, lastUpdate);
    updateXY(current, target);
    updateMotorPower(current, power);
    applyMotorControl(power);
}