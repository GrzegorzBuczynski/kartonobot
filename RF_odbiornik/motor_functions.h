#ifndef MOTOR_FUNCTIONS_H
#define MOTOR_FUNCTIONS_H


#define AX  0.8f
#define MAXPOW 0.3f
#define DZ 0.2f


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

struct State {
    float x;
    float y;
};

// Deklaracje funkcji
void updateTargets(State input, State target);
void vectorDriveMotors2(float xSet, float ySet);
float wheelSpeed(float x, float y);
inline float myMin(float a, float b);
float trimMAXPOW(float value);
void runMotorControl(State& input, State& target, State& current, State& power, unsigned long lastUpdate);

extern float vl;
extern float vr;

#endif
