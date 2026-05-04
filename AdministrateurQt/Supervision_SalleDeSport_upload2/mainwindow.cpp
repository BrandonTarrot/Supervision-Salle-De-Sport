#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "adminwindow.h"
#include "forgotpassword.h"
#include "smtp.h"
#include <QMessageBox>
#include <QApplication>
#include <QInputDialog>
#include <QDateTime>
#include <QSettings>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_Password_2->setEchoMode(QLineEdit::Password);
    ui->pushButton_Login->setCursor(Qt::PointingHandCursor);
    ui->pushButton_Cancel->setCursor(Qt::PointingHandCursor);
    ui->label_Backup->setStyleSheet(
        "QLabel { color: #ff1100; text-decoration: underline; }");
    ui->label_Backup->setVisible(false);

    timerCompteur = new QTimer(this);
    connect(timerCompteur, &QTimer::timeout, this, &MainWindow::mettreAJourCompteur);

    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");

    QString derniereConnexion = settings.value("derniere_connexion", "").toString();
    if (!derniereConnexion.isEmpty())
        ui->label_DerniereConnexion->setText("Dernière connexion : " + derniereConnexion);

    tentativesEchouees = settings.value("tentatives", 0).toInt();
    blocsEchoues       = settings.value("blocs", 0).toInt();

    QDateTime dernierReset = settings.value("dernier_reset").toDateTime();
    if (!dernierReset.isValid() || dernierReset.secsTo(QDateTime::currentDateTime()) >= 3600) {
        if (!settings.value("blocage_definitif", false).toBool() &&
            !settings.value("blocage_fin").toDateTime().isValid()) {
            settings.setValue("tentatives", 0);
            settings.setValue("blocs", 0);
            settings.setValue("dernier_reset", QDateTime::currentDateTime());
            tentativesEchouees = 0;
            blocsEchoues = 0;
        }
    }

    if (settings.value("blocage_definitif", false).toBool()) {
        bloquerDefinitivement();
        return;
    }

    QDateTime fin = settings.value("blocage_fin").toDateTime();
    if (fin.isValid() && fin > QDateTime::currentDateTime()) {
        int resteSecondes = QDateTime::currentDateTime().secsTo(fin);
        demarrerCompteur(resteSecondes);
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::bloquerDefinitivement()
{
    ui->pushButton_Login->setEnabled(false);
    ui->pushButton_Login->setCursor(Qt::ForbiddenCursor);
    ui->lineEdit_Username_2->setEnabled(false);
    ui->lineEdit_Password_2->setEnabled(false);
    ui->checkBox_AfficherMDP->setEnabled(false);
    ui->pushButton_Cancel->setEnabled(false);
    // label_Password_Forgot reste actif intentionnellement
    ui->label_Backup->setVisible(true);
    ui->label_Copyright->setText("⛔ Accès définitivement bloqué - Utilisez le code backup");
    ui->label_Copyright->setStyleSheet("color: #ff1100; font-weight: bold;");
}

void MainWindow::toutReactiver()
{
    ui->lineEdit_Username_2->setEnabled(true);
    ui->lineEdit_Password_2->setEnabled(true);
    ui->checkBox_AfficherMDP->setEnabled(true);
    ui->pushButton_Login->setEnabled(true);
    ui->pushButton_Login->setCursor(Qt::PointingHandCursor);
    ui->pushButton_Cancel->setEnabled(true);
    // label_Password_Forgot toujours actif, pas besoin de le réactiver
    ui->label_Backup->setVisible(false);
    ui->label_Copyright->setText(
        "@Copyright 2026 - All rights reserved - The BasicSlimFit Industry");
    ui->label_Copyright->setStyleSheet("");
}

void MainWindow::demarrerCompteur(int secondes)
{
    secondesRestantes = secondes;

    ui->lineEdit_Username_2->setEnabled(false);
    ui->lineEdit_Password_2->setEnabled(false);
    ui->checkBox_AfficherMDP->setEnabled(false);
    ui->pushButton_Login->setEnabled(false);
    ui->pushButton_Cancel->setEnabled(false);
    // label_Password_Forgot reste actif pendant le blocage
    ui->label_Backup->setVisible(true);

    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    settings.setValue("blocage_fin", QDateTime::currentDateTime().addSecs(secondes));

    timerCompteur->start(1000);
    mettreAJourCompteur();
}

void MainWindow::mettreAJourCompteur()
{
    if (secondesRestantes <= 0) {
        timerCompteur->stop();
        QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
        settings.remove("blocage_fin");
        toutReactiver();
        return;
    }

    int min = secondesRestantes / 60;
    int sec = secondesRestantes % 60;

    ui->label_Copyright->setText(
        QString("⛔ Accès bloqué - Réessayez dans %1:%2")
        .arg(min, 2, 10, QChar('0'))
        .arg(sec, 2, 10, QChar('0')));
    ui->label_Copyright->setStyleSheet("color: #ff1100; font-weight: bold;");

    secondesRestantes--;
}

void MainWindow::envoyerAlerteSecurite()
{
    QString dateHeure = QDateTime::currentDateTime().toString("dd/MM/yyyy a HH:mm:ss");
    QString corps =
        "ALERTE DE SECURITE\n\n"
        "Plusieurs tentatives de connexion ont echoue sur l'interface "
        "Supervision Salle De Sport.\n\n"
        "Date : " + dateHeure + "\n\n"
        "L'acces a ete bloque pendant 30 minutes.\n\n"
        "Si vous n'etes pas a l'origine de ces tentatives, "
        "un danger potentiel existe.\n\n"
        "Supervision Salle De Sport - BasicSlimFit Industry";

    QString user    = "supervision.salledesport@gmail.com";
    QString pass    = "znufrpddovfwcven";
    QString host    = "smtp.gmail.com";
    QString subject = "ALERTE SECURITE - Tentatives suspectes";

    QThread *thread = new QThread();

    // Smtp créé DANS le thread pour éviter le freeze
    connect(thread, &QThread::started, [thread, user, pass, host, subject, corps]() {
        Smtp *smtp = new Smtp(user, pass, host, user, user, subject, corps);
        QObject::connect(smtp, &Smtp::mailSent, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, smtp,   &QObject::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    });

    thread->start();
}

void MainWindow::on_pushButton_Login_clicked()
{
    if (!ui->pushButton_Login->isEnabled()) {
        QMessageBox::warning(this, "Accès bloqué", "Réessayez plus tard.");
        return;
    }

    QString username = ui->lineEdit_Username_2->text().trimmed();
    QString password = ui->lineEdit_Password_2->text();

    if (username == "" && password == "") { //Administrateur //2PvIuL2AqT1OrPkk0
        QString heure = QDateTime::currentDateTime().toString("dd/MM/yyyy à HH:mm:ss");
        QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
        settings.setValue("derniere_connexion", heure);
        ui->label_DerniereConnexion->setText("Dernière connexion : " + heure);
        AdminWindow *adminWin = new AdminWindow(this);
        adminWin->show();
        this->hide();
    } else {
        tentativesEchouees++;
        QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
        settings.setValue("tentatives", tentativesEchouees);

        if (tentativesEchouees % 3 == 0) {
            blocsEchoues++;
            settings.setValue("blocs", blocsEchoues);

            int durees[] = {
                0,
                60,
                300,
                1800
            };

            if (blocsEchoues <= 3) {
                demarrerCompteur(durees[blocsEchoues]);
                if (blocsEchoues == 3)
                    envoyerAlerteSecurite();
            } else {
                settings.setValue("blocage_definitif", true);
                settings.remove("blocage_fin");
                QMessageBox::critical(this, "Accès bloqué",
                    "Accès définitivement bloqué.\n"
                    "Utilisez le code backup pour vous connecter.");
                bloquerDefinitivement();
            }

        } else {
            int restantes = 3 - (tentativesEchouees % 3);
            QMessageBox::warning(this, "Erreur",
                QString("Identifiants invalides. Tentatives restantes : %1")
                .arg(restantes));
            ui->lineEdit_Password_2->clear();
            ui->lineEdit_Password_2->setFocus();
        }
    }
}

void MainWindow::on_pushButton_Cancel_clicked()
{
    if (QMessageBox::question(this, "Quitter", "Voulez-vous vraiment quitter ?",
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        QApplication::quit();
}

void MainWindow::on_checkBox_AfficherMDP_toggled(bool checked)
{
    ui->lineEdit_Password_2->setEchoMode(
        checked ? QLineEdit::Normal : QLineEdit::Password);
}

void MainWindow::on_label_Password_Forgot_linkActivated(const QString &)
{
    forgotpassword(this).exec();
}

void MainWindow::on_label_Backup_linkActivated(const QString &)
{
    bool ok;
    QString code = QInputDialog::getText(this, "Code de secours",
        "Entrez le code backup :", QLineEdit::Normal, "", &ok);

    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    QString codeValide = settings.value("backup_code", "K7X2-M9QP-4RNW").toString();

    if (ok && code.trimmed() == codeValide) {
        timerCompteur->stop();

        settings.remove("blocage_fin");
        settings.remove("blocage_definitif");
        settings.remove("tentatives");
        settings.remove("blocs");
        settings.setValue("dernier_reset", QDateTime::currentDateTime());

        tentativesEchouees = 0;
        blocsEchoues = 0;

        toutReactiver();

        QMessageBox::information(this, "Accès autorisé", "Code valide !");
        AdminWindow *adminWin = new AdminWindow(this);
        adminWin->show();
        this->hide();
    } else if (ok) {
        QMessageBox::warning(this, "Erreur", "Code incorrect.");
    }
}
