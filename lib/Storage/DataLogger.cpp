#include "DataLogger.h"

bool DataLogger::init() {
    if (!LittleFS.begin(true)) {
        Serial.println("BLAD: Nie udalo sie zamontowac systemu LittleFS!");
        return false;
    }
    
    if (!LittleFS.exists(logFileName)) {
        Serial.println("Logger: Brak pliku logow, zostanie utworzony automatycznie przy pierwszym zapisie.");
    } else {
        File file = LittleFS.open(logFileName, FILE_READ);
        int records = file.size() / sizeof(PlantData);
        file.close();
        Serial.printf("Logger: Znaleziono plik logow. Ilosc historycznych rekordow: %d\n", records);
    }

    return true;
}

void DataLogger::logData(PlantData data) {
    File file = LittleFS.open(logFileName, FILE_APPEND);
    if (!file) {
        Serial.println("BLAD: Nie mozna otworzyc pliku do dopisania!");
        return;
    }

    file.write((uint8_t*)&data, sizeof(PlantData));
    file.close();

    File readFile = LittleFS.open(logFileName, FILE_READ);
    int records = readFile.size() / sizeof(PlantData);
    readFile.close();

    Serial.printf("Logger: Zapisano dane binarnie (Rozmiar rekordu: %d bajtow). Rekord nr: %d\n", 
                  sizeof(PlantData), records);
}

void DataLogger::printLogs() {
    File file = LittleFS.open(logFileName, FILE_READ);
    if (!file) return;

    Serial.println("\n--- LOGI BINARNE ---");
    PlantData d;
    int count = 1;

    while (file.read((uint8_t*)&d, sizeof(PlantData))) {
        Serial.printf("[%d] T: %.2f*C | W: %.2f%% | G: %d%% | L: %.1f | Bat: %d%%\n", 
                      count, d.temperature, d.humidity, d.soilMoisture, d.lightLevel, d.batteryPercentage);
        count++;
    }
    file.close();
}

void DataLogger::clearLogs() {
    if (LittleFS.remove(logFileName)) {
        Serial.println("Logger: Pamięć wyczyszczona (plik logow usuniety).");
    } else {
        Serial.println("Logger: Brak pliku do usuniecia.");
    }
}