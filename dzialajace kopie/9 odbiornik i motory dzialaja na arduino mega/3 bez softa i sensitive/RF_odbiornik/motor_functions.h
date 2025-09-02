#ifndef MOTOR_FUNCTIONS_H
#define MOTOR_FUNCTIONS_H



// // Motor Connections (ENA & ENB must use PWM pins)
// #define IN1 A0
// #define IN2 A1
// #define IN3 A2
// // Przeniesione, aby uniknąć kolizji z Timer1 (RadioHead ASK) – IN4 na pin 4
// #define IN4 A3
// // ENA z pinu 10 (Timer1 PWM) na 6 (Timer0 PWM) – stabilny PWM podczas ASK
// #define ENA A4
// #define ENB A5


// Motor Connections (ENA & ENB must use PWM pins)
#define IN1 3
#define IN2 4
#define IN3 7
#define IN4 8
// ENA z pinu 10 (Timer1 PWM) na 6 (Timer0 PWM) – stabilny PWM podczas ASK
#define ENA 5
#define ENB 6

// Kompensacja różnic w charakterystykach silników
// Wartość 0 = brak kompensacji, wartość 50 = dodanie 50 do PWM
#define BONUS_A 0    // Kompensacja dla silnika A (lewy - ENA)
#define BONUS_B 15   // Kompensacja dla silnika B (prawy - ENB)

// Parametr maksymalnej mocy silników (0.0-1.0)
// Zmniejsz tę wartość aby ograniczyć maksymalną prędkość
// #define MAX_MOTOR_POWER 1.0  // Zakres: 0.0-1.0 (1.0 = pełna moc, 0.5 = połowa mocy)
// #define MAX_MOTOR_POWER 0.8   // 80% mocy  
// #define MAX_MOTOR_POWER 0.6   // 60% mocy
// #define MAX_MOTOR_POWER 0.5   // 50% mocy (połowa prędkości)
#define MAX_MOTOR_POWER 0.3   // 30% mocy (tryb powolny)
// #define MAX_MOTOR_POWER 0.1   // 10% mocy (tryb bardzo powolny)

// Deklaracje funkcji
void vectorDriveMotors2();
void wheelSpeed(float x, float y);
inline float myMin(float a, float b);

extern float vl;
extern float vr;

#endif
