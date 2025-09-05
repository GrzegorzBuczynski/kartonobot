#include "reciver.hpp"
#include "parser.hpp"
#include <Arduino.h>

namespace Reciver {

// Konfiguracja
static const unsigned long CONTROL_TIMEOUT_MS = 500;
static const unsigned long PRINT_INTERVAL_MS = 200;

// Stan sterowania + czas
static float xVal = 0.0f;
static float yVal = 0.0f;
static bool sVal = true;
static unsigned long lastUpdateMs = 0;
static unsigned long lastPrintMs = 0;
static bool inFailsafe = true;



void init(unsigned long now) {
    lastUpdateMs = now;
    lastPrintMs = now;
    inFailsafe = true;
    xVal = 0.0f; yVal = 0.0f; sVal = 1;
}

void handleReceive(RH_ASK& driver) {
    uint8_t buf[64];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) {
        if (buflen >= sizeof(buf)) buflen = sizeof(buf) - 1;
        buf[buflen] = '\0';
        float xTmp, yTmp; bool sTmp;
        if (parseMessage((char*)buf, xTmp, yTmp, sTmp)) {
            xVal = xTmp;
            yVal = yTmp;
            sVal = sTmp;
            lastUpdateMs = millis();
            if (inFailsafe) inFailsafe = false;
        }
    }
}

void handleFailsafe(unsigned long now) {
    if (!inFailsafe && (now - lastUpdateMs > CONTROL_TIMEOUT_MS)) {
        xVal = 0.0f; yVal = 0.0f; sVal = 1; inFailsafe = true;
    }
}

void handlePeriodicLog(float xSet, float ySet, int sSet, unsigned long now) {
    if (now - lastPrintMs >= PRINT_INTERVAL_MS) {
        Serial.print("X: "); Serial.print(xSet, 2);
        Serial.print(" Y: "); Serial.print(ySet, 2);
        Serial.print(" SW: "); Serial.println(sSet);
        lastPrintMs = now;
    }
}

} // namespace Reciver
