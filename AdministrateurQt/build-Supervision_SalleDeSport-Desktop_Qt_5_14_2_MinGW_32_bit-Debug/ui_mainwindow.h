/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QLabel *label_Username;
    QLineEdit *lineEdit_Username_2;
    QPushButton *pushButton_Cancel;
    QLabel *label_Password;
    QPushButton *pushButton_Login;
    QLineEdit *lineEdit_Password_2;
    QLabel *label_Copyright;
    QLabel *label_Admin;
    QLabel *label_Supervision;
    QCheckBox *checkBox_AfficherMDP;
    QLabel *label_Password_Forgot;
    QFrame *frame;
    QFrame *frame_2;
    QLabel *label_Backup;
    QLabel *label_DerniereConnexion;
    QFrame *frame_3;
    QFrame *frame_4;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(458, 605);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 451, 561));
        label_Username = new QLabel(groupBox);
        label_Username->setObjectName(QString::fromUtf8("label_Username"));
        label_Username->setGeometry(QRect(30, 80, 161, 61));
        QFont font;
        font.setPointSize(16);
        label_Username->setFont(font);
        label_Username->setLayoutDirection(Qt::LeftToRight);
        label_Username->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEdit_Username_2 = new QLineEdit(groupBox);
        lineEdit_Username_2->setObjectName(QString::fromUtf8("lineEdit_Username_2"));
        lineEdit_Username_2->setGeometry(QRect(80, 140, 271, 31));
        QFont font1;
        font1.setPointSize(15);
        lineEdit_Username_2->setFont(font1);
        lineEdit_Username_2->setEchoMode(QLineEdit::Normal);
        pushButton_Cancel = new QPushButton(groupBox);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));
        pushButton_Cancel->setGeometry(QRect(180, 450, 101, 31));
        QFont font2;
        font2.setPointSize(12);
        font2.setBold(true);
        font2.setWeight(75);
        pushButton_Cancel->setFont(font2);
        pushButton_Cancel->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #91affa, stop:1 #bdcffc);\n"
"    color: white;\n"
"    border-radius: 20px;\n"
"    font-weight: bold;\n"
"}"));
        label_Password = new QLabel(groupBox);
        label_Password->setObjectName(QString::fromUtf8("label_Password"));
        label_Password->setGeometry(QRect(60, 180, 161, 61));
        label_Password->setFont(font);
        label_Password->setLayoutDirection(Qt::LeftToRight);
        label_Password->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pushButton_Login = new QPushButton(groupBox);
        pushButton_Login->setObjectName(QString::fromUtf8("pushButton_Login"));
        pushButton_Login->setGeometry(QRect(140, 390, 181, 51));
        pushButton_Login->setFont(font2);
        pushButton_Login->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #6e97ff, stop:1 #bdcffc);\n"
"    color: white;\n"
"    border-radius: 20px;\n"
"    font-weight: bold;\n"
"}"));
        lineEdit_Password_2 = new QLineEdit(groupBox);
        lineEdit_Password_2->setObjectName(QString::fromUtf8("lineEdit_Password_2"));
        lineEdit_Password_2->setGeometry(QRect(80, 240, 271, 31));
        lineEdit_Password_2->setFont(font1);
        lineEdit_Password_2->setEchoMode(QLineEdit::Password);
        label_Copyright = new QLabel(groupBox);
        label_Copyright->setObjectName(QString::fromUtf8("label_Copyright"));
        label_Copyright->setGeometry(QRect(10, 530, 321, 16));
        label_Admin = new QLabel(groupBox);
        label_Admin->setObjectName(QString::fromUtf8("label_Admin"));
        label_Admin->setGeometry(QRect(20, 20, 161, 16));
        QFont font3;
        font3.setPointSize(12);
        label_Admin->setFont(font3);
        label_Admin->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    color: #f0f6ff;\n"
"}"));
        label_Supervision = new QLabel(groupBox);
        label_Supervision->setObjectName(QString::fromUtf8("label_Supervision"));
        label_Supervision->setGeometry(QRect(20, 40, 261, 21));
        QFont font4;
        font4.setPointSize(14);
        font4.setBold(true);
        font4.setUnderline(false);
        font4.setWeight(75);
        label_Supervision->setFont(font4);
        label_Supervision->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    color: #FFFFFF;\n"
