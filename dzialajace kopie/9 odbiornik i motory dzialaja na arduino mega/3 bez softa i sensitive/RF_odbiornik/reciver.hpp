#pragma once

#include <RH_ASK.h>

namespace Reciver {

// Inicjalizacja stanów czasowych modułu (ustaw na millis())
void init(unsigned long now);

// Odbierz pakiet i zaktualizuj stan, jeśli poprawny
void handleReceive(RH_ASK& driver);

// Failsafe po przekroczeniu czasu bez aktualizacji
void handleFailsafe(unsigned long now);

// Nieblokujące logowanie co stały interwał
void handlePeriodicLog(float xSet, float ySet, int sSet, unsigned long now);

} // namespace Reciver
