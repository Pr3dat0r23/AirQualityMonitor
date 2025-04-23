#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <QMessageBox>
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handleNetworkReply);

    // Konfiguracja wykresu
    QChart *chart = new QChart();
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

MainWindow::~MainWindow()
{
    delete ui;
}

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

void MainWindow::handleNetworkReply(QNetworkReply *reply)
{
    // Obsługa błędów sieciowych
    if (reply->error() != QNetworkReply::NoError) {
        showError(QString("Błąd sieciowy: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    // Odczyt odpowiedzi
    QByteArray data = reply->readAll();
    reply->deleteLater();


    try {
        // Parsowanie JSON
        json response;
        try {
            response = json::parse(data.toStdString());
        } catch (const json::parse_error &e) {
            showError(QString("Błąd parsowania JSON: %1").arg(e.what()));
            return;
        }

        // Rozpoznawanie typu odpowiedzi
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

void MainWindow::processStationsData(const json &data)
{
    ui->stationList->clear();
    stations.clear();

    QString location = ui->locationEdit->text().trimmed().toLower();

    for (const auto &item : data) {
        QString cityName = QString::fromStdString(item["city"]["name"].get<std::string>()).toLower();

        if (!location.isEmpty() && !cityName.contains(location))
            continue; // filtruj po lokalizacji

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


void MainWindow::on_stationList_currentRowChanged(int index)
{
    if (index < 0 || index >= stations.size()) return;

    currentStation = stations[index];

    // Aktualizacja informacji o stacji
    ui->infoText->setHtml(QString("<b>%1</b><br>Adres: %2<br>Współrzędne: %3, %4")
                              .arg(currentStation.name)
                              .arg(currentStation.address)
                              .arg(currentStation.lat)
                              .arg(currentStation.lon));

    // Pobranie czujników dla stacji
    QString apiUrl = QString("https://api.gios.gov.pl/pjp-api/rest/station/sensors/%1").arg(currentStation.id);
    networkManager->get(QNetworkRequest(QUrl(apiUrl)));
}

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

void MainWindow::on_sensorList_currentRowChanged(int index)
{
    if (index < 0 || index >= sensors.size()) return;

    currentSensor = sensors[index];

    // Pobranie danych pomiarowych
    QString apiUrl = QString("https://api.gios.gov.pl/pjp-api/rest/data/getData/%1").arg(currentSensor.id);
    networkManager->get(QNetworkRequest(QUrl(apiUrl)));
}

void MainWindow::processMeasurementsData(const json &data)
{
    // Czyszczenie poprzednich danych
    measurements.clear();
    ui->chartView->chart()->removeAllSeries();
    ui->statsTable->clearContents();
    ui->statsTable->setRowCount(0);

    // Sprawdzenie podstawowej struktury danych
    if (!data.contains("values") || !data["values"].is_array()) {
        showError("Otrzymano nieprawidłową strukturę danych pomiarowych");
        return;
    }

    // Inicjalizacja serii danych i statystyk
    QLineSeries *series = new QLineSeries();
    QVector<double> validValues;
    int rowCounter = 0;

    // Przetwarzanie każdego rekordu
    for (const auto& item : data["values"]) {
        try {
            // Pominięcie rekordów z brakującymi danymi
            if (!item.contains("date") || !item.contains("value")) {
                qDebug() << "Pominięto rekord z brakującymi polami";
                continue;
            }

            // Parsowanie daty
            QDateTime dateTime = QDateTime::fromString(
                QString::fromStdString(item["date"].get<std::string>()),
                Qt::ISODate
                );
            if (!dateTime.isValid()) {
                qDebug() << "Pominięto rekord z nieprawidłową datą";
                continue;
            }

            // Uniwersalna konwersja wartości
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

            // Pominięcie nieprawidłowych wartości
            if (!valueValid || std::isnan(value) || std::isinf(value)) {
                qDebug() << "Pominięto nieprawidłową wartość pomiaru";
                continue;
            }

            // Dodanie prawidłowego pomiaru
            Measurement measurement;
            measurement.date = dateTime;
            measurement.value = value;
            measurements.append(measurement);

            // Dodanie punktu do wykresu
            series->append(dateTime.toMSecsSinceEpoch(), value);
            validValues.append(value);

            // Dodanie wiersza do tabeli
            ui->statsTable->insertRow(rowCounter);
            ui->statsTable->setItem(rowCounter, 0, new QTableWidgetItem(dateTime.toString("yyyy-MM-dd hh:mm")));
            ui->statsTable->setItem(rowCounter, 1, new QTableWidgetItem(QString::number(value, 'f', 2)));
            rowCounter++;

        } catch (const std::exception& e) {
            qDebug() << "Błąd przetwarzania rekordu:" << e.what();
            continue;
        }
    }

    // Sprawdzenie czy są jakieś dane do wyświetlenia
    if (measurements.empty()) {
        showError("Nie znaleziono żadnych prawidłowych danych pomiarowych");
        return;
    }

    // Konfiguracja wykresu
    QChart *chart = ui->chartView->chart();
    chart->addSeries(series);

    // Ustawienia osi czasu
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("dd.MM hh:mm");
    axisX->setTitleText("Data i godzina");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Ustawienia osi wartości
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("µg/m³");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Obliczenie i wyświetlenie statystyk
    if (!validValues.empty()) {
        auto [minIt, maxIt] = std::minmax_element(validValues.begin(), validValues.end());
        double avg = std::accumulate(validValues.begin(), validValues.end(), 0.0) / validValues.size();

        ui->minValueLabel->setText(QString::number(*minIt, 'f', 2));
        ui->maxValueLabel->setText(QString::number(*maxIt, 'f', 2));
        ui->avgValueLabel->setText(QString::number(avg, 'f', 2));
    }

    // Aktualizacja interfejsu
    chart->legend()->setVisible(false);
    chart->setTitle("Dane pomiarowe: " + currentSensorName);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

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

void MainWindow::on_loadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Wczytaj dane", "", "JSON (*.json)");
    if (fileName.isEmpty()) return;

    loadFromJson(fileName);
}

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

void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, "Błąd", message);
}
