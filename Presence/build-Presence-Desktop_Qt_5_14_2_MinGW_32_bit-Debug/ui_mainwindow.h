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
    QLabel *label;
    QLineEdit *lineEdit_User_Name;
    QPushButton *pushButton_Login;
    QPushButton *pushButton_Cancel;
    QLineEdit *lineEdit_Password;
    QLabel *label_2;
    QFrame *frame;
    QLabel *label_6;
    QLabel *label_3;
    QCheckBox *checkBox;
    QLabel *label_5;
    QPushButton *pushButton_Backup;
    QLabel *label_Derniere_connexion;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(531, 714);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 160, 201, 41));
        QFont font;
        font.setPointSize(17);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEdit_User_Name = new QLineEdit(centralwidget);
        lineEdit_User_Name->setObjectName(QString::fromUtf8("lineEdit_User_Name"));
        lineEdit_User_Name->setGeometry(QRect(59, 220, 391, 41));
        QFont font1;
        font1.setPointSize(15);
        font1.setBold(false);
        font1.setWeight(50);
        lineEdit_User_Name->setFont(font1);
        pushButton_Login = new QPushButton(centralwidget);
        pushButton_Login->setObjectName(QString::fromUtf8("pushButton_Login"));
        pushButton_Login->setGeometry(QRect(160, 530, 191, 61));
        QFont font2;
        font2.setPointSize(20);
        font2.setBold(true);
        font2.setWeight(75);
        pushButton_Login->setFont(font2);
        pushButton_Login->setContextMenuPolicy(Qt::PreventContextMenu);
        pushButton_Login->setStyleSheet(QString::fromUtf8("QPushButton{\n"
" background-color: #74a1e8;\n"
" color: white;\n"
" border-radius: 20px;\n"
" font-weight: bold;\n"
"}"));
        pushButton_Login->setCheckable(false);
        pushButton_Login->setAutoDefault(false);
        pushButton_Cancel = new QPushButton(centralwidget);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));
        pushButton_Cancel->setGeometry(QRect(180, 610, 151, 41));
        QFont font3;
        font3.setPointSize(15);
        font3.setBold(true);
        font3.setWeight(75);
        pushButton_Cancel->setFont(font3);
        pushButton_Cancel->setStyleSheet(QString::fromUtf8("QPushButton{\n"
" background-color: #b6a3f0;\n"
" color: white;\n"
" border-radius: 20px;\n"
" font-weight: bold;\n"
"}"));
        lineEdit_Password = new QLineEdit(centralwidget);
        lineEdit_Password->setObjectName(QString::fromUtf8("lineEdit_Password"));
        lineEdit_Password->setGeometry(QRect(60, 370, 391, 41));
        lineEdit_Password->setFont(font1);
        lineEdit_Password->setEchoMode(QLineEdit::Password);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 310, 211, 41));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(-10, 0, 551, 101));
        frame->setStyleSheet(QString::fromUtf8("QFrame{\n"
" background-color: #74a1e8;\n"
" border-radius: 20px;\n"
"}"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_6 = new QLabel(frame);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 10, 221, 21));
        QFont font4;
        font4.setPointSize(13);
        font4.setUnderline(false);
        label_6->setFont(font4);
        label_6->setStyleSheet(QString::fromUtf8("QLabel{\n"
" color: #d9eafc;\n"
"}"));
        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 40, 501, 41));
        QFont font5;
        font5.setPointSize(16);
        font5.setBold(true);
        font5.setWeight(75);
        label_3->setFont(font5);
        label_3->setStyleSheet(QString::fromUtf8("QLabel{\n"
" color: #d9eafc;\n"
"}"));
        label_3->setAlignment(Qt::AlignCenter);
        checkBox = new QCheckBox(centralwidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(20, 430, 261, 31));
        QFont font6;
        font6.setPointSize(11);
        font6.setBold(true);
        font6.setWeight(75);
        checkBox->setFont(font6);
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(170, 480, 171, 20));
        QFont font7;
        font7.setPointSize(10);
        font7.setUnderline(true);
        label_5->setFont(font7);
        label_5->setStyleSheet(QString::fromUtf8("QLabel{\n"
" color: #949494;\n"
"}"));
        pushButton_Backup = new QPushButton(centralwidget);
        pushButton_Backup->setObjectName(QString::fromUtf8("pushButton_Backup"));
        pushButton_Backup->setGeometry(QRect(370, 630, 161, 31));
        label_Derniere_connexion = new QLabel(centralwidget);
        label_Derniere_connexion->setObjectName(QString::fromUtf8("label_Derniere_connexion"));
        label_Derniere_connexion->setGeometry(QRect(10, 110, 241, 16));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 531, 26));
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
        label->setText(QCoreApplication::translate("MainWindow", "Identifiant : ", nullptr));
        pushButton_Login->setText(QCoreApplication::translate("MainWindow", "Connecter", nullptr));
        pushButton_Cancel->setText(QCoreApplication::translate("MainWindow", "Annuler", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Mot de passe :", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Acc\303\250s Utilisateur", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Supervision d'une salle de sport", nullptr));
        checkBox->setText(QCoreApplication::translate("MainWindow", "Afficher le mot de passe ", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Mot de passe oubli\303\251 ?", nullptr));
        pushButton_Backup->setText(QCoreApplication::translate("MainWindow", "entrer les codes backup", nullptr));
        label_Derniere_connexion->setText(QCoreApplication::translate("MainWindow", "Derni\303\250re connexion: ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
