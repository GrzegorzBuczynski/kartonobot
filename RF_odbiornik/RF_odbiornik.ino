#include <RH_ASK.h>
#include <SPI.h>
#include "reciver.hpp"
#include "parser.hpp"
#include "motor_functions.hpp"
#include "structs.hpp"

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
Data data{};


void setup() {
    Serial.begin(9600);
    if (!driver.init()) {
        Serial.println("init failed");
    }
    Serial.println("Odbiornik RF gotowy...");
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
    initData(data);
}



void loop() {

    // Zwiększony bufor – pełna wiadomość może mieć ~34 bajty, więc dajemy zapas
    uint8_t buf[64];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) { // dane odebrane
        if (buflen >= sizeof(buf)) {
            buflen = sizeof(buf) - 1;
        }
        buf[buflen] = '\0';          // zakończenie stringa
        parseMessage((char*)buf, data.input.x, data.input.y, data.sw);
    }
    updateTargets(data);
    limitPowerAxis(data.target.y);
    calculateSoftening(data);
    allocatePower(data);
    powerWheals(data.power.x, data.power.y);
}
