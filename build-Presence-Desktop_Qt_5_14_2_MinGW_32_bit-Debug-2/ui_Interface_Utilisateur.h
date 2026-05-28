/********************************************************************************
** Form generated from reading UI file 'Interface_Utilisateur.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERFACE_UTILISATEUR_H
#define UI_INTERFACE_UTILISATEUR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLabel *label;
    QPushButton *pushButton_Update;
    QPushButton *pushButton_Cancel;
    QPushButton *pushButton_Warn;
    QPushButton *pushButton_History;
    QTableWidget *tableWidget;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(712, 442);
        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(280, 0, 131, 71));
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setToolTipDuration(1);
        label->setAlignment(Qt::AlignCenter);
        pushButton_Update = new QPushButton(Dialog);
        pushButton_Update->setObjectName(QString::fromUtf8("pushButton_Update"));
        pushButton_Update->setGeometry(QRect(10, 350, 181, 51));
        QFont font1;
        font1.setPointSize(14);
        font1.setBold(true);
        font1.setWeight(75);
        pushButton_Update->setFont(font1);
        pushButton_Update->setAutoDefault(false);
        pushButton_Cancel = new QPushButton(Dialog);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));
        pushButton_Cancel->setGeometry(QRect(210, 350, 121, 51));
        pushButton_Cancel->setFont(font1);
        pushButton_Cancel->setAutoDefault(false);
        pushButton_Warn = new QPushButton(Dialog);
        pushButton_Warn->setObjectName(QString::fromUtf8("pushButton_Warn"));
        pushButton_Warn->setGeometry(QRect(380, 350, 121, 51));
        pushButton_Warn->setFont(font1);
        pushButton_Warn->setAutoDefault(false);
        pushButton_History = new QPushButton(Dialog);
        pushButton_History->setObjectName(QString::fromUtf8("pushButton_History"));
        pushButton_History->setGeometry(QRect(530, 350, 151, 51));
        pushButton_History->setFont(font1);
        pushButton_History->setAutoDefault(false);
        tableWidget = new QTableWidget(Dialog);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 70, 661, 241));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("Dialog", "Pr\303\251sence", nullptr));
        pushButton_Update->setText(QCoreApplication::translate("Dialog", "Mettre \303\240 jour", nullptr));
        pushButton_Cancel->setText(QCoreApplication::translate("Dialog", "Fermer", nullptr));
        pushButton_Warn->setText(QCoreApplication::translate("Dialog", "Alerter", nullptr));
        pushButton_History->setText(QCoreApplication::translate("Dialog", "Historique", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERFACE_UTILISATEUR_H
