#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "DataStructures.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C 

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