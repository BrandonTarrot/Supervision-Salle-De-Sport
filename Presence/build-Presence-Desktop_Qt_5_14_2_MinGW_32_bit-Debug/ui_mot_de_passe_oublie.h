/********************************************************************************
** Form generated from reading UI file 'mot_de_passe_oublie.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOT_DE_PASSE_OUBLIE_H
#define UI_MOT_DE_PASSE_OUBLIE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLabel *label_Titre;
    QLineEdit *lineEdit_Mail;
    QPushButton *pushButton_Confirmer;
    QPushButton *pushButton_Retour;
    QLabel *label_Message;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(552, 337);
        label_Titre = new QLabel(Dialog);
        label_Titre->setObjectName(QString::fromUtf8("label_Titre"));
        label_Titre->setGeometry(QRect(180, 30, 231, 20));
        QFont font;
        font.setPointSize(11);
        font.setBold(false);
        font.setWeight(50);
        label_Titre->setFont(font);
        lineEdit_Mail = new QLineEdit(Dialog);
        lineEdit_Mail->setObjectName(QString::fromUtf8("lineEdit_Mail"));
        lineEdit_Mail->setGeometry(QRect(100, 130, 381, 31));
        QFont font1;
        font1.setPointSize(1);
        lineEdit_Mail->setFont(font1);
        pushButton_Confirmer = new QPushButton(Dialog);
        pushButton_Confirmer->setObjectName(QString::fromUtf8("pushButton_Confirmer"));
        pushButton_Confirmer->setGeometry(QRect(100, 250, 101, 41));
        pushButton_Retour = new QPushButton(Dialog);
        pushButton_Retour->setObjectName(QString::fromUtf8("pushButton_Retour"));
        pushButton_Retour->setGeometry(QRect(350, 250, 101, 41));
        label_Message = new QLabel(Dialog);
        label_Message->setObjectName(QString::fromUtf8("label_Message"));
        label_Message->setGeometry(QRect(100, 195, 231, 21));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        label_Titre->setText(QCoreApplication::translate("Dialog", "Inscrivez votre adresse mail", nullptr));
        pushButton_Confirmer->setText(QCoreApplication::translate("Dialog", "Retour", nullptr));
        pushButton_Retour->setText(QCoreApplication::translate("Dialog", "Confirmer", nullptr));
        label_Message->setText(QCoreApplication::translate("Dialog", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOT_DE_PASSE_OUBLIE_H
