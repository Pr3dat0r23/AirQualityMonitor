#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QtCharts>
#include <nlohmann/json.hpp>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using json = nlohmann::json;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_searchButton_clicked();
    void on_stationList_currentRowChanged(int index);
    void on_sensorList_currentRowChanged(int index);
    void on_saveButton_clicked();
    void on_loadButton_clicked();
    void handleNetworkReply(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;

    struct Station {
        int id;
        QString name;
        QString address;
        double lat;
        double lon;
    };

    struct Sensor {
        int id;
        QString paramName;
        QString paramCode;
    };

    struct Measurement {
        QDateTime date;
        double value;
    };

    QList<Station> stations;
    QList<Sensor> sensors;
    QVector<Measurement> measurements;  // Zmienione na QVector dla lepszej wydajności

    Station currentStation;
    Sensor currentSensor;
    QString currentSensorName;

    void processStationsData(const json &data);
    void processSensorsData(const json &data);
    void processMeasurementsData(const json &data);
    void updateChart();
    void updateStats();
    void saveToJson(const QString &filename);
    void loadFromJson(const QString &filename);
    void showError(const QString &message);
};

#endif // MAINWINDOW_H
