#include "parser.hpp"
#include <Arduino.h>

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
