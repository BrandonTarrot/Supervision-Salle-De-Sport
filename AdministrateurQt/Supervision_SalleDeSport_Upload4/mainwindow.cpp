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


// CONSTRUCTEUR

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this); //charge les widgets

    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
    ui->pushButton_Connexion->setCursor(Qt::PointingHandCursor);
    ui->pushButton_Cancel->setCursor(Qt::PointingHandCursor);
    ui->label_Backup_Code->setStyleSheet("QLabel { color: #ff1100; text-decoration: underline; }");
    ui->label_Backup_Code->setVisible(false);

    //créer compteur blocage
    timerCompteur = new QTimer(this);
    connect(timerCompteur, SIGNAL(timeout()), this, SLOT(mettreAJourCompteur()));




    //Infos BDD
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "loginConnection");
    db.setHostName("172.19.6.120");
    db.setPort(3306);
    db.setDatabaseName("supervision_salle_de_sport2");
    db.setUserName("api");
    db.setPassword("api");

    if (!db.open())
    {
        qDebug() << "Erreur connexion login BDD :" << db.lastError().text();
    }




    //Registre windows
    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    QString derniereConnexion = settings.value("derniere_connexion", "").toString();

    if (!derniereConnexion.isEmpty())
    {
        ui->label_LastConnexion->setText("Dernière connexion : " + derniereConnexion);
    }

        //Mémorise les valeurs + converti en entier
        tentativesEchouees = settings.value("tentatives", 0).toInt();
        blocsEchoues       = settings.value("blocs", 0).toInt();

        //Vérification blocage définitif au démarrage
        bool blocageDefinitif = settings.value("blocage_definitif", false).toBool();
        if (blocageDefinitif)
        {
            bloquerDefinitivement();
            return;
        }

        //Restauration blocage en cours quand app relancée
        QDateTime maintenant = QDateTime::currentDateTime();
        QDateTime fin = settings.value("blocage_fin").toDateTime();
        if (fin.isValid() && fin > maintenant)
        {
            int resteSecondes = maintenant.secsTo(fin);
            demarrerCompteur(resteSecondes);
        }
    }



MainWindow::~MainWindow()
{
    QSqlDatabase::removeDatabase("loginConnection");
    delete ui;
}



// VERIFICATION IDENTIFIANTS — SHA-256

