#pragma once

#ifndef ARDUINO
#include <windows.h>
#endif

struct State {
    float x;
    float y;
};

struct Data {
    State input;
    State target;
    State current;
    State power;
    int delayMs;
    unsigned long lastUpdate;
    unsigned long lastTargetUpdate;
    bool sw; // stan przycisku (1 = nie wciśnięty, 0 = wciśnięty)
};

void initData(Data& data);

#ifdef ARDUINO
unsigned long GetTickCount();
#endif