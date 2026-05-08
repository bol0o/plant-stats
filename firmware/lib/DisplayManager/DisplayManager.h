#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "DataStructures.h"
#include "Config.h"

class DisplayManager {
    private:
        Adafruit_SH1106G display;
        bool isAvailable = false;

    public:
        DisplayManager();
        bool init();
        void showData(PlantData data, bool isBleConnected);
        void turnOff();
        void turnOn();
};