/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *locationEdit;
    QPushButton *searchButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QSplitter *splitter;
    QWidget *leftPanel;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QListWidget *stationList;
    QLabel *label_3;
    QListWidget *sensorList;
    QLabel *minValueLabel;
    QLabel *maxValueLabel;
    QLabel *avgValueLabel;
    QLabel *label_4;
    QTextEdit *infoText;
    QChartView *chartView;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 700);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(centralwidget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        locationEdit = new QLineEdit(centralwidget);
        locationEdit->setObjectName("locationEdit");

        horizontalLayout->addWidget(locationEdit);

        searchButton = new QPushButton(centralwidget);
        searchButton->setObjectName("searchButton");

        horizontalLayout->addWidget(searchButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        saveButton = new QPushButton(centralwidget);
        saveButton->setObjectName("saveButton");

        horizontalLayout->addWidget(saveButton);

        loadButton = new QPushButton(centralwidget);
        loadButton->setObjectName("loadButton");

        horizontalLayout->addWidget(loadButton);


        verticalLayout->addLayout(horizontalLayout);

        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        leftPanel = new QWidget(splitter);
        leftPanel->setObjectName("leftPanel");
        verticalLayout_2 = new QVBoxLayout(leftPanel);
        verticalLayout_2->setObjectName("verticalLayout_2");
        label_2 = new QLabel(leftPanel);
        label_2->setObjectName("label_2");

        verticalLayout_2->addWidget(label_2);

        stationList = new QListWidget(leftPanel);
        stationList->setObjectName("stationList");
        stationList->setMinimumSize(QSize(250, 0));

        verticalLayout_2->addWidget(stationList);

        label_3 = new QLabel(leftPanel);
        label_3->setObjectName("label_3");

        verticalLayout_2->addWidget(label_3);

        sensorList = new QListWidget(leftPanel);
        sensorList->setObjectName("sensorList");
        sensorList->setMinimumSize(QSize(250, 0));

        verticalLayout_2->addWidget(sensorList);

        minValueLabel = new QLabel(leftPanel);
        minValueLabel->setObjectName("minValueLabel");

        verticalLayout_2->addWidget(minValueLabel);

        maxValueLabel = new QLabel(leftPanel);
        maxValueLabel->setObjectName("maxValueLabel");

        verticalLayout_2->addWidget(maxValueLabel);

        avgValueLabel = new QLabel(leftPanel);
        avgValueLabel->setObjectName("avgValueLabel");

        verticalLayout_2->addWidget(avgValueLabel);

        label_4 = new QLabel(leftPanel);
        label_4->setObjectName("label_4");

        verticalLayout_2->addWidget(label_4);

        infoText = new QTextEdit(leftPanel);
        infoText->setObjectName("infoText");
        infoText->setReadOnly(true);

        verticalLayout_2->addWidget(infoText);

        splitter->addWidget(leftPanel);
        chartView = new QChartView(splitter);
        chartView->setObjectName("chartView");
        splitter->addWidget(chartView);

        verticalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1000, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Monitor Jako\305\233ci Powietrza", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Lokalizacja:", nullptr));
        locationEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "np. Warszawa, Krak\303\263w", nullptr));
        searchButton->setText(QCoreApplication::translate("MainWindow", "Szukaj stacji", nullptr));
        saveButton->setText(QCoreApplication::translate("MainWindow", "Zapisz dane", nullptr));
        loadButton->setText(QCoreApplication::translate("MainWindow", "Wczytaj dane", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Stacje pomiarowe:", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Czujniki:", nullptr));
        minValueLabel->setText(QCoreApplication::translate("MainWindow", "Min:", nullptr));
        maxValueLabel->setText(QCoreApplication::translate("MainWindow", "Max:", nullptr));
        avgValueLabel->setText(QCoreApplication::translate("MainWindow", "\305\232rednia:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Informacje:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
