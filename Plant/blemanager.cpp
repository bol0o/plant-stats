#include "blemanager.h"
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyService>
#include <QDebug>

BleManager::BleManager(QObject *parent) : QObject(parent) {
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BleManager::onDeviceDiscovered);
}


void BleManager::syncTime() {
    if (!m_service) return;

    QLowEnergyCharacteristic timeChar = m_service->characteristic(TIME_SYNC_CHAR_UUID);
    if (timeChar.isValid()) {
        uint32_t now = QDateTime::currentSecsSinceEpoch();

        QByteArray payload(reinterpret_cast<const char*>(&now), sizeof(now));
        m_service->writeCharacteristic(timeChar, payload, QLowEnergyService::WriteWithResponse);

        qDebug() << "Wysłano czas do kwiatka:" << now;
    }
}

void BleManager::sendAck() {
    if (!m_service) return;

    QLowEnergyCharacteristic ackChar = m_service->characteristic(ACK_CHAR_UUID);
    if (ackChar.isValid()) {
        QByteArray ackData = QByteArray::fromHex("01");
        m_service->writeCharacteristic(ackChar, ackData, QLowEnergyService::WriteWithoutResponse);
    }
}

void BleManager::requestHistory() {
    if (!m_service) return;

    syncTime();

    QLowEnergyCharacteristic historyChar = m_service->characteristic(HISTORY_CHAR_UUID);
    if (historyChar.isValid()) {
        QLowEnergyDescriptor descHist = historyChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (descHist.isValid()) {
            m_service->writeDescriptor(descHist, QByteArray::fromHex("0100"));
        }
    }
}

void BleManager::onCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value) {
    if (value.size() >= 24) {

        uint32_t timestamp;
        float temp, hum, light;
        int soil, battery;

        memcpy(&timestamp, value.data(), 4);
        memcpy(&temp, value.data() + 4, 4);
        memcpy(&hum, value.data() + 8, 4);
        memcpy(&soil, value.data() + 12, 4);
        memcpy(&light, value.data() + 16, 4);
        memcpy(&battery, value.data() + 20, 4);

        if (c.uuid() == LIVE_CHAR_UUID) {
            emit dataReceived(temp, hum, soil, light, battery);
        }
        else if (c.uuid() == HISTORY_CHAR_UUID) {
            emit historyDataReceived(timestamp, temp, hum, soil, light, battery);
        }
    }
}

void BleManager::startScan() {
    emit statusUpdate("Szukam XIAO-C6...");
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BleManager::onDeviceDiscovered(const QBluetoothDeviceInfo &info) {
    if (info.name().contains("XIAO-C6-Kwiatek")) {
        emit statusUpdate("Znaleziono kwiatek! Próbuję się połączyć...");
        m_discoveryAgent->stop();
        connectToDevice(info);
    }
}

void BleManager::connectToDevice(const QBluetoothDeviceInfo &info) {
    if (m_controller) {
        m_controller->disconnectFromDevice();
        delete m_controller;
    }

    m_controller = QLowEnergyController::createCentral(info, this);

    connect(m_controller, &QLowEnergyController::connected, this, &BleManager::onControllerConnected);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BleManager::onServiceDiscovered);
    connect(m_controller, &QLowEnergyController::discoveryFinished, [this](){
        emit statusUpdate("Wszystkie serwisy wykryte.");
    });

    m_controller->connectToDevice();
}

void BleManager::onControllerConnected() {
    emit statusUpdate("Połączono. Szukam usług...");
    m_controller->discoverServices();
}

void BleManager::onServiceDiscovered(const QBluetoothUuid &gatt) {
    if (gatt == SERVICE_UUID) {
        emit statusUpdate("Znaleziono usługę Kwiatka!");

        m_service = m_controller->createServiceObject(gatt, this);

        if (m_service) {
            connect(m_service, &QLowEnergyService::stateChanged, this, &BleManager::onServiceStateChanged);
            connect(m_service, &QLowEnergyService::characteristicChanged, this, &BleManager::onCharacteristicChanged);
            m_service->discoverDetails();
        }
    }
}

void BleManager::onServiceStateChanged(QLowEnergyService::ServiceState newState) {
    if (newState == QLowEnergyService::RemoteServiceDiscovered) {
        QLowEnergyCharacteristic liveChar = m_service->characteristic(LIVE_CHAR_UUID);
        if (liveChar.isValid()) {
            QLowEnergyDescriptor desc = liveChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
            if (desc.isValid()) m_service->writeDescriptor(desc, QByteArray::fromHex("0100"));
        }

        syncTime();

        emit statusUpdate("Gotowy! Połączono i zsynchronizowano czas.");
    }
}