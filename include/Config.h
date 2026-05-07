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

// --- Battery ---
#define PIN_BAT_ADC 0
#define BAT_MULTIPLIER 2.165f

// --- Power Management ---
#define TIME_TO_SLEEP_SEC 5
#define uS_TO_S_FACTOR 1000000ULL

// --- BLE UUIDs ---
#define SERVICE_UUID           "abcd1234-ef56-ab12-cd34-567890abcdef"
#define CHAR_LIVE_DATA_UUID    "beef1234-4321-beef-4321-beef12345678"
#define CHAR_TIME_SYNC_UUID    "cafe1234-4321-cafe-4321-cafe12345678"
#define CHAR_HISTORY_UUID      "dada1234-4321-dada-4321-dada12345678"
#define CHAR_ACK_UUID          "dead1234-4321-dead-4321-dead12345678"
#define BLE_TIMEOUT_MS         120000

// --- OLED DISLAY ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C 

// --- PROXIMITY SENSOR ---
#define MIN_DISTANCE_MM 150