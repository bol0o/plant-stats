#include "DistanceSensor.h"
#include <Arduino.h>

bool DistanceSensor::init() {
    if (!lox.begin()) {
        Serial.println("VL53L0X: BLAD - Nie wykryto czujnika odległości!");
        isAvailable = false;
        return false;
    }
    
    Serial.println("VL53L0X: OK");
    isAvailable = true;
    return true;
}

bool DistanceSensor::checkForWakeup() {
    if (!isAvailable) return false;

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);

    if (measure.RangeStatus == 0) {
        uint16_t dist = measure.RangeMilliMeter;

        if (dist > 20 && dist < threshold_mm) {
            consecutiveCount++;
            
            if (consecutiveCount >= 2) {
                return true; 
            }
        } else {
            consecutiveCount = 0;
        }
    } else {
        consecutiveCount = 0;
    }

    return false;
}