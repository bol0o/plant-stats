#include <Arduino.h>
#include "PlantSensors.h"
#include "DataLogger.h"

PlantSensors sensors;
DataLogger logger;

int logCounter = 0;

void setup() {
    Serial.begin(115200);
    delay(2000); 

    if (!sensors.init()) {
        Serial.println("Błąd inicjalizacji czujników!");
    }
    
    if (!logger.init()) {
        Serial.println("Błąd inicjalizacji systemu plików!");
    }

    logger.clearLogs(); logger.init();

    Serial.println("System gotowy. Rozpoczynam logowanie co 30s.");
}

void loop() {
    logCounter++;
    Serial.printf("\n--- Pomiar nr: %d ---\n", logCounter);

    PlantData data = sensors.readAll();

    Serial.printf("Odczyt: T:%.2f*C | W:%.2f%% | G:%d%% | L:%.1f | Bat:%d%%\n", 
                  data.temperature, 
                  data.humidity, 
                  data.soilMoisture, 
                  data.lightLevel, 
                  data.batteryPercentage);

    logger.logData(data);

    logger.printLogs();

    Serial.println("Czekam 5 sekund na kolejny zapis...");
    delay(10000); 
}