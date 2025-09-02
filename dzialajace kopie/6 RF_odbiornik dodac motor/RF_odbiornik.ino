#include <RH_ASK.h>
#include <SPI.h>
#include "reciver.hpp"

// Pin odbiornika RF
#define RX_PIN 11  // Pin odbioru RF

// Odbiornik RF - prędkość 2000 bps, RX pin 11, TX pin nie używany (255), PTT pin nie używany (255)
RH_ASK driver(2000, RX_PIN, 255, 255);

// Funkcja do parsowania odebranych danych
// Bezpieczna względem braku wsparcia %f w sscanf na AVR – używa prostego tokenizera
bool parseMessage(const char* message, float* x, float* y, int* sw) {
    // Domyślne wartości
    *x = 0.0f;
    *y = 0.0f;
    *sw = 1; // 1 = nie wciśnięty (typowy joystick z pull-up)

    // Sprawdź nagłówek
    if (strncmp(message, "Nadajnik 591", 12) != 0) {
        return false;
    }

    // Znajdź pola X, Y, SW w treści
    const char* px = strstr(message, "X:");
    const char* py = strstr(message, "Y:");
    const char* psw = strstr(message, "SW:");

    bool ok = false;

    // Helper do skopiowania tokena do tymczasowego bufora i konwersji
    auto readToken = [](const char* start, float* outFloat) -> bool {
        if (!start) return false;
        start += 2; // pominąć "X:"/"Y:"
        char tmp[16];
        size_t i = 0;
        while (*start && *start != ' ' && i < sizeof(tmp) - 1) {
            tmp[i++] = *start++;
        }
        tmp[i] = '\0';
        if (i == 0) return false;
        *outFloat = atof(tmp);
        return true;
    };

    auto readTokenInt = [](const char* start, int* outInt) -> bool {
        if (!start) return false;
        start += 3; // pominąć "SW:"
        char tmp[8];
        size_t i = 0;
        while (*start && *start != ' ' && i < sizeof(tmp) - 1) {
            tmp[i++] = *start++;
        }
        tmp[i] = '\0';
        if (i == 0) return false;
        *outInt = atoi(tmp);
        return true;
    };

    if (readToken(px, x)) ok = true;
    if (readToken(py, y)) ok = true;
    // SW jest opcjonalne – jeśli brak, pozostaje 1
    int swTmp;
    if (readTokenInt(psw, &swTmp)) {
        *sw = swTmp;
        ok = true;
    }

    return ok;
}

void setup() {
    Serial.begin(9600);
    if (!driver.init()) {
        Serial.println("init failed");
    }
    Serial.println("Odbiornik RF gotowy...");
}
float xVal, yVal;
int sVal;

float xSet, ySet;
int sSet;
unsigned long now;
unsigned long timer;

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
    if (timer == 0) timer = now;
    if (now - timer > 300) {
        xSet = ySet = 0.0f;
        timer = now;
        // Serial.println("Reset");
    }
    Reciver::handlePeriodicLog(xSet, ySet, sSet, now);
}
