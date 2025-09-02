#include <RH_ASK.h>
#include <SPI.h>
#include "reciver.hpp"
#include "parser.hpp"
#include "motor_functions.h"

// Motor Connections (ENA & ENB must use PWM pins)
#define IN1 9
#define IN2 8
#define IN3 7
#define IN4 6
#define ENA 10
#define ENB 5

// Parametr maksymalnej mocy silników (0.0-1.0)
// Zmniejsz tę wartość aby ograniczyć maksymalną prędkość
// #define MAX_MOTOR_POWER 1.0  // Zakres: 0.0-1.0 (1.0 = pełna moc, 0.5 = połowa mocy)
// #define MAX_MOTOR_POWER 0.8   // 80% mocy  
// #define MAX_MOTOR_POWER 0.6   // 60% mocy
// #define MAX_MOTOR_POWER 0.5   // 50% mocy (połowa prędkości)
#define MAX_MOTOR_POWER 0.3   // 30% mocy (tryb powolny)
// #define MAX_MOTOR_POWER 0.1   // 10% mocy (tryb bardzo powolny)

// Pin odbiornika RF
#define RX_PIN 11  // Pin odbioru RF

// Kompensacja różnic w charakterystykach silników
// Wartość 0 = brak kompensacji, wartość 50 = dodanie 50 do PWM
#define BONUS_A 0    // Kompensacja dla silnika A (lewy - ENA)
#define BONUS_B 20   // Kompensacja dla silnika B (prawy - ENB)

// Odbiornik RF - prędkość 2000 bps, RX pin 11, TX pin nie używany (255), PTT pin nie używany (255)
RH_ASK driver(2000, RX_PIN, 255, 255);

// Parser przeniesiony do parser.cpp
unsigned long timer;

void setup() {
    Serial.begin(9600);
    if (!driver.init()) {
        Serial.println("init failed");
    }
    Serial.println("Odbiornik RF gotowy...");
    timer = millis();
        Serial.println("L298N Motor Controller - Start");
    // Set motor connections as outputs
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    // Start with motors off
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

float xVal, yVal;
int sVal;

float xSet, ySet;
int sSet;
unsigned long now;

void loop() {
    now = millis();
    // Zwiększony bufor – pełna wiadomość może mieć ~34 bajty, więc dajemy zapas
    uint8_t buf[64];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) { // dane odebrane
        // Zapewnij zakończenie NUL nawet przy pełnym buforze
        if (buflen >= sizeof(buf)) {
            buflen = sizeof(buf) - 1;
        }
        buf[buflen] = '\0';          // zakończenie stringa
        
        // Wyświetl surową wiadomość z informacją o długości
        Serial.print("Odebrano (");
        Serial.print(buflen);
        Serial.print(" bajtów): ");
        Serial.println((char*)buf);
        
        // Sparsuj dane
        
        if (parseMessage((char*)buf, &xVal, &yVal, &sVal)) {
            if (xVal > 0.2 || xVal < -0.2 || yVal > 0.2 || yVal < -0.2){
                xSet = xVal;
                ySet = yVal;
                // Użyj tego samego znacznika czasu co w tej iteracji pętli,
                // aby uniknąć underflow (now - timer) w tym samym obiegu
                timer = now;
                // Serial.println("in range");
            }
        } else {
            Serial.println("Błąd parsowania wiadomości!");           
        }
    }
    // Inicjalizacja timera przy pierwszym uruchomieniu
    if (now - timer > 300) {
        xSet = ySet = 0.0f;
        timer = now;
        // Serial.println("Reset");
    }
    Reciver::handlePeriodicLog(xSet, ySet, sSet, now);
    // vectorDriveMotors(xSet, ySet);
}
