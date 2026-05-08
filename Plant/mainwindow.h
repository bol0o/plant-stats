#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QChart>
#include <QSplineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QDateTimeAxis>
#include "blemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStatusUpdate(const QString &message);
    void onDataReceived(float temp, float hum, int soil, float light, int battery);
    void onHistoryDataReceived(uint32_t timestamp, float temp, float hum, int soil, float light, int battery);
    void onDownloadHistoryClicked();
    void onClearLogsClicked();

private:
    Ui::MainWindow *ui;
    BleManager *m_ble;

    QSplineSeries *m_soilSeries;
    QSplineSeries *m_tempSeries;
    QSplineSeries *m_humSeries;
    QSplineSeries *m_lightSeries;

    QChart *m_soilChart;
    QChart *m_tempChart;
    QChart *m_humChart;
    QChart *m_lightChart;

    QChartView *m_soilChartView;
    QChartView *m_tempChartView;
    QChartView *m_humChartView;
    QChartView *m_lightChartView;

    QDateTimeAxis *m_axisTimeSoil;
    QDateTimeAxis *m_axisTimeTemp;
    QDateTimeAxis *m_axisTimeHum;
    QDateTimeAxis *m_axisTimeLight;
};

#endif