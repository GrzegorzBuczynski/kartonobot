#ifndef ARDUINO
#include "console.hpp"

// Funkcja do ustawienia kursora na danej pozycji (pozostawiona, ale nieużywana)
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Ukrywanie kursora konsoli podczas odświeżania statusu (RAII)
#endif
