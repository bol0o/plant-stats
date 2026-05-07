#include <Arduino.h>
#include <time.h>
#include "esp_pm.h" // Biblioteka do zarządzania energią ESP32

#include "PlantSensors.h"
#include "DataLogger.h"
#include "BLEManager.h"

// --- Globalne obiekty ---
PlantSensors sensors;
DataLogger logger;
BLEManager ble;

// --- Ustawienia czasu ---
// Do testów ustawione na 30 sekund (30000 ms). 
// Docelowo zmień na 30 minut (1800000 ms).
const unsigned long LOG_INTERVAL = 600000; 
unsigned long lastLogTime = 0;

void setup() {
    Serial.begin(115200);
    delay(2000); // Krótka przerwa na ustabilizowanie napięcia po restarcie
    Serial.println("\n--- Start Systemu ---");

    // 1. Inicjalizacja modułów
    sensors.init();
    logger.init();
    ble.init();

    // 2. Konfiguracja Auto Light Sleep
    // Procesor sam uśnie, gdy dojdzie do delay() i nie będzie nic do roboty
    esp_pm_config_t pm_config = {
        .max_freq_mhz = 160,       // Max prędkość podczas pracy
        .min_freq_mhz = 40,        // Taktowanie w tle (podtrzymanie BLE)
        .light_sleep_enable = true // Zezwól na automatyczne usypianie rdzenia!
    };
    esp_pm_configure(&pm_config);

    Serial.println("System gotowy. Przechodzę w tryb Auto-Sleep.");
}

void loop() {
    unsigned long currentMillis = millis();

    // =================================================================
    // ZADANIE 1: Zapisywanie logów co określony czas (Zegar)
    // =================================================================
    if (currentMillis - lastLogTime >= LOG_INTERVAL) {
        lastLogTime = currentMillis; // Resetuj stoper
        
        Serial.println("EVENT: Wybudzenie z timera. Odczyt i logowanie...");
        
        PlantData data = sensors.readAll();

        Serial.printf("Temp: %.1f C | Hum: %.1f %% | Gleba: %d %% | Światło: %.1f lux | Bat: %d %%\n", 
              data.temperature, data.humidity, data.soilMoisture, data.lightLevel, data.batteryPercentage);
        
        // Dodaj aktualny czas do danych
        time_t now_unix;
        time(&now_unix);
        data.timestamp = (uint32_t)now_unix;
        
        logger.logData(data); // Zapis do pamięci Flash

        if (ble.isConnected()) {
            ble.updateLive(data);
            Serial.println("Wysłano dane LIVE przez BLE.");
        }
    }

    // =================================================================
    // ZADANIE 2: Obsługa Bluetooth (Zdarzenia z zewnątrz)
    // =================================================================

    if (ble.isConnected() && ble.needsHistory) {
        time_t current_time;
        time(&current_time);
        
        if (current_time > 1704067200) {
            PlantData live = sensors.readAll();
            live.timestamp = (uint32_t)current_time;
            ble.updateLive(live);
            
            ble.sendHistory();
            ble.needsHistory = false;
        }
    }

    if (ble.shouldClearLogs) {
        logger.clearLogs();
        ble.shouldClearLogs = false;
        Serial.println("System: Logi wyczyszczone.");
    }

    // =================================================================
    // ZADANIE 3: "Oddychanie" (Klucz do oszczędzania baterii)
    // =================================================================
    // Ta funkcja mówi procesorowi: "Odpocznij przez 10ms".
    // Ponieważ włączyliśmy 'light_sleep_enable', ESP32 w tym momencie
    // dosłownie wyłącza rdzeń obliczeniowy na ułamek sekundy, drastycznie
    // obniżając pobór prądu, ale zostawiając włączony Bluetooth.
    delay(10); 
}