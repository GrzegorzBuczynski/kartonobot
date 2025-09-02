#include <RH_ASK.h>
#include <SPI.h>
#include "reciver.hpp"
#include "parser.hpp"
#include "motor_functions.h"

// Pin odbiornika RF
#define RX_PIN 11  // Pin odbioru RF
#define MIN_UPDATE_INTERVAL 100
#define DZ 0.02f
#define DX 0.01f
#define DY 0.01f



// Uwaga: RadioHead ASK używa Timer1 na UNO – PWM na pinach 9/10 jest niedostępne.
// Przenieśliśmy ENA z 10 na 6, a IN4 z 6 na 4, by uniknąć konfliktu i pulsowania silników.
// Odbiornik RF - prędkość 2000 bps, RX pin 11, TX pin nie używany (255), PTT pin nie używany (255)
RH_ASK driver(2000, RX_PIN, 255, 255);

// Parser przeniesiony do parser.cpp
unsigned long timer;

float x, y;
float targetX, targetY;
float xVal, yVal;
int sVal;
int sSet;
unsigned long now;
float xSet, ySet;

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
    x = y = 0.0f;
    targetX = targetY = 0.0f;
}

// Ramping state/params for ySet (forward/backward)
float yTarget = 0.0f;
bool rampInitPending = false;       // whether to apply initial jump to threshold
unsigned long lastRampMs = 0;       // last time we changed ySet

// Tunables (you can adjust live and re-upload)
float rampInitialPct = 0.20f;       // initial jump to 20% of max (normalized 0..1)
float rampStepPctFwd = 0.10f;       // per-step increase as % of target (forward)
float rampStepPctRev = 0.05f;       // per-step increase as % of target (reverse)
unsigned long rampIntervalFwdMs = 100; // period between steps forward
unsigned long rampIntervalRevMs = 200; // period between steps reverse
unsigned long lastXYUpdate = 0;
int ax = 0;
int ay = 0;

void loop() {

    now = millis();
    // Zwiększony bufor – pełna wiadomość może mieć ~34 bajty, więc dajemy zapas
    uint8_t buf[64];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) { // dane odebrane
        if (buflen >= sizeof(buf)) {
            buflen = sizeof(buf) - 1;
        }
        buf[buflen] = '\0';          // zakończenie stringa
        updateTargets((char*)buf);
    }
    updateXY();
    vectorDriveMotors2(x, y);
}
