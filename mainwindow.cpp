/**
 * @file mainwindow.cpp
 * @brief Implementacja klasy głównego okna aplikacji monitorującej jakość powietrza
 * @author Piotr Trzeciak
 * @date 2025
 * @version 1.2
 *
 * Pełna implementacja metod klasy MainWindow zarządzającej:
 * - Interfejsem użytkownika
 * - Komunikacją sieciową z API GIOS
 * - Przetwarzaniem danych JSON
 * - Wizualizacją danych na wykresach QtCharts
 * - Operacjami zapisu/odczytu plików JSON
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <QDebug>
#include <QFile>

// =============================================
// Konstruktor/Destruktor
// =============================================

/**
 * @brief Konstruktor klasy MainWindow
 * @param parent Wskaźnik na widget rodzica
 *
 * Inicjalizuje UI, konfiguruje menadżer sieciowy
 * i przygotowuje podstawowy wykres.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Inicjalizacja menadżera sieci
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &MainWindow::handleNetworkReply);

    // Podstawowa konfiguracja wykresu
    QChart *chart = new QChart();
    chart->setTitle("Oczekiwanie na dane...");
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

/**
 * @brief Destruktor klasy MainWindow
 *
 * Zwalnia zasoby UI i czyści pamięć.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

// =============================================
// Sloty prywatne
// =============================================

/**
 * @brief Obsługa kliknięcia przycisku wyszukiwania
 *
 * Weryfikuje poprawność danych wejściowych i inicjuje
 * żądanie sieciowe do API GIOS.
 */
void MainWindow::on_searchButton_clicked()
{
    QString location = ui->locationEdit->text().trimmed();
    if (location.isEmpty()) {
        showError("Proszę wprowadzić nazwę miejscowości");
        return;
    }

    QString apiUrl = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
    networkManager->get(QNetworkRequest(QUrl(apiUrl)));
}

/**
 * @brief Obsługa zmiany wybranej stacji
 * @param currentRow Indeks wybranej stacji w liście
 *
 * Pobiera dane czujników dla wybranej stacji.
 */
void MainWindow::on_stationList_currentRowChanged(int currentRow)
{
    if (currentRow < 0 || currentRow >= stations.size()) return;

    currentStation = stations[currentRow];
    ui->infoText->setHtml(QString("<b>%1</b><br>Adres: %2<br>Współrzędne: %3, %4")
                              .arg(currentStation.name)
                              .arg(currentStation.address)
                              .arg(currentStation.lat)
                              .arg(currentStation.lon));

    QString sensorsUrl = QString("https://api.gios.gov.pl/pjp-api/rest/station/sensors/%1")
                             .arg(currentStation.id);
    networkManager->get(QNetworkRequest(QUrl(sensorsUrl)));
}

/**
 * @brief Obsługa zmiany wybranego czujnika
 * @param currentRow Indeks wybranego czujnika w liście
 *
 * Pobiera dane pomiarowe dla wybranego czujnika.
 */
void MainWindow::on_sensorList_currentRowChanged(int currentRow)
{
    if (currentRow < 0 || currentRow >= sensors.size()) return;

    currentSensor = sensors[currentRow];
    QString dataUrl = QString("https://api.gios.gov.pl/pjp-api/rest/data/getData/%1")
                          .arg(currentSensor.id);
    networkManager->get(QNetworkRequest(QUrl(dataUrl)));
}

/**
 * @brief Obsługa zapisu danych do pliku
 *
 * Umożliwia użytkownikowi wybór lokalizacji i zapisuje
 * aktualne dane pomiarowe do pliku JSON.
 */
