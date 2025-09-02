#include <RH_ASK.h>
#include <SPI.h> // wymagane przez RH_ASK, nawet jeśli nieużywane

// Piny joysticka
#define XPIN A0
#define YPIN A1
#define SWPIN A2

// Pin nadajnika RF
#define TX_PIN 12  // Pin nadawania RF

// Konfiguracja logowania
#define ENABLE_DETAILED_LOGGING true  // Zmień na false aby wyłączyć szczegółowe logi
#define LOG_INTERVAL 500  // Logowanie co 500ms (0.5 sekundy) - częściej
#define ENABLE_RAW_VALUES true  // Pokaż surowe wartości ADC do debugowania

// Nadajnik RF - prędkość 2000 bps, TX pin 12, RX pin nie używany (255), PTT pin nie używany (255)
RH_ASK driver(2000, 255, TX_PIN, 255);

// Zmienne do logowania
unsigned long lastLogTime = 0;
unsigned long messageCounter = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("=== NADAJNIK RF 591 - URUCHAMIANIE ===");
    
    // Wyłączenie wbudowanej diody LED (pin 13)
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("[OK] Wbudowana dioda LED wyłączona");
    
    if (!driver.init()) {
        Serial.println("[ERROR] Inicjalizacja modułu RF nieudana!");
        while(1); // Zatrzymaj program jeśli RF nie działa
    } else {
        Serial.println("[OK] Moduł RF zainicjalizowany pomyślnie");
    }
    
    pinMode(XPIN, INPUT);
    pinMode(YPIN, INPUT);
    pinMode(SWPIN, INPUT_PULLUP);
    
    Serial.println("[OK] Piny joysticka skonfigurowane");
    Serial.println("[INFO] Prędkość transmisji: 2000 bps");
    Serial.println("[INFO] Pin nadawania: " + String(TX_PIN));
    Serial.println("[INFO] Szczegółowe logowanie: " + String(ENABLE_DETAILED_LOGGING ? "WŁĄCZONE" : "WYŁĄCZONE"));
    Serial.println("=== NADAJNIK RF GOTOWY DO PRACY ===\n");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Odczyt surowych wartości z joysticka
    int xRaw = analogRead(XPIN);
    int yRaw = analogRead(YPIN);
    int sVal = digitalRead(SWPIN);
    
    // Normalizacja wartości (z lepszą kalibracją)
    float xVal = (xRaw - 512.0) / 512.0;  // Normalizacja do zakresu -1.0 do +1.0
    float yVal = (yRaw - 512.0) / 512.0;  // Normalizacja do zakresu -1.0 do +1.0

    // Przygotowanie wiadomości
    // Używamy String concatenation zamiast snprintf() - bardziej niezawodne w Arduino
    String message = "Nadajnik 591 X:" + String(xVal, 2) + " Y:" + String(yVal, 2) + " SW:" + String(sVal);
    
    // Konwersja do char array dla RF
    char msg[64];  // Zwiększony rozmiar bufora
    message.toCharArray(msg, sizeof(msg));
    
    // Diagnostyka długości wiadomości
    int msgLength = message.length();
    if (msgLength >= 32) {
        Serial.println("[WARNING] Wiadomość za długa: " + String(msgLength) + " znaków!");
    }
    
    // Wysłanie wiadomości RF
    bool sendSuccess = driver.send((uint8_t *)msg, strlen(msg));
    if (!sendSuccess) {
        Serial.println("[ERROR] Błąd podczas wysyłania wiadomości RF!");
    }
    
    driver.waitPacketSent();
    messageCounter++;
    
    // Logowanie szczegółowe (co określony interwał)
    if (ENABLE_DETAILED_LOGGING && (currentTime - lastLogTime >= LOG_INTERVAL)) {
        Serial.println("=== STATUS NADAJNIKA ===");
        Serial.println("[DEBUG] Oryginalny String: " + message);
        Serial.println("[DEBUG] Długość String: " + String(message.length()));
        Serial.println("[DATA] Wiadomość #" + String(messageCounter) + ": " + String(msg));
        Serial.println("[DEBUG] Długość char[]: " + String(strlen(msg)));
        
        if (ENABLE_RAW_VALUES) {
            Serial.println("[RAW] X_ADC: " + String(xRaw) + " | Y_ADC: " + String(yRaw) + " | SW_RAW: " + String(sVal));
        }
        
        Serial.println("[JOYSTICK] X: " + String(xVal, 3) + " | Y: " + String(yVal, 3) + " | SW: " + String(sVal == LOW ? "WCIŚNIĘTY" : "ZWOLNIONY"));
        Serial.println("[RF] Status wysyłania: " + String(sendSuccess ? "OK" : "BŁĄD"));
        Serial.println("[SYSTEM] Czas pracy: " + String(currentTime/1000) + "s | Wysłane wiadomości: " + String(messageCounter));
        
        // Dodatkowa diagnostyka
        if (xRaw == 0 && yRaw == 0) {
            Serial.println("[WARNING] Joystick może być niepodłączony lub uszkodzony!");
        }
        
        Serial.println("========================\n");
        lastLogTime = currentTime;
    }
    
    // Podstawowe logowanie błędów (zawsze aktywne)
    if (!sendSuccess) {
        Serial.println("[ERROR] Wiadomość #" + String(messageCounter) + " - błąd wysyłania!");
    }
    
    delay(100);
}
