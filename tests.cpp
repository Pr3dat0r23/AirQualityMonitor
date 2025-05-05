/**
 * @file tests.cpp
 * @brief Plik implementujący testy jednostkowe dla aplikacji AirQualityMonitor
 * @author Piotr Trzeciak
 * @date 2023
 * @version 1.0
 *
 * Plik zawiera testy jednostkowe dla głównych funkcjonalności klasy MainWindow,
 * w tym przetwarzania danych stacji, czujników, pomiarów oraz operacji na plikach JSON.
 */

#include "mainwindow.h"
#include <QtTest/QtTest>
#include <QtWidgets/QApplication>

/**
 * @class TestAirQualityMonitor
 * @brief Klasa testująca funkcjonalności aplikacji AirQualityMonitor
 *
 * Klasa zawiera zestaw testów jednostkowych weryfikujących poprawność działania
 * głównych komponentów aplikacji, takich jak przetwarzanie danych z API GIOS
 * i operacje na plikach JSON.
 */
class TestAirQualityMonitor : public QObject
{
    Q_OBJECT

private slots:
    /**
     * @brief Inicjalizacja środowiska testowego przed wykonaniem wszystkich testów
     *
     * Tworzy instancję QApplication i MainWindow potrzebne do przeprowadzenia testów.
     */
    void initTestCase();

    /**
     * @brief Sprzątanie zasobów po wykonaniu wszystkich testów
     *
     * Zwalnia pamięć zajmowaną przez QApplication i MainWindow.
     */
    void cleanupTestCase();

    /**
     * @brief Test przetwarzania danych stacji pomiarowych
     *
     * Weryfikuje poprawność parsowania i przetwarzania danych stacji pomiarowych
     * w formacie JSON na strukturę danych aplikacji.
     */
    void testStationProcessing();

    /**
     * @brief Test przetwarzania danych czujników
     *
     * Sprawdza poprawność konwersji danych czujników z formatu JSON
     * na obiekty wewnętrzne aplikacji.
     */
    void testSensorProcessing();

    /**
     * @brief Test przetwarzania danych pomiarowych
     *
     * Weryfikuje poprawność odczytu i interpretacji danych pomiarowych
     * z API GIOS.
     */
    void testMeasurementProcessing();

    /**
     * @brief Test operacji na plikach JSON
     *
     * Sprawdza poprawność zapisu i odczytu danych do/z pliku JSON,
     * w tym pełnego cyklu serializacji i deserializacji danych.
     */
    void testJsonSaveLoad();
};

// Globalne wskaźniki na obiekty aplikacji
QApplication* app = nullptr;       ///< Globalna instancja QApplication
MainWindow* mainWindow = nullptr;  ///< Główne okno aplikacji

/**
 * @brief Implementacja metody initTestCase
 */
void TestAirQualityMonitor::initTestCase()
{
    int argc = 0;
    char* argv[] = {nullptr};
    app = new QApplication(argc, argv);  // Tworzymy QApplication
    mainWindow = new MainWindow();       // Tworzymy główne okno

    QVERIFY(app != nullptr);
    QVERIFY(mainWindow != nullptr);
}

/**
 * @brief Implementacja metody cleanupTestCase
 */
void TestAirQualityMonitor::cleanupTestCase()
{
    delete mainWindow;
    delete app;

    mainWindow = nullptr;
    app = nullptr;
}

/**
 * @brief Implementacja testu przetwarzania stacji
 */
void TestAirQualityMonitor::testStationProcessing()
{
    MainWindow mainWindow;
    json testData = json::parse(R"(
        [
            {
                "id": 1,
                "stationName": "Test Station",
                "city": {"name": "Test City"},
                "addressStreet": "Test Street",
                "gegrLat": "50.0",
                "gegrLon": "20.0"
            }
        ]
    )");

    mainWindow.processStationsData(testData);

    // Weryfikacja wyników
    QCOMPARE(mainWindow.stations.size(), 1);
    QCOMPARE(mainWindow.stations[0].name, QString("Test Station"));
    QCOMPARE(mainWindow.stations[0].address, QString("Test City, Test Street"));
    QCOMPARE(mainWindow.stations[0].lat, 50.0);
    QCOMPARE(mainWindow.stations[0].lon, 20.0);
}

/**
 * @brief Implementacja testu przetwarzania czujników
 */
void TestAirQualityMonitor::testSensorProcessing()
{
    MainWindow mainWindow;
    json testData = json::parse(R"(
        [
            {
                "id": 101,
                "param": {
                    "paramName": "PM10",
                    "paramCode": "PM10"
                }
            }
        ]
    )");

    mainWindow.processSensorsData(testData);

    // Weryfikacja wyników
    QCOMPARE(mainWindow.sensors.size(), 1);
    QCOMPARE(mainWindow.sensors[0].paramName, QString("PM10"));
    QCOMPARE(mainWindow.sensors[0].paramCode, QString("PM10"));
}

/**
 * @brief Implementacja testu przetwarzania pomiarów
 */
void TestAirQualityMonitor::testMeasurementProcessing()
{
    MainWindow mainWindow;
    json testData = json::parse(R"(
        {
            "values": [
                {
                    "date": "2023-01-01 12:00:00",
                    "value": 25.5
                },
                {
                    "date": "2023-01-01 13:00:00",
                    "value": 30.2
                }
            ]
        }
    )");

    mainWindow.processMeasurementsData(testData);

    // Weryfikacja wyników
    QCOMPARE(mainWindow.measurements.size(), 2);
    QCOMPARE(mainWindow.measurements[0].value, 25.5);
    QCOMPARE(mainWindow.measurements[1].value, 30.2);
}

/**
 * @brief Implementacja testu operacji JSON
 */
void TestAirQualityMonitor::testJsonSaveLoad()
{
    MainWindow mainWindow;

    // Przygotowanie danych testowych
    mainWindow.currentStation = {1, "Test Station", "Test Address", 50.0, 20.0};
    mainWindow.currentSensor = {101, "PM10", "PM10"};
    mainWindow.measurements = {
        {QDateTime::fromString("2023-01-01 12:00:00", "yyyy-MM-dd hh:mm:ss"), 25.5},
        {QDateTime::fromString("2023-01-01 13:00:00", "yyyy-MM-dd hh:mm:ss"), 30.2}
    };

    // Utworzenie tymczasowego pliku
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString tempFileName = tempFile.fileName();
    tempFile.close();

    // Test zapisu
    mainWindow.saveToJson(tempFileName);

    // Wyczyszczenie danych
    mainWindow.currentStation = {};
    mainWindow.currentSensor = {};
    mainWindow.measurements.clear();

    // Test odczytu
    mainWindow.loadFromJson(tempFileName);

    // Weryfikacja wyników
    QCOMPARE(mainWindow.currentStation.name, QString("Test Station"));
    QCOMPARE(mainWindow.currentSensor.paramName, QString("PM10"));
    QCOMPARE(mainWindow.measurements.size(), 2);
    QCOMPARE(mainWindow.measurements[0].value, 25.5);
    QCOMPARE(mainWindow.measurements[1].value, 30.2);
}

/**
 * @brief Makro główne frameworka testowego Qt
 *
 * Tworzy główną funkcję main dla testów i łączy z metadanymi Qt.
 */
QTEST_MAIN(TestAirQualityMonitor)

/**
 * @brief Dyrektywa dołączająca metadane Qt
 */
#include "tests.moc"
