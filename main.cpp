/**
 * @file main.cpp
 * @brief Główny plik źródłowy aplikacji
 * @author Piotr Trzeciak
 * @date 2025
 */

#include "mainwindow.h"
#include <QApplication>
#include <windows.h>

/**
 * @brief Główna funkcja aplikacji
 * @param argc Liczba argumentów
 * @param argv Tablica argumentów
 * @return Kod wyjścia aplikacji
 */
int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    if (!qEnvironmentVariableIsSet("QT_DEBUG")) {
        FreeConsole(); // Ukryj konsolę na Windows
    }
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
