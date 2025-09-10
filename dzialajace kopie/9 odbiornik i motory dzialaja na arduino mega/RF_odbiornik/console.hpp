#pragma once

#ifndef ARDUINO
void gotoxy(int x, int y);

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
#endif