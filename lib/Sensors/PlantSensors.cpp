#include "PlantSensors.h"

bool PlantSensors::init() {
    Wire.begin(PIN_SDA, PIN_SCL);

    pinMode(PIN_SOIL_ADC, INPUT);
    pinMode(PIN_LIGHT_ADC, INPUT);

    if (bme.begin()) {
        Serial.println("BME280: OK");
    } else {
        Serial.println("BME280: BLAD");
    }

    Serial.println("Czujniki zainicjowane poprawnie");
    return true;
}

int PlantSensors::mapSoilMoisturePercentage(int rawValue) {
  int percentage = map(rawValue, SOIL_DRY_VALUE, SOIL_WET_VALUE, 0, 100);
  
  return constrain(percentage, 0, 100);
}

float PlantSensors::mapLightLux(int rawValue) {
    float voltage = (rawValue / ADC_RESOLUTION) * SYSTEM_VOLTAGE;
    
    if (voltage < 0.01) return 0.0;
    if (voltage > 3.29) voltage = 3.29;

    float rLdr = (SYSTEM_VOLTAGE * LIGHT_RESISTOR_VALUE / voltage) - LIGHT_RESISTOR_VALUE;

    float lux = pow(500.0 / (rLdr / 1000.0), 1.0 / 0.7);

    return lux;
}

PlantData PlantSensors::readAll() {
    PlantData data;

    BME_Result bmeRes = bme.getValues();

    if (bmeRes.success) {
        data.temperature = bmeRes.temperature;
        data.humidity = bmeRes.humidity;
    } else {
        data.temperature = -99.0;
        data.humidity = -99.0;
    }

    data.soilMoisture = mapSoilMoisturePercentage(analogRead(PIN_SOIL_ADC));
    data.lightLevel = mapLightLux(analogRead(PIN_LIGHT_ADC));

    return data;
}