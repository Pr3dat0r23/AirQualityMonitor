/**
 * @file mainwindow.h
 * @brief Definicja klasy MainWindow - głównego okna aplikacji monitorującej jakość powietrza.
 * @author Piotr Trzeciak
 * @date 2025
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <nlohmann/json.hpp>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using json = nlohmann::json;

/**
 * @class MainWindow
 * @brief Główne okno aplikacji monitorującej jakość powietrza.
 *
 * Klasa odpowiada za interfejs użytkownika, komunikację z API GIOS,
 * wyświetlanie danych pomiarowych na wykresie oraz zarządzanie danymi.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy MainWindow
     * @param parent Wskaźnik na obiekt rodzica (domyślnie nullptr)
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow
     */
    ~MainWindow();

private slots:
    friend class TestAirQualityMonitor;

    /**
     * @brief Slot obsługujący kliknięcie przycisku wyszukiwania stacji
     */
    void on_searchButton_clicked();

    /**
     * @brief Slot obsługujący zmianę wybranej stacji
     * @param index Indeks wybranej stacji
     */
    void on_stationList_currentRowChanged(int index);

    /**
     * @brief Slot obsługujący zmianę wybranego czujnika
     * @param index Indeks wybranego czujnika
     */
    void on_sensorList_currentRowChanged(int index);

    /**
     * @brief Slot obsługujący kliknięcie przycisku zapisu danych
     */
    void on_saveButton_clicked();

    /**
     * @brief Slot obsługujący kliknięcie przycisku wczytywania danych
     */
    void on_loadButton_clicked();

    /**
     * @brief Slot obsługujący odpowiedź z sieci
     * @param reply Wskaźnik na obiekt odpowiedzi sieciowej
     */
    void handleNetworkReply(QNetworkReply *reply);

private:
    Ui::MainWindow *ui; ///< Wskaźnik na interfejs użytkownika
    QNetworkAccessManager *networkManager; ///< Menadżer połączeń sieciowych

    /**
     * @struct Station
     * @brief Struktura przechowująca dane stacji pomiarowej
     */
    struct Station {
        int id; ///< ID stacji
        QString name; ///< Nazwa stacji
        QString address; ///< Adres stacji
        double lat; ///< Szerokość geograficzna
        double lon; ///< Długość geograficzna
    };

    /**
     * @struct Sensor
     * @brief Struktura przechowująca dane czujnika
     */
    struct Sensor {
        int id; ///< ID czujnika
        QString paramName; ///< Nazwa parametru
        QString paramCode; ///< Kod parametru
    };

    /**
     * @struct Measurement
     * @brief Struktura przechowująca dane pomiarowe
     */
    struct Measurement {
        QDateTime date; ///< Data i czas pomiaru
        double value; ///< Wartość pomiaru
    };

    QList<Station> stations; ///< Lista stacji
    QList<Sensor> sensors; ///< Lista czujników
    QVector<Measurement> measurements; ///< Wektor pomiarów

    Station currentStation; ///< Aktualnie wybrana stacja
    Sensor currentSensor; ///< Aktualnie wybrany czujnik
    QString currentSensorName; ///< Nazwa aktualnego czujnika

    /**
     * @brief Przetwarza dane stacji z formatu JSON
     * @param data Dane w formacie JSON
     */
    void processStationsData(const json &data);

    /**
     * @brief Przetwarza dane czujników z formatu JSON
     * @param data Dane w formacie JSON
     */
    void processSensorsData(const json &data);

    /**
     * @brief Przetwarza dane pomiarowe z formatu JSON
     * @param data Dane w formacie JSON
     */
    void processMeasurementsData(const json &data);

    /**
     * @brief Aktualizuje wykres pomiarów
     */
    void updateChart();

    /**
     * @brief Aktualizuje statystyki pomiarów
     */
    void updateStats();

    /**
     * @brief Zapisuje dane do pliku JSON
     * @param filename Nazwa pliku
     */
    void saveToJson(const QString &filename);

    /**
     * @brief Wczytuje dane z pliku JSON
     * @param filename Nazwa pliku
     */
    void loadFromJson(const QString &filename);

    /**
     * @brief Wyświetla komunikat o błędzie
     * @param message Treść komunikatu
     */
    void showError(const QString &message);
};

#endif // MAINWINDOW_H
