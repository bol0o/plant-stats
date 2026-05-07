#include "DisplayManager.h"

DisplayManager::DisplayManager() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

bool DisplayManager::init() {
    // SH1106 inicjalizuje się odrobinę inaczej (drugi argument to reset)
    if(!display.begin(SCREEN_ADDRESS, true)) {
        Serial.println("OLED: BLAD - Nie wykryto ekranu SH1106!");
        isAvailable = false;
        return false;
    }
    
    Serial.println("OLED: OK (SH1106 1.3\")");
    isAvailable = true;
    
    // Ekran startowy
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE); // Zmieniona stała koloru
    display.setTextSize(1);
    
    display.setCursor(25, 25);
    display.println("Inicjalizacja");
    
    display.display();
    delay(1500); 
    
    turnOff(); 
    return true;
}

void DisplayManager::showData(PlantData data, bool isBleConnected) {
    if (!isAvailable) return;
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    
    // Pasek górny (Status)
    display.setCursor(0, 0);
    display.print("Bat: ");
    display.print(data.batteryPercentage);
    display.print("%");
    
    if (isBleConnected) {
        display.setCursor(100, 0);
        display.print("BLE");
    }
    
    // Linia oddzielająca
    display.drawLine(0, 10, 128, 10, SH110X_WHITE);

    // Główne odczyty
    display.setCursor(0, 16);
    display.printf("Temp:    %.1f C", data.temperature);
    
    display.setCursor(0, 28);
    display.printf("Wilg P:  %.1f %%", data.humidity);
    
    display.setCursor(0, 40);
    display.printf("Gleba:   %d %%", data.soilMoisture);
    
    display.setCursor(0, 52);
    display.printf("Swiatlo: %.0f lx", data.lightLevel);

    display.display();
}

void DisplayManager::turnOff() {
    if (!isAvailable) return;
    display.oled_command(SH110X_DISPLAYOFF); // Dedykowana komenda uśpienia
}

void DisplayManager::turnOn() {
    if (!isAvailable) return;
    display.oled_command(SH110X_DISPLAYON); // Dedykowana komenda wybudzenia
}