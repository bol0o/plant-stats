#include <Arduino.h>
#include <time.h>
#include "esp_pm.h"

#include "PlantSensors.h"
#include "DataLogger.h"
#include "BLEManager.h"
#include "DistanceSensor.h"
#include "DisplayManager.h"

PlantSensors sensors;
DataLogger logger;
BLEManager ble;
DistanceSensor distSensor;
DisplayManager screen;

const unsigned long LOG_INTERVAL = 600000; 
unsigned long lastLogTime = 0;

bool isScreenOn = false;
unsigned long screenWakeupTime = 0;
unsigned long lastScreenUpdate = 0;
const unsigned long SCREEN_TIMEOUT_MS = 10000; 

void setup() {
    Serial.begin(115200);
    delay(2000); 
    Serial.println("\n--- Start Systemu ---");

    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000);
    delay(100);

    if(sensors.init()) {
        Serial.println("Sensory: OK");
    }
    delay(50);

    if(distSensor.init()) {
        Serial.println("VL53L0X: OK");
    }
    delay(50);

    if(screen.init()) {
        Serial.println("OLED: OK");
    }
    
    logger.init();
    ble.init();

    esp_pm_config_t pm_config = {
        .max_freq_mhz = 160,
        .min_freq_mhz = 40,
        .light_sleep_enable = true 
    };
    esp_pm_configure(&pm_config);

    Serial.println("System gotowy.");
}

void loop() {
    unsigned long currentMillis = millis();

    // Obsługa gestu i ekranu
    // Wybudzanie przy wykryciu ręki
    if (distSensor.checkForWakeup()) {
        screenWakeupTime = currentMillis; 
        
        if (!isScreenOn) {
            isScreenOn = true;
            Serial.println("Ekran włączony");
            screen.turnOn();
            
            lastScreenUpdate = 0; 
        }
    }

    // Usypianie ekranu po czasie
    if (isScreenOn && (currentMillis - screenWakeupTime > SCREEN_TIMEOUT_MS)) {
        isScreenOn = false;
        Serial.println("Ekran wyłączony");
        screen.turnOff();
    }

    // Odświeżanie danych na ekranie tylko gdy swieci
    if (isScreenOn && (currentMillis - lastScreenUpdate >= 1000)) {
        lastScreenUpdate = currentMillis;
        screen.showData(sensors.readAll(), ble.isConnected());
    }

    // Zapisywanie logow
    if (currentMillis - lastLogTime >= LOG_INTERVAL) {
        lastLogTime = currentMillis;
        
        PlantData data = sensors.readAll();
        
        time_t now_unix;
        time(&now_unix);
        data.timestamp = (uint32_t)now_unix;
        
        // logger.logData(data); 

        if (ble.isConnected()) {
            ble.updateLive(data);
        }
    }

    // Obsluga BLE
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
    }

    delay(20); 
}