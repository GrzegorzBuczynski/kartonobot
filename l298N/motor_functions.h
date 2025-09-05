#ifndef MOTOR_FUNCTIONS_H
#define MOTOR_FUNCTIONS_H

// Deklaracje funkcji
void setMotorSpeeds(int leftSpeed, int rightSpeed);
void driveForward(float speed, float turn);
void driveReverse(float speed, float turn);
void turnInPlace(float turnValue);
void stopMotors();
void vectorDriveMotors(float xVal, float yVal);
void setMotorWithDirection(int enablePin, float speed);
void forward();
void reverse();

#endif
