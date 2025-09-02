#include <RH_ASK.h>
#include <SPI.h> // wymagane przez RH_ASK, nawet jeśli nieużywane
#include <math.h>

// Piny joysticka
#define XPIN A0
#define YPIN A1
#define SWPIN A2

// Pin nadajnika RF
#define TX_PIN 12  // Pin nadawania RF

// Deadzone threshold for normalized joystick values
#define DEADZONE 0.15f

// Nadajnik RF - prędkość 2000 bps, TX pin 12, RX pin nie używany (255), PTT pin nie używany (255)
RH_ASK driver(2000, 255, TX_PIN, 255);

// (Usunięto zmienne do logowania)

void setup() {
    // Wyłączenie wbudowanej diody LED (pin 13)
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Inicjalizacja modułu RF
    if (!driver.init()) {
        while (1) { /* stop if RF init fails */ }
    }

    pinMode(XPIN, INPUT);
    pinMode(YPIN, INPUT);
    pinMode(SWPIN, INPUT_PULLUP);
}

void loop() {
    // Odczyt surowych wartości z joysticka
    int xRaw = analogRead(XPIN);
    int yRaw = analogRead(YPIN);
    int sVal = digitalRead(SWPIN);

    // Normalizacja wartości
    float xVal = (xRaw - 512.0) / 512.0;  // Zakres -1.0 do +1.0
    float yVal = (yRaw - 512.0) / 512.0;  // Zakres -1.0 do +1.0

    // Wyślij tylko, gdy |x| lub |y| przekracza deadzone
    if (fabsf(xVal) > DEADZONE || fabsf(yVal) > DEADZONE) {
        // Przygotowanie i wysłanie wiadomości
        String message = "Nadajnik 591 X:" + String(xVal, 2) + " Y:" + String(yVal, 2) + " SW:" + String(sVal);
        char msg[64];
        message.toCharArray(msg, sizeof(msg));

        driver.send((uint8_t *)msg, strlen(msg));
        driver.waitPacketSent();
    }

    delay(100);
}
