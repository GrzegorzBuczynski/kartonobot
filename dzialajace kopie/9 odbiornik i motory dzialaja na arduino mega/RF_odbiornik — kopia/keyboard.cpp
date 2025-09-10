#ifndef ARDUINO
#include "keyboard.hpp"

void updateFromArrows(State& s) { // Aktualizuje stan na podstawie naciśnięć klawiszy strzałek
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)  { if (s.x > -1.0f) s.x -= INCREMENT_STEP; }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { if (s.x <  1.0f) s.x += INCREMENT_STEP; }
    if (GetAsyncKeyState(VK_UP) & 0x8000)    { if (s.y <  1.0f) s.y += INCREMENT_STEP; }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)  { if (s.y > -1.0f) s.y -= INCREMENT_STEP; }
}

bool escPressed() { // Sprawdza, czy naciśnięto klawisz ESC
    if (_kbhit()) {
        char key = _getch();
        if (key == 27) return true;
    }
    return false;
}
#endif

