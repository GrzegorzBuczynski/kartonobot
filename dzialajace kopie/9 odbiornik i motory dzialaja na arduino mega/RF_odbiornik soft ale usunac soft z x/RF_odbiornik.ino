#include <RH_ASK.h>
#include <SPI.h>
#include "reciver.hpp"
#include "parser.hpp"
#include "motor_functions.h"

// Pin odbiornika RF
#define RX_PIN 11  // Pin odbioru RF



// Uwaga: RadioHead ASK używa Timer1 na UNO – PWM na pinach 9/10 jest niedostępne.
// Przenieśliśmy ENA z 10 na 6, a IN4 z 6 na 4, by uniknąć konfliktu i pulsowania silników.
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
        // Serial.print("Odebrano (");
        // Serial.print(buflen);
        // Serial.print(" bajtów): ");
        // Serial.println((char*)buf);
        
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
            Serial.println("Parse error");           
        }
    }
    // Inicjalizacja timera przy pierwszym uruchomieniu
    if (now - timer > 500) {
        xSet = ySet = 0.0f;
        vl = vr = 0.0f;
        timer = now;
        // Serial.println("Reset");
    }
    // Reciver::handlePeriodicLog(xSet, ySet, sSet, now);
    wheelSpeed(xSet, ySet);
    vectorDriveMotors2();
}
