// g++ "c:\Users\stacja\Documents\Arduino\RF_odbiornik\arrow_input.cpp" -o "c:\Users\stacja\Documents\Arduino\RF_odbiornik\arrow_input.exe"
// "c:\Users\stacja\Documents\Arduino\RF_odbiornik\arrow_input.exe"
// g++ arrow_input.cpp -o arrow_input.exe -mwindows
// g++ arrow_input.cpp -o arrow_input2.exe -mconsole


#include <iostream>
#include <conio.h> // Do funkcji _getch()
#include <iomanip> // Do formatowania wyjścia
#include <windows.h> // Do funkcji GetAsyncKeyState
#include "motor_functions.h"

// Funkcja do ustawienia kursora na danej pozycji (pozostawiona, ale nieużywana)
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


// Ukrywanie kursora konsoli podczas odświeżania statusu (RAII)
struct CursorGuard {
    HANDLE hOut{GetStdHandle(STD_OUTPUT_HANDLE)};
    CONSOLE_CURSOR_INFO original{};
    bool ok{false};
    explicit CursorGuard(bool hide) {
        ok = GetConsoleCursorInfo(hOut, &original);
        if (ok && hide) {
            CONSOLE_CURSOR_INFO tmp = original;
            tmp.bVisible = FALSE;
            SetConsoleCursorInfo(hOut, &tmp);
        }
    }
    ~CursorGuard() {
        if (ok) {
            SetConsoleCursorInfo(hOut, &original);
        }
    }
};

static inline void printIntro() {
    std::cout << "Uzyj strzalek, aby zmienic wartosci X i Y. Nacisnij ESC, aby wyjsc." << std::endl;
    std::cout << std::fixed << std::setprecision(1);
}

static inline void printStatus(const State& s) {
    std::cout << '\r'
            << "Input: "
              << "X: " << std::setw(4) << s.x
              << "  Y: " << std::setw(4) << s.y
              << "      "
              << std::flush;
}

static inline void initStatus(const State& s) {
    std::cout << "X: " << std::setw(4) << s.x << "  Y: " << std::setw(4) << s.y << std::flush;
}

static inline void updateFromArrows(State& s, float step) {
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)  { if (s.x > -1.0f) s.x -= step; }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { if (s.x <  1.0f) s.x += step; }
    if (GetAsyncKeyState(VK_UP) & 0x8000)    { if (s.y <  1.0f) s.y += step; }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)  { if (s.y > -1.0f) s.y -= step; }
}

static inline bool escPressed() {
    if (_kbhit()) {
        char key = _getch();
        if (key == 27) return true;
    }
    return false;
}



static void runLoop(State& input, float step, DWORD delayMs) {
    unsigned long lastUpdate = GetTickCount();
    while (true) {
        State target{0.0f, 0.0f};
        State current{0.0f, 0.0f};
        State power{0.0f, 0.0f};
        if (escPressed()) break;
        updateFromArrows(input, step);
        runMotorControl(input, target, current, power, lastUpdate);
        printStatus(input, target, current, power);
        Sleep(delayMs);
    }
}

int main() {
    State input{0.0f, 0.0f};
    
    const float step = 0.1f;

    printIntro();
    {
        CursorGuard hideCursor(true); // ukryj kursor na czas animacji linii statusu
        initStatus(input);
        runLoop(input, step, 50);
        std::cout << std::endl; // złam linię po zakończeniu pętli
    } // kursor zostanie przywrócony

    std::cout << "Program zakonczony." << std::endl;
    return 0;
}