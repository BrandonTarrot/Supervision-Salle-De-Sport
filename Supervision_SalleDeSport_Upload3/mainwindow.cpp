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
#include <QCryptographicHash>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

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
    connect(timerCompteur, SIGNAL(timeout()), this, SLOT(mettreAJourCompteur()));

    // Connexion à la BDD pour la vérification des identifiants
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "loginConnection");
    db.setHostName("172.19.6.120");
    db.setPort(3306);
    db.setDatabaseName("supervision_salle_de_sport2");
    db.setUserName("api");
    db.setPassword("api");

    if (!db.open()) {
        qDebug() << "Erreur connexion login BDD :" << db.lastError().text();
    }

    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");

    // Récupération de la dernière connexion
    QString derniereConnexion = settings.value("derniere_connexion", "").toString();
    if (!derniereConnexion.isEmpty()) {
        ui->label_DerniereConnexion->setText("Dernière connexion : " + derniereConnexion);
    }

    // Récupération des compteurs sauvegardés
    tentativesEchouees = settings.value("tentatives", 0).toInt();
    blocsEchoues       = settings.value("blocs", 0).toInt();

    // Reset automatique toutes les heures si pas de blocage actif
    QDateTime dernierReset = settings.value("dernier_reset").toDateTime();
    QDateTime maintenant = QDateTime::currentDateTime();

    bool resetNecessaire = false;
    if (!dernierReset.isValid()) {
        resetNecessaire = true;
    } else {
        int secondesEcoulees = dernierReset.secsTo(maintenant);
        if (secondesEcoulees >= 3600) {
            resetNecessaire = true;
        }
    }

    if (resetNecessaire) {
        bool blocageDefinitif = settings.value("blocage_definitif", false).toBool();
        bool blocageEnCours = settings.value("blocage_fin").toDateTime().isValid();

        if (!blocageDefinitif && !blocageEnCours) {
            settings.setValue("tentatives", 0);
            settings.setValue("blocs", 0);
            settings.setValue("dernier_reset", maintenant);
            tentativesEchouees = 0;
            blocsEchoues = 0;
        }
    }

    // Vérifie si un blocage définitif est actif
    bool blocageDefinitif = settings.value("blocage_definitif", false).toBool();
    if (blocageDefinitif) {
        bloquerDefinitivement();
        return;
    }

    // Vérifie si un blocage temporaire est en cours
    QDateTime fin = settings.value("blocage_fin").toDateTime();
    if (fin.isValid() && fin > maintenant) {
        int resteSecondes = maintenant.secsTo(fin);
        demarrerCompteur(resteSecondes);
    }
}

MainWindow::~MainWindow()
{
    QSqlDatabase::removeDatabase("loginConnection");
    delete ui;
}

bool MainWindow::verifierIdentifiants(const QString &login, const QString &mdp)
{
    QSqlDatabase db = QSqlDatabase::database("loginConnection");

    qDebug() << "BDD ouverte :" << db.isOpen();
    qDebug() << "Login saisi :" << login;
    qDebug() << "MDP saisi :" << mdp;

    if (!db.isOpen()) {
        return false;
    }

    QByteArray motDePasseUtf8 = mdp.toUtf8();
    QByteArray hashBrut = QCryptographicHash::hash(motDePasseUtf8, QCryptographicHash::Sha256);
    QString mdpHache = hashBrut.toHex();

    qDebug() << "Hash calculé :" << mdpHache;

    QSqlQuery query(db);
    query.prepare("SELECT mot_de_passe FROM authentification WHERE login = :login");
    query.bindValue(":login", login);

    if (query.exec() && query.next()) {
        QString hashBDD = query.value(0).toString();
        qDebug() << "Hash BDD     :" << hashBDD;
        qDebug() << "Identiques   :" << (hashBDD == mdpHache);

        if (hashBDD == mdpHache) {
            return true;
        }
    } else {
        qDebug() << "Aucun utilisateur trouvé pour ce login";
    }

    return false;
}