"}"));
        checkBox_AfficherMDP = new QCheckBox(groupBox);
        checkBox_AfficherMDP->setObjectName(QString::fromUtf8("checkBox_AfficherMDP"));
        checkBox_AfficherMDP->setGeometry(QRect(90, 280, 141, 17));
        label_Password_Forgot = new QLabel(groupBox);
        label_Password_Forgot->setObjectName(QString::fromUtf8("label_Password_Forgot"));
        label_Password_Forgot->setGeometry(QRect(160, 320, 141, 16));
        QFont font5;
        font5.setPointSize(10);
        font5.setUnderline(true);
        label_Password_Forgot->setFont(font5);
        label_Password_Forgot->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    color: #5B7FD4;\n"
"    text-decoration: underline;\n"
"}"));
        label_Password_Forgot->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);
        frame = new QFrame(groupBox);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setEnabled(false);
        frame->setGeometry(QRect(-10, -10, 511, 81));
        frame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #6e97ff, stop:1 #c7d7fc);\n"
"    border-radius: 10px;\n"
"}"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        frame_2 = new QFrame(groupBox);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setEnabled(false);
        frame_2->setGeometry(QRect(406, 70, 31, 491));
        frame_2->setStyleSheet(QString::fromUtf8("QFrame {\n"
"     background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #d7e1fa, stop:1 #ffffff);\n"
"}"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        label_Backup = new QLabel(groupBox);
        label_Backup->setObjectName(QString::fromUtf8("label_Backup"));
        label_Backup->setGeometry(QRect(330, 530, 71, 16));
        QFont font6;
        font6.setPointSize(8);
        font6.setBold(false);
        font6.setItalic(true);
        font6.setUnderline(true);
        font6.setWeight(50);
        font6.setStrikeOut(false);
        font6.setKerning(false);
        label_Backup->setFont(font6);
        label_Backup->setStyleSheet(QString::fromUtf8("QLabel {\n"
"   color: #ff1100;\n"
"    text-decoration: underline;\n"
"}"));
        label_Backup->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);
        label_DerniereConnexion = new QLabel(groupBox);
        label_DerniereConnexion->setObjectName(QString::fromUtf8("label_DerniereConnexion"));
        label_DerniereConnexion->setGeometry(QRect(10, 70, 411, 16));
        label_DerniereConnexion->setStyleSheet(QString::fromUtf8("QLabel {\n"
"   color: #5c5c5c;\n"
"    text-decoration: underline;\n"
"}"));
        frame_3 = new QFrame(groupBox);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setEnabled(false);
        frame_3->setGeometry(QRect(80, 160, 271, 16));
        frame_3->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #bdcffc, stop:1 #f2f6ff);\n"
"    border-radius: 10px;\n"
"}"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        frame_4 = new QFrame(groupBox);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setEnabled(false);
        frame_4->setGeometry(QRect(80, 260, 271, 16));
        frame_4->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #bdcffc, stop:1 #f2f6ff);\n"
"    border-radius: 10px;\n"
"}"));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        frame_2->raise();
        frame_4->raise();
        frame_3->raise();
        frame->raise();
        label_Username->raise();
        lineEdit_Username_2->raise();
        pushButton_Cancel->raise();
        label_Password->raise();
        pushButton_Login->raise();
        lineEdit_Password_2->raise();
        label_Copyright->raise();
        label_Admin->raise();
        label_Supervision->raise();
        checkBox_AfficherMDP->raise();
        label_Password_Forgot->raise();
        label_Backup->raise();
        label_DerniereConnexion->raise();
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 458, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        label_Username->setText(QCoreApplication::translate("MainWindow", "Identifiant :", nullptr));
        pushButton_Cancel->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        label_Password->setText(QCoreApplication::translate("MainWindow", "Mot De Passe :", nullptr));
        pushButton_Login->setText(QCoreApplication::translate("MainWindow", "Connexion", nullptr));
        label_Copyright->setText(QCoreApplication::translate("MainWindow", "@Copyright 2026 - All rights reserved - The BasicSlimFit Industry", nullptr));
        label_Admin->setText(QCoreApplication::translate("MainWindow", "Acc\303\250s Administrateur", nullptr));
        label_Supervision->setText(QCoreApplication::translate("MainWindow", "Supervision Salle De Sport", nullptr));
        checkBox_AfficherMDP->setText(QCoreApplication::translate("MainWindow", "Afficher Mot De Passe", nullptr));
        label_Password_Forgot->setText(QCoreApplication::translate("MainWindow", "<a href=\"forgot\">Mot De Passe Oubli\303\251 ?</a>", nullptr));
        label_Backup->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><a href=\"forgot\"><span style=\" text-decoration: underline; color:#0000ff;\">Code Backup</span></a></p></body></html>", nullptr));
        label_DerniereConnexion->setText(QCoreApplication::translate("MainWindow", "Derni\303\250re Connexion :", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
