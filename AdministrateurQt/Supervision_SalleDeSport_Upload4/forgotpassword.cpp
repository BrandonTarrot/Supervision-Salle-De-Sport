#include "forgotpassword.h"
#include "ui_forgotpassword.h"
#include "smtp.h"
#include <QThread>
#include <QSettings>
#include <QRandomGenerator>

const QString GMAIL_USER = "supervision.salledesport@gmail.com";
const QString GMAIL_PASS = "znufrpddovfwcven";
const QString MAIL_ADMIN = "supervision.salledesport@gmail.com";


// CONSTRUCTEUR

forgotpassword::forgotpassword(QWidget *parent)
    : QDialog(parent), ui(new Ui::forgotpassword)
{
    ui->setupUi(this);
    setWindowTitle("Mot de passe oublié");
    ui->lineEdit_Mail->setText(MAIL_ADMIN);
    ui->lineEdit_Mail->setReadOnly(true);
    ui->lineEdit_Mail->setStyleSheet("color: gray;");
}

forgotpassword::~forgotpassword()
{
    delete ui;
}


// GENERATION CODE BACKUP

QString forgotpassword::genererCode()
{
    QString caracteres = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    QString resultat   = "";

    for (int i = 0; i < 4; i = i + 1)
    {
        int   indexAleatoire  = QRandomGenerator::global()->bounded(caracteres.size());
        QChar caractereChoisi = caracteres.at(indexAleatoire);
        resultat = resultat + caractereChoisi;
    }

    resultat = resultat + "-";

    for (int i = 0; i < 4; i = i + 1)
    {
        int   indexAleatoire  = QRandomGenerator::global()->bounded(caracteres.size());
        QChar caractereChoisi = caracteres.at(indexAleatoire);
        resultat = resultat + caractereChoisi;
    }

    resultat = resultat + "-";

    for (int i = 0; i < 4; i = i + 1)
    {
        int   indexAleatoire  = QRandomGenerator::global()->bounded(caracteres.size());
        QChar caractereChoisi = caracteres.at(indexAleatoire);
        resultat = resultat + caractereChoisi;
    }

    return resultat;
}


// BOUTON CONFIRMER — ENVOI MAIL CODE BACKUP

void forgotpassword::on_pushButton_Confirm_clicked()
{
    QString mail = ui->lineEdit_Mail->text().trimmed();

    if (mail.isEmpty())
    {
        ui->label_Message->setStyleSheet("color: orange;");
        ui->label_Message->setText("Veuillez entrer votre adresse mail.");
        return;
    }

    if (mail != MAIL_ADMIN)
    {
        ui->label_Message->setStyleSheet("color: red;");
        ui->label_Message->setText("Adresse mail non reconnue.");
        return;
    }

    QString nouveauCode = genererCode();
    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    settings.setValue("backup_code", nouveauCode);

    ui->label_Message->setStyleSheet("color: blue;");
    ui->label_Message->setText("Envoi en cours...");
    ui->pushButton_Confirm->setEnabled(false);

    QString corps = "Bonjour,\n\nVoici votre nouveau code de secours :\n\n";
    corps = corps + "    " + nouveauCode + "\n\n";
    corps = corps + "Entrez ce code via Code Backup sur la page de connexion.\n";
    corps = corps + "Ce code remplace l'ancien et est a usage unique.\n\n";
    corps = corps + "Supervision Salle De Sport - BasicSlimFit Industry";

    new Smtp(GMAIL_USER, GMAIL_PASS, "smtp.gmail.com", GMAIL_USER, MAIL_ADMIN,
             "Récupération de mot de passe", corps);

    ui->label_Message->setStyleSheet("color: green;");
    ui->label_Message->setText("Mail envoyé ! Vérifiez votre boîte.");
}


// BOUTON RETOUR

void forgotpassword::on_pushButton_Back_clicked()
{
    this->close();
}