bool MainWindow::verifierIdentifiants(const QString &login, const QString &mdp) //True or false
{
    QSqlDatabase db = QSqlDatabase::database("loginConnection"); //reprend infos bdd
    if (!db.isOpen())
    {
        return false;
    }

    QByteArray motDePasseUtf8 = mdp.toUtf8();
    QByteArray hashBrut = QCryptographicHash::hash(motDePasseUtf8, QCryptographicHash::Sha256); //Hash en SHA-256
    QString mdpHache = hashBrut.toHex(); //Convertit en hexa

    QSqlQuery requeteSQL(db); //Requete pour récupérer Hash
    requeteSQL.prepare("SELECT mot_de_passe FROM authentification WHERE login = :Clavier");
    requeteSQL.bindValue(":Clavier", login); //vérifie

    if (requeteSQL.exec())
    {
        if (requeteSQL.next())
        {
            QString hashBDD = requeteSQL.value(0).toString();
            if (hashBDD == mdpHache) //Compare
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}


// BLOCAGE — ACTIVATION / DESACTIVATION

void MainWindow::bloquerDefinitivement() //si 12 tentatives efectué
{
    ui->pushButton_Connexion->setEnabled(false);
    ui->pushButton_Connexion->setCursor(Qt::ForbiddenCursor);
    ui->lineEdit_Username->setEnabled(false);
    ui->lineEdit_Password->setEnabled(false);
    ui->checkBox_Show_Password->setEnabled(false);
    ui->pushButton_Cancel->setEnabled(false);
    ui->label_Backup_Code->setVisible(true);
    ui->label_Copyright->setText("Accès définitivement bloqué - Utilisez le code backup");
    ui->label_Copyright->setStyleSheet("color: #ff1100; font-weight: bold;");
}

void MainWindow::toutReactiver()
{
    ui->lineEdit_Username->setEnabled(true);
    ui->lineEdit_Password->setEnabled(true);
    ui->checkBox_Show_Password->setEnabled(true);
    ui->pushButton_Connexion->setEnabled(true);
    ui->pushButton_Connexion->setCursor(Qt::PointingHandCursor);
    ui->pushButton_Cancel->setEnabled(true);
    ui->label_Backup_Code->setVisible(false);
    ui->label_Copyright->setText("@Copyright 2026 - All rights reserved - The BasicSlimFit Industry");
    ui->label_Copyright->setStyleSheet("");
}


// COMPTEUR DE BLOCAGE

void MainWindow::demarrerCompteur(int secondes)
{
    secondesRestantes = secondes; //stocke

    //Desactive les widgets pendant de compteurs
    ui->lineEdit_Username->setEnabled(false);
    ui->lineEdit_Password->setEnabled(false);
    ui->checkBox_Show_Password->setEnabled(false);
    ui->pushButton_Connexion->setEnabled(false);
    ui->pushButton_Cancel->setEnabled(false);

    ui->label_Backup_Code->setVisible(true); //rentrer le code backup

    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    QDateTime maintenant = QDateTime::currentDateTime(); //date actuelle
    QDateTime finBlocage = maintenant.addSecs(secondes); //calcule le temps en partant de la date récupéré
    settings.setValue("blocage_fin", finBlocage); //sauvegarde

    timerCompteur->start(1000); //met à jour le compteur chaque 1000ms
    mettreAJourCompteur();
}

void MainWindow::mettreAJourCompteur()
{
    if (secondesRestantes <= 0)
    {
        timerCompteur->stop();
        QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
        settings.remove("blocage_fin");
        toutReactiver(); //ligne 148
        return;
    }

    int minutes = secondesRestantes / 60; //convertit en minutes
    int secondes = secondesRestantes % 60; //convertit en secondes

    QString texteMinutes = QString::number(minutes);
    if (minutes < 10)
    {
        texteMinutes = "0" + texteMinutes; //00:00
    }

    QString texteSecondes = QString::number(secondes);
    if (secondes < 10)
    {
        texteSecondes = "0" + texteSecondes; //00:00
    }

    QString message = "Accès bloqué - Réessayez dans " + texteMinutes + ":" + texteSecondes;
    ui->label_Copyright->setText(message);
    ui->label_Copyright->setStyleSheet("color: #ff1100; font-weight: bold;");

    secondesRestantes = secondesRestantes - 1;
}


// ALERTE MAIL SECURITE

void MainWindow::envoyerAlerteSecurite()
{
    QString dateHeure = QDateTime::currentDateTime().toString("dd/MM/yyyy a HH:mm:ss"); //convertit en format lisible

    //Création mail
    QString mail = "ALERTE DE SECURITE\n\n";
    mail = mail + "Plusieurs tentatives de connexion ont echoue.\n\n";
    mail = mail + "Date : " + dateHeure + "\n\n";
    mail = mail + "L'acces a ete bloque pendant 30 minutes.\n\n";
    mail = mail + "Supervision Salle De Sport - BasicSlimFit Industry";

    QString user    = "supervision.salledesport@gmail.com";
    QString pass    = "znufrpddovfwcven";
    QString subject = "ALERTE SECURITE - Tentatives suspectes";

    new Smtp(user, pass, "smtp.gmail.com", user, user, subject, mail);
}



// BOUTON CONNEXION

void MainWindow::on_pushButton_Connexion_clicked()
{
    QString username = ui->lineEdit_Username->text().trimmed();
    QString password = ui->lineEdit_Password->text();

    bool identifiantsCorrects = verifierIdentifiants(username, password); //ligne 97

    if (identifiantsCorrects)
    {
        QString heure = QDateTime::currentDateTime().toString("dd/MM/yyyy à HH:mm:ss"); //convertit en format lisible

        QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
        settings.setValue("derniere_connexion", heure);
        ui->label_LastConnexion->setText("Dernière connexion : " + heure); //met à jour la date du label

        //Remet tous les éventuelles blocages à zéro
        settings.setValue("tentatives", 0);
        settings.setValue("blocs", 0);
        settings.remove("blocage_fin");
        settings.remove("blocage_definitif");
        tentativesEchouees = 0;
        blocsEchoues = 0;

        AdminWindow *adminWin = new AdminWindow(this);
        adminWin->show(); //Ouvre la page Administrateur
        this->hide(); //Cache la page Authentification
    }
    else
    {
        tentativesEchouees = tentativesEchouees + 1;
        QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
        settings.setValue("tentatives", tentativesEchouees);

        bool blocageDeclenche = false;
        if (tentativesEchouees % 3 == 0)
        {
            blocageDeclenche = true;
        }

        if (blocageDeclenche)
        {
            blocsEchoues = blocsEchoues + 1;
            settings.setValue("blocs", blocsEchoues); //bloc Echoué = 3 tentatives

            int durees[4]; //bloc Echoué = +1 durees
            durees[0] = 0;
            durees[1] = 60;
            durees[2] = 300;
            durees[3] = 1800;

            if (blocsEchoues <= 3)
            {
                int dureeBlocage = durees[blocsEchoues];
                demarrerCompteur(dureeBlocage);

                if (blocsEchoues == 3)
                {
                    envoyerAlerteSecurite(); //Mail ligne 222
                }
            }
            else
            {
                settings.setValue("blocage_definitif", true);
                settings.remove("blocage_fin");

                QMessageBox::critical(this, "Accès bloqué",
                    "Accès définitivement bloqué.\n"
                    "Utilisez le code backup pour vous connecter.");

                bloquerDefinitivement();
            }
        }
        else
        {   //Actions a chaque tentatives
            int restantes = 3 - (tentativesEchouees % 3);
            QString message = "Identifiants invalides. Tentatives restantes : " + QString::number(restantes);
            QMessageBox::warning(this, "Erreur", message);
            ui->lineEdit_Password->clear();
            ui->lineEdit_Password->setFocus();
        }
    }
}


// BOUTON ANNULER

void MainWindow::on_pushButton_Cancel_clicked()
{
    int reponse = QMessageBox::question(this, "Quitter",
        "Voulez-vous vraiment quitter ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reponse == QMessageBox::Yes)
    {
        QApplication::quit();
    }
}



// CHECKBOX AFFICHER MOT DE PASSE

void MainWindow::on_checkBox_Show_Password_toggled(bool checked) //echo mode = ........
{
    if (checked)
    {
        ui->lineEdit_Password->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
    }
}



// LABEL MOT DE PASSE OUBLIE

void MainWindow::on_label_Forgot_Password_linkActivated(const QString &)
{
    forgotpassword dialog(this); //forgotpassword.cpp
    dialog.exec();
}



// LABEL CODE BACKUP

void MainWindow::on_label_Backup_Code_linkActivated(const QString &) //visible uniquement pendant blocage
{
    bool ok;
    QString code = QInputDialog::getText(this, "Code de secours", //affiche boite de saisie vide
        "Entrez le code backup :", QLineEdit::Normal, "", &ok);

    QSettings settings("BasicSlimFit", "SupervisionSalleDeSport");
    QString codeValide = settings.value("backup_code", "XXXX-XXXX-XXXX").toString(); //Format Code Backup

    if (ok) //Si code backup bon
    {
        QString codeNettoye = code.trimmed(); //supprime les espaces
        if (codeNettoye == codeValide) //compare le code avec le registre
        {
            timerCompteur->stop();

            //repart à zéro
            settings.remove("blocage_fin");
            settings.remove("blocage_definitif");
            settings.remove("tentatives");
            settings.remove("blocs");

            tentativesEchouees = 0;
            blocsEchoues = 0;

            toutReactiver(); //ligne 148

            QMessageBox::information(this, "Accès autorisé", "Code valide !");
            AdminWindow *adminWin = new AdminWindow(this);
            adminWin->show();
            this->hide();
        }
        else
        {
            QMessageBox::warning(this, "Erreur", "Code incorrect.");
        }
    }
}
//A bientot
