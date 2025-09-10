#include "structs.hpp"

#ifdef ARDUINO
#include <Arduino.h>
#endif

void initData(Data& data) {
    data.input = {0.0f, 0.0f};
    data.target = {0.0f, 0.0f};
    data.current = {0.0f, 0.0f};
    data.power = {0.0f, 0.0f};
    data.delayMs = 50;
    data.lastUpdate = GetTickCount();
    data.lastTargetUpdate = GetTickCount();
}