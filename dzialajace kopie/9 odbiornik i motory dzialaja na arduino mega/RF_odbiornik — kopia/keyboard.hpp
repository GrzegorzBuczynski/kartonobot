#pragma once

#include "structs.hpp"
#define INCREMENT_STEP 0.1f
#ifndef ARDUINO
#include <conio.h> // Do funkcji _getch()
#include <iomanip> // Do formatowania wyjścia
#include <windows.h> // Do funkcji GetAsyncKeyState
#endif


// Deklaracje (definicje w keyboard.cpp)
bool escPressed();
void updateFromArrows(State& s);
