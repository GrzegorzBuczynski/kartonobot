#ifndef ARDUINO
#include "arrow_input.hpp"


static void runLoop(Data &data) {
    data.lastUpdate = GetTickCount();
    while (true) {

        if (escPressed()) break;
    updateFromArrows(data.input);
    updateTargets(data);
    limitPowerAxis(data.target.y);
    calculateSoftening(data);
    allocatePower(data);
    printStatus(data);
        Sleep(data.delayMs);
    }
}

int main() {

    Data data{};
    initData(data);

    printIntro();
    {
    CursorGuard hideCursor(true); // ukryj kursor na czas animacji linii statusu
    initStatus(data);
        runLoop(data);
        std::cout << std::endl; // złam linię po zakończeniu pętli
    } // kursor zostanie przywrócony

    std::cout << "Program zakonczony." << std::endl;
    return 0;
}
#endif