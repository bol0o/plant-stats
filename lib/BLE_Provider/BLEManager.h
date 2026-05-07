#pragma once

#include <NimBLEDevice.h>
#include "PlantSensors.h"
#include "Config.h"
#include "LittleFS.h"

class BLEManager {
public:
    bool deviceConnected = false;
    bool needsHistory = false;
    bool shouldClearLogs = false;

    void init();
    void updateLive(PlantData data);
    void sendHistory();
    void checkTimeout();
    void startAdv();
    void stopAdv();
    bool isConnected() { return deviceConnected; }

private:
    NimBLEServer* pServer;
    NimBLECharacteristic* pLiveChar;
    NimBLECharacteristic* pTimeChar;
    NimBLECharacteristic* pHistoryChar;
    NimBLECharacteristic* pAckChar;

    unsigned long lastActivityTime = 0;
    bool advertising = false;

    class ServerCallbacks : public NimBLEServerCallbacks {
        BLEManager* _mgr;
    public:
        ServerCallbacks(BLEManager* mgr) : _mgr(mgr) {}
        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
    };

    class TimeCallbacks : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override;
    };

    class HistoryCallbacks : public NimBLECharacteristicCallbacks {
        BLEManager* _mgr;
    public:
        HistoryCallbacks(BLEManager* mgr) : _mgr(mgr) {}
        void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override;
    };

    class AckCallbacks : public NimBLECharacteristicCallbacks {
        BLEManager* _mgr;
    public:
        AckCallbacks(BLEManager* mgr) : _mgr(mgr) {}
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override;
    };
};