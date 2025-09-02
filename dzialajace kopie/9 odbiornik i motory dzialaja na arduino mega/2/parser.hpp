#pragma once

// Parsuje wiadomość w formacie: "Nadajnik 591 X:<float> Y:<float> SW:<int>"
// Zwraca true, jeśli udało się odczytać co najmniej jedno z pól.
bool parseMessage(const char* message, float* x, float* y, int* sw);
