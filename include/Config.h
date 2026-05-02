#pragma once

// --- I2C ---
#define PIN_SDA 22
#define PIN_SCL 23
#define BME280_ADDRESS 0x76

// --- Soil Moisture ---
#define PIN_SOIL_ADC 2

#define SOIL_DRY_VALUE 2835 
#define SOIL_WET_VALUE 1080

// --- Light Level ---
#define PIN_LIGHT_ADC 1
#define LIGHT_RESISTOR_VALUE 10000 // 10k Ohm
#define ADC_RESOLUTION 4095.0 // 12-bit
#define SYSTEM_VOLTAGE 3.3