#include <RH_ASK.h>
#include <SPI.h>

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

void loop() {
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
        float xVal, yVal;
        int sVal;
        
        if (parseMessage((char*)buf, &xVal, &yVal, &sVal)) {
            Serial.println("--- Sparsowane dane ---");
            Serial.print("X: ");
            Serial.print(xVal, 2);
            Serial.print(" | Y: ");
            Serial.print(yVal, 2);
            Serial.print(" | SW: ");
            Serial.println(sVal);
            
            // Tutaj możesz dodać logikę sterowania na podstawie odebranych wartości
            // Przykład interpretacji:
            if (xVal > 0.5) {
                Serial.println("Ruch w prawo");
            } else if (xVal < -0.5) {
                Serial.println("Ruch w lewo");
            }
            
            if (yVal > 0.5) {
                Serial.println("Ruch do przodu");
            } else if (yVal < -0.5) {
                Serial.println("Ruch do tyłu");
            }
            
            if (sVal == 0) {
                Serial.println("Przycisk wciśnięty!");
            }
            
            Serial.println("----------------------");
        } else {
            Serial.println("Błąd parsowania wiadomości!");
        }
    }
}
