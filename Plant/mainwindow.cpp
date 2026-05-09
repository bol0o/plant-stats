#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_ble = new BleManager(this);

    auto setupChart = [](QChart *&chart, QSplineSeries *&series, QDateTimeAxis *&axisX, QChartView *&view, QString title, int yMin, int yMax, QColor color, QVBoxLayout *layout) {
        series = new QSplineSeries();
        series->setColor(color);

        chart = new QChart();
        chart->addSeries(series);
        chart->setTitle(title);
        chart->legend()->hide();
        chart->setTheme(QChart::ChartThemeDark);
        chart->setBackgroundBrush(QBrush(QColor(30, 30, 36)));

        axisX = new QDateTimeAxis();
        axisX->setTickCount(5);
        axisX->setFormat("hh:mm:ss");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(yMin, yMax);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        view = new QChartView(chart);
        view->setRenderHint(QPainter::Antialiasing);
        view->setRubberBand(QChartView::HorizontalRubberBand);
        view->setMinimumHeight(250);

        layout->addWidget(view);
    };

    setupChart(m_soilChart, m_soilSeries, m_axisTimeSoil, m_soilChartView, "Ground humidity (%)", 0, 100, Qt::cyan, ui->chartLayoutSoil);
    setupChart(m_tempChart, m_tempSeries, m_axisTimeTemp, m_tempChartView, "Temperature (°C)", 15, 35, Qt::red, ui->chartLayoutTemp);
    setupChart(m_humChart, m_humSeries, m_axisTimeHum, m_humChartView, "Air humidity (%)", 0, 100, Qt::green, ui->chartLayoutHum);
    setupChart(m_lightChart, m_lightSeries, m_axisTimeLight, m_lightChartView, "Light (lx)", 0, 4095, Qt::yellow, ui->chartLayoutLight);

    connect(m_ble, &BleManager::statusUpdate, this, &MainWindow::onStatusUpdate);
    connect(m_ble, &BleManager::dataReceived, this, &MainWindow::onDataReceived);
    connect(m_ble, &BleManager::historyDataReceived, this, &MainWindow::onHistoryDataReceived);

    connect(ui->btnScan, &QPushButton::clicked, m_ble, &BleManager::startScan);
    connect(ui->btnDownloadHistory, &QPushButton::clicked, this, &MainWindow::onDownloadHistoryClicked);
    connect(ui->btnClearLogs, &QPushButton::clicked, this, &MainWindow::onClearLogsClicked);

    // Style
    QString modernStyle = R"(
        QWidget {
            background-color: #1E1E24;
            color: #E2E2E2;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
        }

        QGroupBox {
            border: 2px solid #2B2B36;
            border-radius: 12px;
            margin-top: 25px; /* Miejsce na tytuł */
            background-color: #25252D;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 5px 15px;
            background-color: #2B2B36;
            color: #00ADB5; /* Morski akcent */
            border-radius: 8px;
            font-size: 15px;
            font-weight: 800;
            letter-spacing: 1px;
        }

        QGroupBox QLabel {
            background-color: transparent;
            font-size: 16px;
            font-weight: 500;
            padding: 4px;
        }

        QLabel#lblStatus {
            background-color: transparent;
            color: #8A8A9D;
            font-size: 14px;
            font-style: italic;
        }

        QPushButton {
            background-color: #2B2B36;
            border: 2px solid #363645;
            border-radius: 10px;
            padding: 10px;
            font-size: 15px;
            font-weight: bold;
            color: #FFFFFF;
        }
        QPushButton:hover {
            background-color: #363645;
            border-color: #00ADB5;
        }
        QPushButton:pressed {
            background-color: #1A1A20;
            border-color: #00ADB5;
        }

        QPushButton#btnScan, QPushButton#btnDownloadHistory {
            background-color: #00ADB5;
            border: none;
            color: #1E1E24;
        }
        QPushButton#btnScan:hover, QPushButton#btnDownloadHistory:hover {
            background-color: #00C4CC;
        }
        QPushButton#btnScan:pressed, QPushButton#btnDownloadHistory:pressed {
            background-color: #008C94;
        }

        QPushButton#btnClearLogs {
            background-color: #FF4B4B;
            border: none;
            color: #FFFFFF;
            margin-top: 15px;
        }
        QPushButton#btnClearLogs:hover {
            background-color: #FF6B6B;
        }
        QPushButton#btnClearLogs:pressed {
            background-color: #CC3C3C;
        }

        QScrollArea, QScrollArea > QWidget > QWidget {
            background-color: transparent;
            border: none;
        }

        QScrollBar:vertical {
            border: none;
            background: #1E1E24;
            width: 12px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #363645;
            min-height: 40px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background: #00ADB5;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
        }
    )";

    this->setStyleSheet(modernStyle);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onDownloadHistoryClicked() {
    m_soilSeries->clear();
    m_tempSeries->clear();
    ui->lblStatus->setText("Status: History request sent...");

    m_ble->requestHistory();
}

