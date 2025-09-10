#ifndef ARDUINO
#include "printing.hpp"
#include <iostream>
#include <iomanip>

void printIntro() {
    std::cout << "Uzyj strzalek, aby zmienic wartosci X i Y. Nacisnij ESC, aby wyjsc." << std::endl;
    std::cout << std::fixed << std::setprecision(1);
}

void printStatus(const Data& data) {
    std::cout << '\r'
              << "Input: X: " << std::setw(4) << data.input.x << " Y: " << std::setw(4) << data.input.y << " | "
              << "Target: X: " << std::setw(4) << data.target.x << " Y: " << std::setw(4) << data.target.y << " | "
              << "Current: X: " << std::setw(4) << data.current.x << " Y: " << std::setw(4) << data.current.y << " | "
              << "Power: Left X: " << std::setw(4) << data.power.x << " Right Y: " << std::setw(4) << data.power.y
              << "      "
              << std::flush;
}

void initStatus(const Data& data) {
    std::cout << "X: " << std::setw(4) << data.input.x << "  Y: " << std::setw(4) << data.input.y << std::flush;
}
#endif
