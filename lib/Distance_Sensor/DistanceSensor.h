#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include "Config.h"

class DistanceSensor {
    private:
        Adafruit_VL53L0X lox;
        bool isAvailable = false;
        uint16_t threshold_mm = MIN_DISTANCE_MM;
        uint8_t consecutiveCount = 0;

    public:
        bool init();
        bool checkForWakeup();
};