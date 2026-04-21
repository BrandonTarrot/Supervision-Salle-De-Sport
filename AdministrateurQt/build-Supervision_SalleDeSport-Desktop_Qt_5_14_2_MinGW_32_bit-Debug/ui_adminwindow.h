/********************************************************************************
** Form generated from reading UI file 'adminwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADMINWINDOW_H
#define UI_ADMINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AdminWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QLabel *label_Titre;
    QLabel *label_Logo;
    QTableView *tableView_Affichage;
    QPushButton *pushButton_Affichage;
    QPushButton *pushButton_Historique;
    QPushButton *pushButton_Gestion;
    QPushButton *pushButton_Luminosite;
    QPushButton *pushButton_Temperature;
    QPushButton *pushButton_Presence;
    QPushButton *pushButton_Actualiser;
    QTableWidget *tableWidget_Gestion;
    QTableView *tableView_Historique;
    QFrame *frame;
    QLabel *label_Ventilateur;
    QPushButton *pushButton_Fermer;
    QLabel *bulb_off;
    QLabel *bulb_on;
    QLabel *label_Temperature;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *AdminWindow)
    {
        if (AdminWindow->objectName().isEmpty())
            AdminWindow->setObjectName(QString::fromUtf8("AdminWindow"));
        AdminWindow->resize(754, 541);
        centralwidget = new QWidget(AdminWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 751, 571));
        label_Titre = new QLabel(groupBox);
        label_Titre->setObjectName(QString::fromUtf8("label_Titre"));
        label_Titre->setGeometry(QRect(40, 10, 151, 91));
        QFont font;
        font.setPointSize(15);
        label_Titre->setFont(font);
        label_Logo = new QLabel(groupBox);
        label_Logo->setObjectName(QString::fromUtf8("label_Logo"));
        label_Logo->setGeometry(QRect(180, 30, 51, 51));
        label_Logo->setPixmap(QPixmap(QString::fromUtf8(":/images/Ressources/AdminLogo.jpg")));
        label_Logo->setScaledContents(true);
        tableView_Affichage = new QTableView(groupBox);
        tableView_Affichage->setObjectName(QString::fromUtf8("tableView_Affichage"));
        tableView_Affichage->setGeometry(QRect(60, 140, 571, 311));
        pushButton_Affichage = new QPushButton(groupBox);
        pushButton_Affichage->setObjectName(QString::fromUtf8("pushButton_Affichage"));
        pushButton_Affichage->setGeometry(QRect(60, 100, 181, 41));
        pushButton_Historique = new QPushButton(groupBox);
        pushButton_Historique->setObjectName(QString::fromUtf8("pushButton_Historique"));
        pushButton_Historique->setGeometry(QRect(440, 100, 191, 41));
        pushButton_Gestion = new QPushButton(groupBox);
        pushButton_Gestion->setObjectName(QString::fromUtf8("pushButton_Gestion"));
        pushButton_Gestion->setGeometry(QRect(240, 100, 201, 41));
        pushButton_Luminosite = new QPushButton(groupBox);
        pushButton_Luminosite->setObjectName(QString::fromUtf8("pushButton_Luminosite"));
        pushButton_Luminosite->setGeometry(QRect(60, 460, 171, 21));
        pushButton_Temperature = new QPushButton(groupBox);
        pushButton_Temperature->setObjectName(QString::fromUtf8("pushButton_Temperature"));
        pushButton_Temperature->setGeometry(QRect(260, 460, 171, 21));
        pushButton_Presence = new QPushButton(groupBox);
        pushButton_Presence->setObjectName(QString::fromUtf8("pushButton_Presence"));
        pushButton_Presence->setGeometry(QRect(460, 460, 171, 21));
        pushButton_Actualiser = new QPushButton(groupBox);
        pushButton_Actualiser->setObjectName(QString::fromUtf8("pushButton_Actualiser"));
        pushButton_Actualiser->setGeometry(QRect(640, 140, 91, 31));
        tableWidget_Gestion = new QTableWidget(groupBox);
        tableWidget_Gestion->setObjectName(QString::fromUtf8("tableWidget_Gestion"));
        tableWidget_Gestion->setGeometry(QRect(60, 140, 571, 311));
        tableView_Historique = new QTableView(groupBox);
        tableView_Historique->setObjectName(QString::fromUtf8("tableView_Historique"));
        tableView_Historique->setGeometry(QRect(60, 140, 571, 311));
        frame = new QFrame(groupBox);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setEnabled(false);
        frame->setGeometry(QRect(40, 140, 20, 311));
        frame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    background-color: #84a6ff;\n"
"}"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_Ventilateur = new QLabel(groupBox);
        label_Ventilateur->setObjectName(QString::fromUtf8("label_Ventilateur"));
        label_Ventilateur->setGeometry(QRect(660, 180, 61, 61));
        label_Ventilateur->setPixmap(QPixmap(QString::fromUtf8(":/images/Ressources/ventilo_on.gif")));
        label_Ventilateur->setScaledContents(true);
        pushButton_Fermer = new QPushButton(groupBox);
        pushButton_Fermer->setObjectName(QString::fromUtf8("pushButton_Fermer"));
        pushButton_Fermer->setGeometry(QRect(650, 460, 75, 23));
        bulb_off = new QLabel(groupBox);
        bulb_off->setObjectName(QString::fromUtf8("bulb_off"));
        bulb_off->setGeometry(QRect(680, 300, 31, 51));
        bulb_off->setPixmap(QPixmap(QString::fromUtf8(":/images/Ressources/bulb_off.png")));
        bulb_off->setScaledContents(true);
        bulb_on = new QLabel(groupBox);
        bulb_on->setObjectName(QString::fromUtf8("bulb_on"));
        bulb_on->setGeometry(QRect(680, 300, 31, 51));
        bulb_on->setPixmap(QPixmap(QString::fromUtf8(":/images/Ressources/bulb_on.png")));
        bulb_on->setScaledContents(true);
        label_Temperature = new QLabel(groupBox);
        label_Temperature->setObjectName(QString::fromUtf8("label_Temperature"));
        label_Temperature->setGeometry(QRect(680, 250, 41, 20));
        tableView_Affichage->raise();
        label_Titre->raise();
        label_Logo->raise();
        pushButton_Affichage->raise();
        pushButton_Historique->raise();
        pushButton_Gestion->raise();
        pushButton_Luminosite->raise();
        pushButton_Temperature->raise();
        pushButton_Presence->raise();
        pushButton_Actualiser->raise();
        tableWidget_Gestion->raise();
        tableView_Historique->raise();
        frame->raise();
        label_Ventilateur->raise();
        pushButton_Fermer->raise();
        bulb_off->raise();
        bulb_on->raise();
        label_Temperature->raise();
        AdminWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(AdminWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 754, 21));
        AdminWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(AdminWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        AdminWindow->setStatusBar(statusbar);

        retranslateUi(AdminWindow);

        QMetaObject::connectSlotsByName(AdminWindow);
    } // setupUi

    void retranslateUi(QMainWindow *AdminWindow)
    {
        AdminWindow->setWindowTitle(QCoreApplication::translate("AdminWindow", "MainWindow", nullptr));
        groupBox->setTitle(QCoreApplication::translate("AdminWindow", "GroupBox", nullptr));
        label_Titre->setText(QCoreApplication::translate("AdminWindow", "Administrateur", nullptr));
        label_Logo->setText(QString());
        pushButton_Affichage->setText(QCoreApplication::translate("AdminWindow", "Affichage", nullptr));
        pushButton_Historique->setText(QCoreApplication::translate("AdminWindow", "Historique", nullptr));
        pushButton_Gestion->setText(QCoreApplication::translate("AdminWindow", "Gestion", nullptr));
        pushButton_Luminosite->setText(QCoreApplication::translate("AdminWindow", "Luminosit\303\251", nullptr));
        pushButton_Temperature->setText(QCoreApplication::translate("AdminWindow", "Temp\303\251rature / Humidit\303\251", nullptr));
        pushButton_Presence->setText(QCoreApplication::translate("AdminWindow", "Pr\303\251sence", nullptr));
        pushButton_Actualiser->setText(QCoreApplication::translate("AdminWindow", "Actualiser", nullptr));
        label_Ventilateur->setText(QString());
        pushButton_Fermer->setText(QCoreApplication::translate("AdminWindow", "Fermer", nullptr));
        bulb_off->setText(QString());
        bulb_on->setText(QString());
        label_Temperature->setText(QCoreApplication::translate("AdminWindow", "temperature", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdminWindow: public Ui_AdminWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADMINWINDOW_H
