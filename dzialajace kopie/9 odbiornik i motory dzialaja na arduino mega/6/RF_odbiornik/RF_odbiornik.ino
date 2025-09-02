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
// Ramping state/params for ySet (forward/backward)
float yTarget = 0.0f;
bool rampInitPending = false;       // whether to apply initial jump to threshold
unsigned long lastRampMs = 0;       // last time we changed ySet

// Tunables (you can adjust live and re-upload)
float rampInitialPct = 0.20f;       // initial jump to 20% of max (normalized 0..1)
float rampStepPctFwd = 0.10f;       // per-step increase as % of target (forward)
float rampStepPctRev = 0.05f;       // per-step increase as % of target (reverse)
unsigned long rampIntervalFwdMs = 10; // period between steps forward
unsigned long rampIntervalRevMs = 20; // period between steps reverse

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
            if (xVal > 0.1 || xVal < -0.1 || yVal > 0.2 || yVal < -0.2) {
                // ySet = yVal;
                xSet = 0.8 * xVal;

                // Update target and mark ramp initialization when direction or magnitude changes
                float prevTarget = yTarget;
                yTarget = yVal;
                // Trigger initial jump when direction changes or when we newly want movement
                if ((prevTarget == 0.0f && (yTarget > 0.0f || yTarget < 0.0f)) ||
                    (prevTarget > 0.0f && yTarget < 0.0f) ||
                    (prevTarget < 0.0f && yTarget > 0.0f)) {
                    rampInitPending = true;
                }

                // Reset ramp timer to make first step responsive to fresh command
                lastRampMs = now;

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
        xSet = 0.0f;
        yTarget = 0.0f;
        ySet = 0.0f;
        rampInitPending = false;
        lastRampMs = now;
        vl = vr = 0.0f;
        timer = now;
        // Serial.println("Reset");
    }

    // Ramping logic for ySet towards yTarget
    auto signf = [](float v) -> float { return (v > 0.0f) - (v < 0.0f); };
    float tgt = yTarget;
    float dir = signf(tgt);
    unsigned long interval = (dir >= 0.0f) ? rampIntervalFwdMs : rampIntervalRevMs;
    float stepPct = (dir >= 0.0f) ? rampStepPctFwd : rampStepPctRev;

    // Apply initial jump to threshold (but don't exceed target)
    if (rampInitPending && dir != 0.0f) {
        float initialMag = rampInitialPct;
        float desiredMag = (tgt < 0.0f) ? -tgt : tgt;
        float jumpMag = (desiredMag < initialMag) ? desiredMag : initialMag;
        ySet = dir * jumpMag;
        rampInitPending = false;
        lastRampMs = now;
    }

    // Step towards target at configured intervals
    if (now - lastRampMs >= interval) {
        float err = tgt - ySet;
        float errDir = signf(err);
        if (errDir != 0.0f) {
            float stepMag = stepPct * ((tgt < 0.0f) ? -tgt : tgt);
            // If target is zero, step relative to threshold to still move
            if (tgt == 0.0f) stepMag = stepPct * rampInitialPct;

            float next = ySet + errDir * stepMag;
            // Clamp to not overshoot target
            if ((errDir > 0.0f && next > tgt) || (errDir < 0.0f && next < tgt)) {
                next = tgt;
            }
            ySet = next;
        }
        lastRampMs = now;
    }

    // Reciver::handlePeriodicLog(xSet, ySet, sSet, now);
    wheelSpeed(xSet, ySet);
    vectorDriveMotors2();
}
