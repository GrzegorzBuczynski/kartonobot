#ifndef MOTOR_FUNCTIONS_H
#define MOTOR_FUNCTIONS_H

#include "structs.hpp"

#define AX  0.8f
#define MAXPOW 0.3f
#define DZ 0.2f
#define MAX_NO_UPDATE_INTERVAL 500
#define myMin(a,b) ((a)<(b)?(a):(b))


// Motor Connections (ENA & ENB must use PWM pins)
#define IN1 3
#define IN2 4
#define IN3 7
#define IN4 8
// ENA z pinu 10 (Timer1 PWM) na 6 (Timer0 PWM) – stabilny PWM podczas ASK
#define ENA 5
#define ENB 6

// Kompensacja różnic w charakterystykach silników (osobno dla przód/tył)
// Wartość 0 = brak kompensacji, większa wartość = dodanie do PWM (0-255)
// Dostosuj FWD, jeśli pojazd dryfuje przy jeździe do przodu. REV pozostaw wg potrzeby.
// Domyślnie zachowujemy poprzednie 20 tylko dla prawego silnika przy jeździe wstecz.
#define BONUS_A_FWD 10  // lewy (ENA) podczas jazdy do przodu – skoryguj dryf w lewo
#define BONUS_A_REV 0   // lewy (ENA) podczas jazdy do tyłu
#define BONUS_B_FWD 0   // prawy (ENB) podczas jazdy do przodu
#define BONUS_B_REV 20  // prawy (ENB) podczas jazdy do tyłu (jak wcześniej)


#ifdef ARDUINO
void driveLeftWeel(float speed);
void driveRightWeel(float speed);
#endif

// Deklaracje funkcji
void updateTargets(Data &data);
float wheelPower(float x, float y);
void calculateSoftening(Data &data);
void limitPowerAxis(float &value);
void allocatePower(Data &data);
void powerWheals(float leftSpeed, float rightSpeed);

extern float vl;
extern float vr;

#endif
