/********************************************************************************
** Form generated from reading UI file 'forgotpassword.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORGOTPASSWORD_H
#define UI_FORGOTPASSWORD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_forgotpassword
{
public:
    QLabel *label_Titre;
    QLineEdit *lineEdit_Identifiant;
    QLineEdit *lineEdit_2;
    QLabel *label_id;
    QLabel *label_mdp;
    QPushButton *pushButton_Retour;
    QPushButton *pushButton_Confirmer;
    QLabel *label_Message;
    QLineEdit *lineEdit_Mail;

    void setupUi(QDialog *forgotpassword)
    {
        if (forgotpassword->objectName().isEmpty())
            forgotpassword->setObjectName(QString::fromUtf8("forgotpassword"));
        forgotpassword->resize(381, 415);
        label_Titre = new QLabel(forgotpassword);
        label_Titre->setObjectName(QString::fromUtf8("label_Titre"));
        label_Titre->setGeometry(QRect(120, 60, 131, 16));
        lineEdit_Identifiant = new QLineEdit(forgotpassword);
        lineEdit_Identifiant->setObjectName(QString::fromUtf8("lineEdit_Identifiant"));
        lineEdit_Identifiant->setGeometry(QRect(80, 330, 291, 21));
        lineEdit_2 = new QLineEdit(forgotpassword);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(80, 390, 291, 21));
        label_id = new QLabel(forgotpassword);
        label_id->setObjectName(QString::fromUtf8("label_id"));
        label_id->setGeometry(QRect(80, 310, 101, 16));
        label_mdp = new QLabel(forgotpassword);
        label_mdp->setObjectName(QString::fromUtf8("label_mdp"));
        label_mdp->setGeometry(QRect(80, 370, 121, 16));
        pushButton_Retour = new QPushButton(forgotpassword);
        pushButton_Retour->setObjectName(QString::fromUtf8("pushButton_Retour"));
        pushButton_Retour->setGeometry(QRect(50, 210, 91, 23));
        pushButton_Confirmer = new QPushButton(forgotpassword);
        pushButton_Confirmer->setObjectName(QString::fromUtf8("pushButton_Confirmer"));
        pushButton_Confirmer->setGeometry(QRect(210, 210, 91, 21));
        label_Message = new QLabel(forgotpassword);
        label_Message->setObjectName(QString::fromUtf8("label_Message"));
        label_Message->setGeometry(QRect(40, 160, 221, 16));
        lineEdit_Mail = new QLineEdit(forgotpassword);
        lineEdit_Mail->setObjectName(QString::fromUtf8("lineEdit_Mail"));
        lineEdit_Mail->setGeometry(QRect(50, 110, 271, 20));

        retranslateUi(forgotpassword);

        QMetaObject::connectSlotsByName(forgotpassword);
    } // setupUi

    void retranslateUi(QDialog *forgotpassword)
    {
        forgotpassword->setWindowTitle(QCoreApplication::translate("forgotpassword", "Dialog", nullptr));
        label_Titre->setText(QCoreApplication::translate("forgotpassword", "Entrez votre adresse mail", nullptr));
        label_id->setText(QCoreApplication::translate("forgotpassword", "Nouvel Identifiant", nullptr));
        label_mdp->setText(QCoreApplication::translate("forgotpassword", "Nouveau Mot De Passe", nullptr));
        pushButton_Retour->setText(QCoreApplication::translate("forgotpassword", "Retour", nullptr));
        pushButton_Confirmer->setText(QCoreApplication::translate("forgotpassword", "Confirmer", nullptr));
        label_Message->setText(QCoreApplication::translate("forgotpassword", "R\303\251sultat", nullptr));
    } // retranslateUi

};

namespace Ui {
    class forgotpassword: public Ui_forgotpassword {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORGOTPASSWORD_H
