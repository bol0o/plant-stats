#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <QObject>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyService>
#include <QDateTime>

class BleManager : public QObject {
    Q_OBJECT
public:
    void requestHistory();
    explicit BleManager(QObject *parent = nullptr);
    void startScan();
    void syncTime();
    void sendAck();

signals:
    void dataReceived(float temp, float hum, int soil, float light, int battery);
    void historyDataReceived(uint32_t timestamp, float temp, float hum, int soil, float light, int battery);
    void statusUpdate(const QString &message);

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);
    void onControllerConnected();
    void onServiceDiscovered(const QBluetoothUuid &gatt);
    void onServiceStateChanged(QLowEnergyService::ServiceState newState);
    void onCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value);

private:
    void connectToDevice(const QBluetoothDeviceInfo &info);

    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QLowEnergyController *m_controller = nullptr;
    QLowEnergyService *m_service = nullptr;

    const QBluetoothUuid SERVICE_UUID = QBluetoothUuid(QString("abcd1234-ef56-ab12-cd34-567890abcdef"));
    const QBluetoothUuid LIVE_CHAR_UUID = QBluetoothUuid(QString("beef1234-4321-beef-4321-beef12345678"));
    const QBluetoothUuid TIME_SYNC_CHAR_UUID = QBluetoothUuid(QString("cafe1234-4321-cafe-4321-cafe12345678"));
    const QBluetoothUuid HISTORY_CHAR_UUID = QBluetoothUuid(QString("dada1234-4321-dada-4321-dada12345678"));
    const QBluetoothUuid ACK_CHAR_UUID = QBluetoothUuid(QString("dead1234-4321-dead-4321-dead12345678"));
};

#endif