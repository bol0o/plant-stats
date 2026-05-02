#pragma once

#include <Arduino.h>
#include <Wire.h>

struct BME_Result {
    float temperature;
    float humidity;
    bool success;
};

class BME280_Driver {
    private:
        uint8_t _addr;

        struct {
            uint16_t dig_T1; int16_t dig_T2, dig_T3;
            uint16_t dig_P1; int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
            uint8_t  dig_H1; int16_t dig_H2; uint8_t  dig_H3; int16_t dig_H4, dig_H5; int8_t dig_H6;
        } _calib;

        int32_t t_fine;

        void writeReg(uint8_t reg, uint8_t val);
        bool readRegs(uint8_t reg, uint8_t* data, uint8_t len);
        float compensateTemp(int32_t adc_T);
        float compensateHum(int32_t adc_H);
    
    public:
        BME280_Driver(uint8_t addr = 0x76) : _addr(addr) {}
        bool begin();
        BME_Result getValues();
};