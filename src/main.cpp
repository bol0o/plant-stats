#include <Arduino.h>
#include "PlantSensors.h"

PlantSensors sensors;

void setup() {
    Serial.begin(115200);
    
    delay(2000); 
    
    if (sensors.init()) {
        Serial.println("Status: Inicjalizacja powiodla sie!");
    } else {
        Serial.println("Status: Blad krytyczny inicjalizacji!");
    }    
}

void loop() {
    PlantData data = sensors.readAll();

    Serial.println("");    
    if (data.temperature > -90.0) {
        Serial.print("Temperatura:   "); Serial.print(data.temperature); Serial.println(" *C");
        Serial.print("Wilgotnosc pow: "); Serial.print(data.humidity);    Serial.println(" %");
    } else {
        Serial.println("BME280: Blad odczytu (sprawdz polaczenia!)");
    }

    Serial.print("Gleba (wilg):  "); Serial.print(data.soilMoisture);   Serial.println(" %");
    Serial.print("Swiatlo:       "); Serial.print(data.lightLevel, 1);  Serial.println(" Lux");

    Serial.println("----------------------------------------");

    delay(3000); 
}