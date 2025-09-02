#include "reciver.hpp"
#include <Arduino.h>

namespace Reciver {

// Konfiguracja
static const unsigned long CONTROL_TIMEOUT_MS = 500;
static const unsigned long PRINT_INTERVAL_MS = 200;

// Stan sterowania + czas
static float xVal = 0.0f;
static float yVal = 0.0f;
static int sVal = 1;
static unsigned long lastUpdateMs = 0;
static unsigned long lastPrintMs = 0;
static bool inFailsafe = true;

// Parsowanie wiadomości "Nadajnik 591 X:<f> Y:<f> SW:<d>"
static bool parseMessage(const char* message, float* x, float* y, int* sw) {
    *x = 0.0f; *y = 0.0f; *sw = 1;
    if (strncmp(message, "Nadajnik 591", 12) != 0) {
        return false;
    }

    const char* px = strstr(message, "X:");
    const char* py = strstr(message, "Y:");
    const char* psw = strstr(message, "SW:");

    auto readToken = [](const char* start, float* outFloat) -> bool {
        if (!start) return false;
        start += 2;
        char tmp[16];
        size_t i = 0;
        while (*start && *start != ' ' && i < sizeof(tmp) - 1) tmp[i++] = *start++;
        tmp[i] = '\0';
        if (i == 0) return false;
        *outFloat = atof(tmp);
        return true;
    };

    auto readTokenInt = [](const char* start, int* outInt) -> bool {
        if (!start) return false;
        start += 3;
        char tmp[8];
        size_t i = 0;
        while (*start && *start != ' ' && i < sizeof(tmp) - 1) tmp[i++] = *start++;
        tmp[i] = '\0';
        if (i == 0) return false;
        *outInt = atoi(tmp);
        return true;
    };

    bool ok = false;
    if (readToken(px, x)) ok = true;
    if (readToken(py, y)) ok = true;
    int swTmp;
    if (readTokenInt(psw, &swTmp)) { *sw = swTmp; ok = true; }
    return ok;
}

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
        float xTmp, yTmp; int sTmp;
        if (parseMessage((char*)buf, &xTmp, &yTmp, &sTmp)) {
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