void MainWindow::onStatusUpdate(const QString &message) {
    ui->lblStatus->setText("Status: " + message);
}

void MainWindow::onDataReceived(float temp, float hum, int soil, float light, int battery) {

    QString baseStyle = "font-size: 16px; margin: 5px;";

    ui->lblLiveTemp->setText(QString("Temperature: %1 °C 🌡️").arg(temp, 0, 'f', 1));
    ui->lblLiveTemp->setStyleSheet(baseStyle);

    ui->lblLiveHum->setText(QString("Air hum.: %1 % 💧").arg(hum, 0, 'f', 1));
    ui->lblLiveHum->setStyleSheet(baseStyle);

    ui->lblLiveLight->setText(QString("Light: %1 lux ☀️").arg(light, 0, 'f', 1));
    ui->lblLiveLight->setStyleSheet(baseStyle);

    QString soilText = QString("Ground hum.: %1 % ").arg(soil);
    if (soil < 20) {
        soilText += "🥀";
        ui->lblLiveSoil->setStyleSheet(baseStyle + "color: #ff4444; font-weight: bold;");
    } else if (soil < 60) {
        soilText += "🪴";
        ui->lblLiveSoil->setStyleSheet(baseStyle + "color: #ffbb33; font-weight: bold;");
    } else {
        soilText += "🌿";
        ui->lblLiveSoil->setStyleSheet(baseStyle + "color: #00C851; font-weight: bold;");
    }
    ui->lblLiveSoil->setText(soilText);

    QString batText = QString("Battery: %1 % ").arg(battery);
    if (battery < 15) {
        batText += "🪫";
        ui->lblLiveBattery->setStyleSheet(baseStyle + "color: #ff4444; font-weight: bold;");
    } else if (battery >= 80) {
        batText += "🔋";
        ui->lblLiveBattery->setStyleSheet(baseStyle + "color: #00C851; font-weight: bold;");
    } else {
        batText += "🔋";
        ui->lblLiveBattery->setStyleSheet(baseStyle);
    }
    ui->lblLiveBattery->setText(batText);
}

void MainWindow::onHistoryDataReceived(uint32_t timestamp, float temp, float hum, int soil, float light, int battery) {
    qint64 msTime = static_cast<qint64>(timestamp) * 1000;
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(msTime);

    m_soilSeries->append(msTime, soil);
    m_tempSeries->append(msTime, temp);
    m_humSeries->append(msTime, hum);
    m_lightSeries->append(msTime, light);

    if (m_soilSeries->count() == 1) {
        m_axisTimeSoil->setMin(dt); m_axisTimeTemp->setMin(dt);
        m_axisTimeHum->setMin(dt); m_axisTimeLight->setMin(dt);
    }
    if (dt > m_axisTimeSoil->max()) {
        m_axisTimeSoil->setMax(dt); m_axisTimeTemp->setMax(dt);
        m_axisTimeHum->setMax(dt); m_axisTimeLight->setMax(dt);
    }
}

void MainWindow::onClearLogsClicked() {
    m_ble->sendAck();
    ui->lblStatus->setText("Status: Clear history request sent...");
}