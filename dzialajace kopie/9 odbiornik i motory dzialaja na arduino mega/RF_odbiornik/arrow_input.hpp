#pragma once

// PC (Windows) build with simulator (no Arduino):
// g++ arrow_input.cpp motor_functions_sim.cpp -o arrow_input.exe -mwindows
// or console subsystem:
// g++ arrow_input.cpp motor_functions_sim.cpp keyboard.cpp printing.cpp -o arrow_input2.exe -mconsole
// g++ arrow_input.cpp motor_functions.cpp keyboard.cpp structs.cpp printing.cpp -o arrow_input2.exe -mconsole


#include <iostream>
#include <conio.h> // Do funkcji _getch()
#include <iomanip> // Do formatowania wyjścia
#include <windows.h> // Do funkcji GetAsyncKeyState
#include "console.hpp"
#include "keyboard.hpp"
#include "printing.hpp"
#include "motor_functions.hpp"
#include "structs.hpp"