void MainWindow::on_saveButton_clicked()
{
    if (measurements.isEmpty()) {
        showError("Brak danych do zapisania");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz dane", "", "JSON (*.json)");
    if (fileName.isEmpty()) return;

    saveToJson(fileName);
}

/**
 * @brief Obsługa wczytywania danych z pliku
 *
 * Umożliwia użytkownikowi wybór pliku JSON
 * i wczytuje z niego dane pomiarowe.
 */
void MainWindow::on_loadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Wczytaj dane", "", "JSON (*.json)");
    if (fileName.isEmpty()) return;

    loadFromJson(fileName);
}

// =============================================
// Metody prywatne
// =============================================

/**
 * @brief Przetwarza odpowiedź sieciową
 * @param reply Wskaźnik na obiekt odpowiedzi sieciowej
 *
 * Kieruje odpowiedź do odpowiedniej metody przetwarzania
 * w zależności od typu żądania.
 */
void MainWindow::handleNetworkReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        showError(QString("Błąd sieciowy: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    try {
        json response = json::parse(data.toStdString());
        QString url = reply->url().toString();

        if (url.contains("findAll")) {
            processStationsData(response);
        }
        else if (url.contains("sensors")) {
            processSensorsData(response);
        }
        else if (url.contains("getData")) {
            processMeasurementsData(response);
        }
    } catch (const std::exception &e) {
        showError(QString("Błąd przetwarzania danych: %1").arg(e.what()));
    }
}

/**
 * @brief Przetwarza dane stacji pomiarowych
 * @param data Dane JSON z listą stacji
 *
 * Parsuje dane stacji, filtruje je według lokalizacji
 * i wypełnia listę stacji w interfejsie.
 */
void MainWindow::processStationsData(const json &data)
{
    ui->stationList->clear();
    stations.clear();

    QString location = ui->locationEdit->text().trimmed().toLower();

    for (const auto &item : data) {
        QString cityName = QString::fromStdString(item["city"]["name"].get<std::string>()).toLower();

        if (!location.isEmpty() && !cityName.contains(location))
            continue;

        Station station;
        station.id = item["id"].get<int>();
        station.name = QString::fromStdString(item["stationName"].get<std::string>());

        QString address = QString::fromStdString(item["city"]["name"].get<std::string>());
        if (item["addressStreet"].is_string()) {
            address += ", " + QString::fromStdString(item["addressStreet"].get<std::string>());
        }
        station.address = address;

        station.lat = QString::fromStdString(item["gegrLat"].get<std::string>()).toDouble();
        station.lon = QString::fromStdString(item["gegrLon"].get<std::string>()).toDouble();

        stations.append(station);
        ui->stationList->addItem(station.name + "\n" + station.address);
    }

    if (stations.isEmpty()) {
        showError("Nie znaleziono stacji w podanej lokalizacji.");
    }
}

/**
 * @brief Przetwarza dane czujników
 * @param data Dane JSON z listą czujników
 *
 * Parsuje dane czujników i wypełnia listę czujników w interfejsie.
 */
void MainWindow::processSensorsData(const json &data)
{
    ui->sensorList->clear();
    sensors.clear();

    for (const auto &item : data) {
        Sensor sensor;
        sensor.id = item["id"].get<int>();
        sensor.paramName = QString::fromStdString(item["param"]["paramName"].get<std::string>());
        sensor.paramCode = QString::fromStdString(item["param"]["paramCode"].get<std::string>());

        sensors.append(sensor);
        ui->sensorList->addItem(sensor.paramName + " (" + sensor.paramCode + ")");
    }
}

/**
 * @brief Przetwarza dane pomiarowe i aktualizuje wykres
 * @param data Dane JSON z pomiarami
 *
 * Parsuje dane pomiarowe, tworzy nową serię danych,
 * konfiguruje wykres i oblicza statystyki.
 */
