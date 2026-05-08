#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "DataStructures.h"
#include "Config.h"
#include "BME280_Driver.h"

class PlantSensors {
    private:
        BME280_Driver bme;
        int mapSoilMoisturePercentage(int rawValue);
        float mapLightLux(int rawValue);
        float readBatteryVoltage();
        int calculateBatteryPercentage(float voltage);
    public:
        bool init();
        PlantData readAll();
};