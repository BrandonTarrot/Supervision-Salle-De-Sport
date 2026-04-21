#include "forgotpassword.h"
#include "ui_forgotpassword.h"
#include "smtp.h"
#include <QMessageBox>
#include <QThread>
#include <QSettings>
#include <QRandomGenerator>

const QString GMAIL_USER = "supervision.salledesport@gmail.com";
const QString GMAIL_PASS = "znufrpddovfwcven";
const QString MAIL_ADMIN = "supervision.salledesport@gmail.com";

forgotpassword::forgotpassword(QWidget *parent)
    : QDialog(parent), ui(new Ui::forgotpassword)
{
    ui->setupUi(this);
    setWindowTitle("Mot de passe oublié");
    ui->lineEdit_Mail->setText(MAIL_ADMIN);
    ui->lineEdit_Mail->setReadOnly(true);
    ui->lineEdit_Mail->setStyleSheet("color: gray;");
}

forgotpassword::~forgotpassword() { delete ui; }

QString forgotpassword::genererCode()
{
    auto rand4 = []() {
        const QString chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
        QString result;
        for (int i = 0; i < 4; i++)
            result += chars[QRandomGenerator::global()->bounded(chars.size())];
        return result;
    };
    return rand4() + "-" + rand4() + "-" + rand4();
}

void forgotpassword::on_pushButton_Confirmer_clicked()
{
    QString mail = ui->lineEdit_Mail->text().trimmed();

    if (mail.isEmpty()) {
        ui->label_Message->setStyleSheet("color: orange;");
        ui->label_Message->setText("Veuillez entrer votre adresse mail.");
        return;
    }

    if (mail != MAIL_ADMIN) {
        ui->label_Message->setStyleSheet("color: red;");
        ui->label_Message->setText("Adresse mail non reconnue.");
        return;
    }

    QString nouveauCode = genererCode();
    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    settings.setValue("backup_code", nouveauCode);

    ui->label_Message->setStyleSheet("color: blue;");
    ui->label_Message->setText("Envoi en cours...");
    ui->pushButton_Confirmer->setEnabled(false);

    QString corps =
        "Bonjour,\n\nVoici votre nouveau code de secours :\n\n"
        "    " + nouveauCode + "\n\n"
        "Entrez ce code via 'Code Backup' sur la page de connexion.\n"
        "Ce code remplace l'ancien et est à usage unique.\n\n"
        "Supervision Salle De Sport - BasicSlimFit Industry";

    QThread *thread = new QThread();

    // Smtp créé DANS le thread pour éviter le freeze
    connect(thread, &QThread::started, [this, thread, corps]() {
        Smtp *smtp = new Smtp(
            GMAIL_USER, GMAIL_PASS, "smtp.gmail.com",
            GMAIL_USER, MAIL_ADMIN,
            "Récupération de mot de passe",
            corps
        );
        QObject::connect(smtp, &Smtp::mailSent, this,
            [this, thread, smtp](bool success, const QString &msg) {
                if (success) {
                    ui->label_Message->setStyleSheet("color: green;");
                    ui->label_Message->setText("Mail envoyé ! Vérifiez votre boîte.");
                } else {
                    ui->label_Message->setStyleSheet("color: red;");
                    ui->label_Message->setText(msg);
                    ui->pushButton_Confirmer->setEnabled(true);
                }
                smtp->deleteLater();
                thread->quit();
                thread->deleteLater();
            }, Qt::QueuedConnection);
    });

    thread->start();
}

void forgotpassword::on_pushButton_Retour_clicked()
{
    this->close();
}