void MainWindow::processMeasurementsData(const json &data)
{
    measurements.clear();
    QChart *chart = ui->chartView->chart();
    chart->removeAllSeries();

    // Usuwanie istniejących osi
    foreach(QAbstractAxis *axis, chart->axes()) {
        chart->removeAxis(axis);
        delete axis;
    }

    if (!data.contains("values") || !data["values"].is_array()) {
        showError("Otrzymano nieprawidłową strukturę danych pomiarowych");
        return;
    }

    QLineSeries *series = new QLineSeries();
    QVector<double> validValues;

    for (const auto& item : data["values"]) {
        try {
            if (!item.contains("date") || !item.contains("value")) {
                continue;
            }

            QDateTime dateTime = QDateTime::fromString(
                QString::fromStdString(item["date"].get<std::string>()),
                Qt::ISODate
                );

            if (!dateTime.isValid()) continue;

            double value = 0;
            bool valueValid = false;

            if (item["value"].is_number()) {
                value = item["value"].get<double>();
                valueValid = true;
            }
            else if (item["value"].is_string()) {
                QString valueStr = QString::fromStdString(item["value"].get<std::string>()).trimmed();
                if (!valueStr.isEmpty()) {
                    bool conversionOk = false;
                    value = valueStr.toDouble(&conversionOk);
                    valueValid = conversionOk;
                }
            }

            if (!valueValid || std::isnan(value) || std::isinf(value)) {
                continue;
            }

            series->append(dateTime.toMSecsSinceEpoch(), value);
            validValues.append(value);
            measurements.append({dateTime, value});

        } catch (const std::exception& e) {
            qDebug() << "Błąd przetwarzania rekordu:" << e.what();
            continue;
        }
    }

    if (measurements.empty()) {
        showError("Nie znaleziono żadnych prawidłowych danych pomiarowych");
        return;
    }

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("dd.MM hh:mm");
    axisX->setTitleText("Data i godzina");

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(currentSensor.paramCode + " [µg/m³]");

    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    if (!validValues.empty()) {
        auto [minIt, maxIt] = std::minmax_element(validValues.begin(), validValues.end());
        double avg = std::accumulate(validValues.begin(), validValues.end(), 0.0) / validValues.size();

        ui->minValueLabel->setText(QString::number(*minIt, 'f', 2));
        ui->maxValueLabel->setText(QString::number(*maxIt, 'f', 2));
        ui->avgValueLabel->setText(QString::number(avg, 'f', 2));
    }

    chart->setTitle("Dane pomiarowe: " + currentSensor.paramName);
    chart->legend()->setVisible(false);
}

/**
 * @brief Zapisuje dane do pliku JSON
 * @param filename Ścieżka do pliku docelowego
 *
 * Serializuje aktualne dane (stację, czujnik, pomiary)
 * do formatu JSON i zapisuje do pliku.
 */
void MainWindow::saveToJson(const QString &filename)
{
    json data;

    // Zapis informacji o stacji
    data["station"]["id"] = currentStation.id;
    data["station"]["name"] = currentStation.name.toStdString();
    data["station"]["address"] = currentStation.address.toStdString();
    data["station"]["lat"] = currentStation.lat;
    data["station"]["lon"] = currentStation.lon;

    // Zapis informacji o czujniku
    data["sensor"]["id"] = currentSensor.id;
    data["sensor"]["paramName"] = currentSensor.paramName.toStdString();
    data["sensor"]["paramCode"] = currentSensor.paramCode.toStdString();

    // Zapis pomiarów
    for (const auto &m : measurements) {
        json measurement;
        measurement["date"] = m.date.toString(Qt::ISODate).toStdString();
        measurement["value"] = m.value;
        data["measurements"].push_back(measurement);
    }

    std::ofstream file(filename.toStdString());
    if (file.is_open()) {
        file << data.dump(2);
        QMessageBox::information(this, "Sukces", "Dane zostały zapisane");
    } else {
        showError("Nie można zapisać pliku");
    }
}

/**
 * @brief Wczytuje dane z pliku JSON
 * @param filename Ścieżka do pliku źródłowego
 *
 * Deserializuje dane z pliku JSON i aktualizuje
 * interfejs użytkownika na podstawie wczytanych danych.
 */
