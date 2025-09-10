#include "motor_functions.hpp"

float my_abs(float x) {
    return x < 0.0f ? -x : x;
}

#ifdef ARDUINO
#include <Arduino.h>

void powerWheals(float leftSpeed, float rightSpeed) {
  driveLeftWeel(leftSpeed);
  driveRightWeel(rightSpeed);
}

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

unsigned long GetTickCount() {
  return millis();
}

#endif // ARDUINO

// PC: update targets based on input and reset to 0 after inactivity
void updateTargets(Data &data) {
  if (data.input.y > DZ || data.input.y < -DZ || data.input.x > DZ || data.input.x < -DZ) {
    data.target.y = data.input.y;
    data.target.x = data.input.x;
    data.lastTargetUpdate = GetTickCount();
  }
  if (GetTickCount() - data.lastTargetUpdate > MAX_NO_UPDATE_INTERVAL) {
    data.target.x = 0.0f;
    data.target.y = 0.0f;
  }
}

#define MAX_DELTA 0.1f

void calculateSoftening(Data &data) {
  unsigned long now = GetTickCount();
  if (now < data.lastCurrentUpdate + 50) return; 
  float deltax = data.target.x - data.current.x;
  if (deltax > MAX_DELTA) data.current.x += MAX_DELTA;
  else if (deltax < -MAX_DELTA) data.current.x -= MAX_DELTA;
  else data.current.x = data.target.x;

  float deltay = data.target.y - data.current.y;
  if (deltay > MAX_DELTA) data.current.y += MAX_DELTA;
  else if (deltay < -MAX_DELTA) data.current.y -= MAX_DELTA;
  else data.current.y = data.target.y;
  data.current.x = data.target.x;
  
  data.lastCurrentUpdate = now;
}

float wheelPower(float x, float y) {
  float z = 0.0f; 
  if (y >= DZ || y <= -DZ) {
    if (x >= DZ)
      z = y - (y * my_abs(x) * 0.5);
    else
      z = y;
  }
  else if (x >=  DZ || x <= -DZ) 
    z = -x * 0.5;
  return z;
}



// Common for both builds
void limitPowerAxis(float &value) {
  if (value > MAXPOW) value = MAXPOW;
  if (value < -MAXPOW) value = -MAXPOW;
}

void allocatePower(Data &data){
    data.power.x = wheelPower(-data.current.x, data.current.y);
    data.power.y = wheelPower(data.current.x, data.current.y);
    limitPowerAxis(data.power.x);
    limitPowerAxis(data.power.y);
}