void MainWindow::bloquerDefinitivement()
{
    ui->pushButton_Login->setEnabled(false);
    ui->pushButton_Login->setCursor(Qt::ForbiddenCursor);
    ui->lineEdit_Username_2->setEnabled(false);
    ui->lineEdit_Password_2->setEnabled(false);
    ui->checkBox_AfficherMDP->setEnabled(false);
    ui->pushButton_Cancel->setEnabled(false);
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
    ui->label_Backup->setVisible(true);

    // Sauvegarde de la fin du blocage
    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    QDateTime maintenant = QDateTime::currentDateTime();
    QDateTime finBlocage = maintenant.addSecs(secondes);
    settings.setValue("blocage_fin", finBlocage);

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

    int minutes = secondesRestantes / 60;
    int secondes = secondesRestantes % 60;

    QString minutesStr = QString::number(minutes);
    if (minutes < 10) {
        minutesStr = "0" + minutesStr;
    }

    QString secondesStr = QString::number(secondes);
    if (secondes < 10) {
        secondesStr = "0" + secondesStr;
    }

    QString message = "⛔ Accès bloqué - Réessayez dans " + minutesStr + ":" + secondesStr;
    ui->label_Copyright->setText(message);
    ui->label_Copyright->setStyleSheet("color: #ff1100; font-weight: bold;");

    secondesRestantes = secondesRestantes - 1;
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

    // Récupération des credentials Gmail depuis la BDD
    QString user = "supervision.salledesport@gmail.com";
    QString pass = "znufrpddovfwcven";

    QSqlDatabase db = QSqlDatabase::database("loginConnection");
    if (db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("SELECT gmail_user, gmail_pass FROM authentification WHERE login = 'Administrateur'");
        if (query.exec()) {
            if (query.next()) {
                user = query.value(0).toString();
                pass = query.value(1).toString();
            }
        }
    }

    QString host    = "smtp.gmail.com";
    QString subject = "ALERTE SECURITE - Tentatives suspectes";

    // Création du thread d'envoi
    QThread *thread = new QThread();

    connect(thread, &QThread::started, [thread, user, pass, host, subject, corps]() {
        Smtp *smtp = new Smtp(user, pass, host, user, user, subject, corps);
        QObject::connect(smtp, SIGNAL(mailSent(bool, QString)), thread, SLOT(quit()));
        QObject::connect(thread, SIGNAL(finished()), smtp,   SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
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

    bool identifiantsCorrects = verifierIdentifiants(username, password);

    if (identifiantsCorrects) {
        // Connexion réussie
        QString heure = QDateTime::currentDateTime().toString("dd/MM/yyyy à HH:mm:ss");
        QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
        settings.setValue("derniere_connexion", heure);
        ui->label_DerniereConnexion->setText("Dernière connexion : " + heure);

        // Reset des compteurs après connexion réussie
        settings.setValue("tentatives", 0);
        settings.setValue("blocs", 0);
        settings.remove("blocage_fin");
        settings.remove("blocage_definitif");
        tentativesEchouees = 0;
        blocsEchoues = 0;

        AdminWindow *adminWin = new AdminWindow(this);
        adminWin->show();
        this->hide();

    } else {
        // Connexion échouée
        tentativesEchouees = tentativesEchouees + 1;
        QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
        settings.setValue("tentatives", tentativesEchouees);

        // Vérification si un blocage doit être déclenché
        bool blocageDeclenche = false;
        if (tentativesEchouees % 3 == 0) {
            blocageDeclenche = true;
        }

        if (blocageDeclenche) {
            blocsEchoues = blocsEchoues + 1;
            settings.setValue("blocs", blocsEchoues);

            // Tableau des durées de blocage
            int durees[4];
            durees[0] = 0;     // index 0 — non utilisé
            durees[1] = 60;    // bloc 1 — 1 minute
            durees[2] = 300;   // bloc 2 — 5 minutes
            durees[3] = 1800;  // bloc 3 — 30 minutes

            if (blocsEchoues <= 3) {
                int dureeBlocage = durees[blocsEchoues];
                demarrerCompteur(dureeBlocage);

                if (blocsEchoues == 3) {
                    envoyerAlerteSecurite();
                }
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
            QString message = "Identifiants invalides. Tentatives restantes : " + QString::number(restantes);
            QMessageBox::warning(this, "Erreur", message);
            ui->lineEdit_Password_2->clear();
            ui->lineEdit_Password_2->setFocus();
        }
    }
}

void MainWindow::on_pushButton_Cancel_clicked()
{
    int reponse = QMessageBox::question(this, "Quitter",
        "Voulez-vous vraiment quitter ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reponse == QMessageBox::Yes) {
        QApplication::quit();
    }
}

void MainWindow::on_checkBox_AfficherMDP_toggled(bool checked)
{
    if (checked) {
        ui->lineEdit_Password_2->setEchoMode(QLineEdit::Normal);
    } else {
        ui->lineEdit_Password_2->setEchoMode(QLineEdit::Password);
    }
}

void MainWindow::on_label_Password_Forgot_linkActivated(const QString &)
{
    forgotpassword dialog(this);
    dialog.exec();
}

void MainWindow::on_label_Backup_linkActivated(const QString &)
{
    bool ok;
    QString code = QInputDialog::getText(this, "Code de secours",
        "Entrez le code backup :", QLineEdit::Normal, "", &ok);

    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    QString codeValide = settings.value("backup_code", "K7X2-M9QP-4RNW").toString();

    if (ok) {
        QString codeNettoye = code.trimmed();
        if (codeNettoye == codeValide) {
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
        } else {
            QMessageBox::warning(this, "Erreur", "Code incorrect.");
        }
    }
}
