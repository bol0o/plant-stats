#include <Arduino.h>
#include "BLEManager.h"
#include <sys/time.h>

void BLEManager::init() {
    NimBLEDevice::init("XIAO-C6-Kwiatek");
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this)); 

    NimBLEService* pService = pServer->createService(SERVICE_UUID);

    pLiveChar = pService->createCharacteristic(
        CHAR_LIVE_DATA_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pTimeChar = pService->createCharacteristic(
        CHAR_TIME_SYNC_UUID,
        NIMBLE_PROPERTY::WRITE
    );
    pTimeChar->setCallbacks(new TimeCallbacks());

    pHistoryChar = pService->createCharacteristic(
        CHAR_HISTORY_UUID, 
        NIMBLE_PROPERTY::NOTIFY
    );
    
    pHistoryChar->setCallbacks(new HistoryCallbacks(this));

    pAckChar = pService->createCharacteristic(
    CHAR_ACK_UUID,
    NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    pAckChar->setCallbacks(new AckCallbacks(this));

    pService->start();
    startAdv();
}

void BLEManager::sendHistory() {
    if (!deviceConnected) return;

    File file = LittleFS.open("/datalog.bin", FILE_READ);
    if (!file) {
        Serial.println("BLE: Błąd otwarcia pliku logów!");
        return;
    }

    uint32_t fileSize = file.size();
    uint32_t recordCount = fileSize / sizeof(PlantData);

    pHistoryChar->setValue((uint8_t*)&recordCount, 4);
    pHistoryChar->notify();
    delay(150);

    PlantData buffer;
    while (file.read((uint8_t*)&buffer, sizeof(PlantData))) {
        pHistoryChar->setValue((uint8_t*)&buffer, sizeof(PlantData));
        pHistoryChar->notify();
        delay(25);
    }

    file.close();
    Serial.println("BLE: Cała historia została wysłana.");
}

void BLEManager::startAdv() {
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    
    pAdvertising->addServiceUUID(SERVICE_UUID);

    pAdvertising->setMinInterval(1200); 
    pAdvertising->setMaxInterval(1200);
    
    pAdvertising->start();
    advertising = true;
    lastActivityTime = millis();

    Serial.println("BLE: Nadawanie co 1s...");
}

void BLEManager::stopAdv() {
    if (advertising) {
        NimBLEDevice::getAdvertising()->stop();
        advertising = false;
        Serial.println("BLE: Bluetooth wyłączony (Oszczędzanie energii).");
    }
}

void BLEManager::updateLive(PlantData data) {
    if (deviceConnected) {
        pLiveChar->setValue((uint8_t*)&data, sizeof(PlantData));
        pLiveChar->notify();
    }
}

void BLEManager::TimeCallbacks::onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
    NimBLEAttValue value = pCharacteristic->getValue();
    if (value.length() == 4) {
        uint32_t epoch;
        memcpy(&epoch, value.data(), 4);
        
        struct timeval tv;
        tv.tv_sec = epoch;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        
        Serial.printf("BLE: Czas ustawiony na: %u\n", epoch);
    }
}

void BLEManager::HistoryCallbacks::onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) {
    Serial.println("BLE: Laptop zasubskrybował historię. Można słać.");
    _mgr->needsHistory = true; 
}

void BLEManager::ServerCallbacks::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
    _mgr->deviceConnected = true;
    Serial.println("BLE: Klient połączony.");
};

void BLEManager::ServerCallbacks::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
    _mgr->deviceConnected = false;
    _mgr->needsHistory = false;
    
    _mgr->startAdv(); 
    Serial.println("BLE: Klient rozłączony. Wznowiono nadawanie.");
};

void BLEManager::AckCallbacks::onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
    NimBLEAttValue value = pCharacteristic->getValue();

    if (value.length() > 0 && value.data()[0] == 1) {
        Serial.println("BLE: Odebrano ACK. Laptop potwierdził odbiór danych.");
        _mgr->shouldClearLogs = true; 
    }
}