void MainWindow::loadFromJson(const QString &filename)
{
    try {
        std::ifstream file(filename.toStdString());
        json data = json::parse(file);

        // Wczytanie informacji o stacji
        currentStation.id = data["station"]["id"].get<int>();
        currentStation.name = QString::fromStdString(data["station"]["name"].get<std::string>());
        currentStation.address = QString::fromStdString(data["station"]["address"].get<std::string>());
        currentStation.lat = data["station"]["lat"].get<double>();
        currentStation.lon = data["station"]["lon"].get<double>();

        // Wczytanie informacji o czujniku
        currentSensor.id = data["sensor"]["id"].get<int>();
        currentSensor.paramName = QString::fromStdString(data["sensor"]["paramName"].get<std::string>());
        currentSensor.paramCode = QString::fromStdString(data["sensor"]["paramCode"].get<std::string>());

        // Wczytanie pomiarów
        measurements.clear();
        for (const auto &item : data["measurements"]) {
            Measurement m;
            m.date = QDateTime::fromString(
                QString::fromStdString(item["date"].get<std::string>()),
                Qt::ISODate
                );
            m.value = item["value"].get<double>();
            measurements.append(m);
        }

        // Aktualizacja UI
        ui->stationList->clear();
        ui->stationList->addItem(currentStation.name + "\n" + currentStation.address);

        ui->sensorList->clear();
        ui->sensorList->addItem(currentSensor.paramName + " (" + currentSensor.paramCode + ")");

        updateChart();
        updateStats();
        QMessageBox::information(this, "Sukces", "Dane zostały wczytane");

    } catch (const std::exception &e) {
        showError("Błąd wczytywania pliku: " + QString(e.what()));
    }
}

/**
 * @brief Wyświetla komunikat o błędzie
 * @param message Treść komunikatu błędu
 *
 * Wyświetla standardowe okno dialogowe z komunikatem błędu.
 */
void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, "Błąd", message);
}

/**
 * @brief Aktualizuje wykres pomiarów
 *
 * Odświeża wykres na podstawie aktualnych danych pomiarowych.
 */
void MainWindow::updateChart()
{
    QChart *chart = ui->chartView->chart();
    chart->removeAllSeries();

    if (measurements.isEmpty()) return;

    QLineSeries *series = new QLineSeries();
    series->setName(currentSensor.paramName);

    for (const auto &m : measurements) {
        series->append(m.date.toMSecsSinceEpoch(), m.value);
    }

    chart->addSeries(series);
    chart->setTitle(QString("%1 - %2").arg(currentSensor.paramName).arg(currentStation.name));

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("dd.MM.yyyy hh:mm");
    axisX->setTitleText("Data i godzina");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(currentSensor.paramCode);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

/**
 * @brief Aktualizuje statystyki pomiarów
 *
 * Oblicza i wyświetla podstawowe statystyki (min, max, średnia)
 * w panelu informacyjnym.
 */
void MainWindow::updateStats()
{
    if (measurements.isEmpty()) return;

    double min = measurements.first().value;
    double max = measurements.first().value;
    double sum = 0;
    QDateTime minTime, maxTime;

    for (const auto &m : measurements) {
        if (m.value < min) {
            min = m.value;
            minTime = m.date;
        }
        if (m.value > max) {
            max = m.value;
            maxTime = m.date;
        }
        sum += m.value;
    }

    double avg = sum / measurements.size();

    QString stats = ui->infoText->toHtml();
    stats += "<hr><b>Statystyki:</b><br>";
    stats += QString("Min: %1 (%2)<br>").arg(min).arg(minTime.toString("dd.MM.yyyy hh:mm"));
    stats += QString("Max: %1 (%2)<br>").arg(max).arg(maxTime.toString("dd.MM.yyyy hh:mm"));
    stats += QString("Średnia: %1").arg(avg);

    ui->infoText->setHtml(stats);
}
