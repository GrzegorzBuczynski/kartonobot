#include "motor_functions.hpp"

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
  if (data.input.y > DZ || data.input.y < -DZ) {
    data.target.y = data.input.y;
    data.target.x = data.input.x;
    data.lastTargetUpdate = GetTickCount();
  }
  else if (data.input.x > DZ || data.input.x < -DZ) {
    data.target.x = data.input.x;
    data.lastTargetUpdate = GetTickCount();
  }
  if (GetTickCount() - data.lastTargetUpdate > MAX_NO_UPDATE_INTERVAL) {
    data.target.x = 0.0f;
    data.target.y = 0.0f;
  }
}

// PC: simple easing and power proxy
void calculateSoftening(Data &data) {
  static constexpr float MAX_DELTA_X = 0.6f;  // Increased for faster x response
  // for x
  // float delta_x = data.target.x - data.current.x;
  // if (delta_x > MAX_DELTA_X) data.current.x += MAX_DELTA_X;
  // else if (delta_x < -MAX_DELTA_X) data.current.x -= MAX_DELTA_X;
  // else 
  data.current.x = data.target.x;
  // for y - less reactive at higher speeds
  float max_delta_y = 0.2f / (1.0f + abs(data.current.y) * 2.0f);  // Increased base
  float delta_y = data.target.y - data.current.y;
  if (delta_y > max_delta_y) data.current.y += max_delta_y;
  else if (delta_y < -max_delta_y) data.current.y -= max_delta_y;
  else data.current.y = data.target.y;
}

float wheelPowerLeft(float x, float y) {
  float z; 
  if (x == 0) x = 0.0;
  if (x >=  0){
    if (y >= 0) // Forward-Left
      z = y - x * TURNING_FACTOR_LEFT;
    else
      z = y + x * TURNING_FACTOR_LEFT;
  }
  else {
    z = y;
  }
  if (z == 0) z = 0.0;
  return z;
}

float wheelPowerRight(float x, float y) {
  float z; 
  if (x == 0) x = 0.0;
  if (x >=  0){
    if (y >= 0) // Forward-Right
      z = y - x * TURNING_FACTOR_RIGHT;
    else
      z = y + x * TURNING_FACTOR_RIGHT;
  }
  else {
    z = y;
  }
  if (z == 0) z = 0.0;
  return z;
}

// Common for both builds
void limitPowerAxis(float &value) {
  if (value > MAXPOW) value = MAXPOW;
  if (value < -MAXPOW) value = -MAXPOW;
}

void allocatePower(Data &data){
    data.power.x = wheelPowerLeft(-data.current.x, data.current.y);
    data.power.y = wheelPowerRight(data.current.x, data.current.y);
    // data.power.x = data.current.y - data.current.x * TURNING_FACTOR_LEFT;  // left
    // data.power.y = (data.current.y + data.current.x * TURNING_FACTOR_RIGHT);  // right, negated for reversed motor
    limitPowerAxis(data.power.x);
    limitPowerAxis(data.power.y);
}