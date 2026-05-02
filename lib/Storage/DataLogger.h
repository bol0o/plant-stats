#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include "DataStructures.h"

class DataLogger {
    private:
        const char* logFileName = "/datalog.bin";
        
    public:
        bool init();
        void logData(PlantData data);
        void printLogs();
        void clearLogs();
};