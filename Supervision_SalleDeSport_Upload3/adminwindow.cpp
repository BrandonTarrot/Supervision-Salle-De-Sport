#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QDebug>

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    setWindowTitle("Page Administrateur - Supervision Salle De Sport");
    ui->label_Logo->setPixmap(QPixmap(":/images/Ressources/AdminLogo.jpg"));

    // ── Initialisation des QMovie AVANT la connexion BDD ──
    movieVentilo = new QMovie(":/images/Ressources/ventilo_on.gif");
    ui->label_Ventilateur->setScaledContents(true);
    ui->label_Ventilateur->setVisible(false);

    moviePresence = new QMovie(":/images/Ressources/Running_Stickman.gif");
    ui->label_Presence->setScaledContents(true);
    ui->label_Presence->setVisible(false);

    // ── Connexion BDD (déclenche le timer qui utilise les movies) ──
    connecterBDD();

    // ── Récupération de l'état de la lampe au démarrage ──
    QSqlQuery queryLampe(db);
    queryLampe.exec("SELECT luminosite FROM mesures_luminosite ORDER BY date DESC LIMIT 1");
    if (queryLampe.next()) {
        int etatLampe = queryLampe.value(0).toInt();
        if (etatLampe == 1) {
            ui->bulb_on->setVisible(true);
            ui->bulb_off->setVisible(false);
        } else {
            ui->bulb_on->setVisible(false);
            ui->bulb_off->setVisible(true);
        }
    } else {
        ui->bulb_on->setVisible(false);
        ui->bulb_off->setVisible(false);
    }

    // ── Récupération de l'état du ventilateur au démarrage ──
    QSqlQuery queryVentilo(db);
    queryVentilo.exec("SELECT temperature FROM mesures_humidite_temperature ORDER BY date DESC LIMIT 1");
    if (queryVentilo.next()) {
        etatVentilateur = queryVentilo.value(0).toInt();
    } else {
        etatVentilateur = 0;
    }

    chargerTable("mesures_luminosite");
    afficherWidgetsAffichage();
}

AdminWindow::~AdminWindow()
{
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

void AdminWindow::connecterBDD()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("172.19.6.120");
    db.setPort(3306);
    db.setDatabaseName("supervision_salle_de_sport2");
    db.setUserName("api");
    db.setPassword("api");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur BDD",
            "Connexion échouée : " + db.lastError().text());
        qDebug() << "Erreur BDD :" << db.lastError().text();
    } else {
        qDebug() << "Connexion BDD réussie !";
        mettreAJourLabelTemperature();

        // Démarrage du rafraîchissement automatique
        timerEtatCapteurs = new QTimer(this);
        connect(timerEtatCapteurs, SIGNAL(timeout()), this, SLOT(mettreAJourEtatCapteurs()));
        timerEtatCapteurs->start(5000);
        mettreAJourEtatCapteurs();
    }
}

void AdminWindow::chargerTable(const QString &nomTable)
{
    if (!db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    QString requete = "SELECT * FROM " + nomTable + " ORDER BY date DESC";
    query.exec(requete);

    QStandardItemModel *modeleTableau = new QStandardItemModel(this);
    QSqlRecord enregistrement = query.record();

    // Récupération des noms de colonnes
    QStringList nomsColonnes;
    int nombreColonnes = enregistrement.count();
    for (int i = 0; i < nombreColonnes; i = i + 1) {
        QString nomColonne = enregistrement.fieldName(i);
        nomsColonnes.append(nomColonne);
    }
    modeleTableau->setHorizontalHeaderLabels(nomsColonnes);

    // Remplissage des lignes
    while (query.next()) {
        QList<QStandardItem*> ligne;
        for (int i = 0; i < nombreColonnes; i = i + 1) {
            QString valeur = query.value(i).toString();
            QStandardItem *cellule = new QStandardItem(valeur);
            ligne.append(cellule);
        }
        modeleTableau->appendRow(ligne);
    }

    ui->tableView_Affichage->setModel(modeleTableau);
    ui->tableView_Affichage->resizeColumnsToContents();
    ui->tableView_Affichage->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_Affichage->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Réinitialisation du style des boutons
    ui->pushButton_Luminosite->setStyleSheet("");
    ui->pushButton_Temperature->setStyleSheet("");
    ui->pushButton_Presence->setStyleSheet("");

    // Application du style sur le bouton actif
    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";
    if (nomTable == "mesures_luminosite") {
        ui->pushButton_Luminosite->setStyleSheet(styleActif);
        ui->label_Categorie->setText("Luminosité");
    } else if (nomTable == "mesures_humidite_temperature") {
        ui->pushButton_Temperature->setStyleSheet(styleActif);
        ui->label_Categorie->setText("Température / Humidité");
    } else if (nomTable == "mesures_presence") {
        ui->pushButton_Presence->setStyleSheet(styleActif);
        ui->label_Categorie->setText("Présence");
    }
}

void AdminWindow::initialiserTableauGestion()
{
    ui->tableWidget_Gestion->setColumnCount(3);
    QStringList entetes;
    entetes.append("Mesure");
    entetes.append("Allumer");
    entetes.append("Eteindre");
    ui->tableWidget_Gestion->setHorizontalHeaderLabels(entetes);

    QStringList mesures;
    mesures.append("Lumières");
    mesures.append("Ventilateur");  // ← au lieu de "Température"
    mesures.append("Présence");

    QStringList tables;
    tables.append("mesures_luminosite");
    tables.append("mesures_humidite_temperature");
    tables.append("mesures_presence");

    QStringList colonnes;
    colonnes.append("luminosite");
    colonnes.append("temperature");
    colonnes.append("presence");

    ui->tableWidget_Gestion->setRowCount(mesures.size());

    for (int i = 0; i < mesures.size(); i = i + 1) {
        QTableWidgetItem *itemMesure = new QTableWidgetItem(mesures.at(i));
        ui->tableWidget_Gestion->setItem(i, 0, itemMesure);

        // Pas de boutons pour la présence (capteur PIR, valeur automatique)
        if (mesures.at(i) == "Présence") {
            QTableWidgetItem *itemAuto = new QTableWidgetItem("Automatique");
            itemAuto->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Gestion->setItem(i, 1, itemAuto);
            ui->tableWidget_Gestion->setSpan(i, 1, 1, 2);
            continue;
        }

        // Bouton Allumer
        QPushButton *boutonAllumer = new QPushButton("Allumer");
        boutonAllumer->setFixedWidth(80);
        boutonAllumer->setProperty("action",  "allumer");
        boutonAllumer->setProperty("table",   tables.at(i));
        boutonAllumer->setProperty("colonne", colonnes.at(i));
        connect(boutonAllumer, SIGNAL(clicked()), this, SLOT(on_boutonGestion_clicked()));
        ui->tableWidget_Gestion->setCellWidget(i, 1, boutonAllumer);

        // Bouton Eteindre
        QPushButton *boutonEteindre = new QPushButton("Eteindre");
        boutonEteindre->setFixedWidth(80);
        boutonEteindre->setProperty("action",  "eteindre");
        boutonEteindre->setProperty("table",   tables.at(i));
        boutonEteindre->setProperty("colonne", colonnes.at(i));
        connect(boutonEteindre, SIGNAL(clicked()), this, SLOT(on_boutonGestion_clicked()));
        ui->tableWidget_Gestion->setCellWidget(i, 2, boutonEteindre);
    }

    ui->tableWidget_Gestion->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_Gestion->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void AdminWindow::mettreAJourVentilateur(int etat)
{
    ui->label_Ventilateur->setVisible(true);

    if (etat == 1) {
        ui->label_Ventilateur->setMovie(movieVentilo);
        movieVentilo->start();
    } else {
        movieVentilo->stop();
        QPixmap image(":/images/Ressources/ventilo_off.png");
        QPixmap imageRedimensionnee = image.scaled(ui->label_Ventilateur->size(), Qt::KeepAspectRatio);
        ui->label_Ventilateur->setPixmap(imageRedimensionnee);
    }
}

void AdminWindow::on_boutonGestion_clicked()
{
    QPushButton *bouton = qobject_cast<QPushButton*>(sender());
    if (bouton == nullptr) {
        return;
    }
    if (!db.isOpen()) {
        return;
    }

    QString table   = bouton->property("table").toString();
    QString colonne = bouton->property("colonne").toString();
    QString action  = bouton->property("action").toString();

    int etat = 0;
    if (action == "allumer") {
        etat = 1;
    } else {
        etat = 0;
    }

    // Détermination de la colonne dans etat_capteurs
    QString colonneEtat = "";
    if (table == "mesures_luminosite") {
        colonneEtat = "luminosite";
    } else if (table == "mesures_humidite_temperature") {
        colonneEtat = "temperature";
    } else if (table == "mesures_presence") {
        colonneEtat = "presence";
    }

    // Mise à jour de etat_capteurs
    QSqlQuery queryEtat(db);
    QString requeteUpdate = "UPDATE etat_capteurs SET " + colonneEtat + " = :etat";
    queryEtat.prepare(requeteUpdate);
    queryEtat.bindValue(":etat", etat);
    queryEtat.exec();

    // Pour la température : uniquement visuel + etat_capteurs
    if (table == "mesures_humidite_temperature") {
        etatVentilateur = etat;
        mettreAJourVentilateur(etat);
        return;
    }

    // Pour luminosité et présence : INSERT dans la table d'historique
    QSqlQuery query(db);
    QString requeteInsert = "INSERT INTO " + table + " (" + colonne + ", date, zone) VALUES (:etat, NOW(), 1)";
    query.prepare(requeteInsert);
    query.bindValue(":etat", etat);

    if (query.exec()) {
        chargerTable(table);

        if (table == "mesures_luminosite") {
            if (etat == 1) {
                ui->bulb_on->setVisible(true);
                ui->bulb_off->setVisible(false);
            } else {
                ui->bulb_on->setVisible(false);
                ui->bulb_off->setVisible(true);
            }
        }
    } else {
        QMessageBox::critical(this, "Erreur",
            "Échec de la requête : " + query.lastError().text());
    }
}

void AdminWindow::chargerHistorique()
{
    if (!db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    QString requete =
        "SELECT 'Luminosité' AS type, luminosite AS valeur, date FROM mesures_luminosite "
        "UNION ALL "
        "SELECT 'Présence' AS type, presence AS valeur, date FROM mesures_presence "
        "UNION ALL "
        "SELECT 'Température' AS type, temperature AS valeur, date FROM mesures_humidite_temperature "
        "ORDER BY date DESC "
        "LIMIT 100";
    query.exec(requete);

    QStandardItemModel *modeleHistorique = new QStandardItemModel(this);
    QStringList entetes;
    entetes.append("Type");
    entetes.append("Valeur");
    entetes.append("Date / Heure");
    modeleHistorique->setHorizontalHeaderLabels(entetes);

    while (query.next()) {
        QString type   = query.value(0).toString();
        QString valeur = query.value(1).toString();
        QDateTime dateHeure = query.value(2).toDateTime();
        QString date = dateHeure.toString("dd/MM/yyyy HH:mm:ss");

        // Conversion 0/1 en texte lisible
        if (type == "Luminosité") {
            int valeurInt = query.value(1).toInt();
            if (valeurInt == 1) {
                valeur = "Allumé";
            } else {
                valeur = "Éteint";
            }
        } else if (type == "Présence") {
            int valeurInt = query.value(1).toInt();
            if (valeurInt == 1) {
                valeur = "Présent";
            } else {
                valeur = "Absent";
            }
        }

        QList<QStandardItem*> ligne;
        QStandardItem *itemType   = new QStandardItem(type);
        QStandardItem *itemValeur = new QStandardItem(valeur);
        QStandardItem *itemDate   = new QStandardItem(date);
        ligne.append(itemType);
        ligne.append(itemValeur);
        ligne.append(itemDate);
        modeleHistorique->appendRow(ligne);
    }

    ui->tableView_Historique->setModel(modeleHistorique);
    ui->tableView_Historique->resizeColumnsToContents();
    ui->tableView_Historique->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void AdminWindow::afficherWidgetsAffichage()
{
    ui->tableView_Affichage->setVisible(true);
    ui->pushButton_Luminosite->setVisible(true);
    ui->pushButton_Temperature->setVisible(true);
    ui->pushButton_Presence->setVisible(true);
    ui->pushButton_Actualiser->setVisible(true);
    ui->tableWidget_Gestion->setVisible(false);
    ui->tableView_Historique->setVisible(false);

    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";
    ui->pushButton_Affichage->setStyleSheet(styleActif);
    ui->pushButton_Gestion->setStyleSheet("");
    ui->pushButton_Historique->setStyleSheet("");

    if (etatVentilateur == 1) {
        ui->label_Ventilateur->setVisible(true);
        ui->label_Ventilateur->setMovie(movieVentilo);
        movieVentilo->start();
    }
}

void AdminWindow::afficherWidgetsGestion()
{
    ui->tableView_Affichage->setVisible(false);
    ui->pushButton_Luminosite->setVisible(false);
    ui->pushButton_Temperature->setVisible(false);
    ui->pushButton_Presence->setVisible(false);
    ui->pushButton_Actualiser->setVisible(false);
    ui->tableView_Historique->setVisible(false);
    ui->tableWidget_Gestion->setVisible(true);

    initialiserTableauGestion();

    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";
    ui->pushButton_Gestion->setStyleSheet(styleActif);
    ui->pushButton_Affichage->setStyleSheet("");
    ui->pushButton_Historique->setStyleSheet("");

    mettreAJourVentilateur(etatVentilateur);
}

void AdminWindow::afficherWidgetsHistorique()
{
    ui->tableView_Affichage->setVisible(false);
    ui->pushButton_Luminosite->setVisible(false);
    ui->pushButton_Temperature->setVisible(false);
    ui->pushButton_Presence->setVisible(false);
    ui->pushButton_Actualiser->setVisible(false);
    ui->tableWidget_Gestion->setVisible(false);
    ui->tableView_Historique->setVisible(true);

    chargerHistorique();

    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";
    ui->pushButton_Historique->setStyleSheet(styleActif);
    ui->pushButton_Affichage->setStyleSheet("");
    ui->pushButton_Gestion->setStyleSheet("");

    if (etatVentilateur == 1) {
        ui->label_Ventilateur->setVisible(true);
        ui->label_Ventilateur->setMovie(movieVentilo);
        movieVentilo->start();
    }
}

void AdminWindow::on_pushButton_Affichage_clicked()
{
    afficherWidgetsAffichage();
}

void AdminWindow::on_pushButton_Gestion_clicked()
{
    afficherWidgetsGestion();
}

void AdminWindow::on_pushButton_Historique_clicked()
{
    afficherWidgetsHistorique();
}

void AdminWindow::on_pushButton_Luminosite_clicked()
{
    chargerTable("mesures_luminosite");
}

void AdminWindow::on_pushButton_Temperature_clicked()
{
    chargerTable("mesures_humidite_temperature");
}

void AdminWindow::on_pushButton_Presence_clicked()
{
    chargerTable("mesures_presence");
}

void AdminWindow::on_pushButton_Actualiser_clicked()
{
    ui->tableView_Affichage->setVisible(false);

    // Petit délai pour effet flash visuel
    QTimer::singleShot(500, this, SLOT(reafficherTableau()));
}

void AdminWindow::reafficherTableau()
{
    QString tableActive = "";

    QString styleLumi = ui->pushButton_Luminosite->styleSheet();
    QString styleTemp = ui->pushButton_Temperature->styleSheet();
    QString stylePres = ui->pushButton_Presence->styleSheet();

    if (!styleLumi.isEmpty()) {
        tableActive = "mesures_luminosite";
    } else if (!styleTemp.isEmpty()) {
        tableActive = "mesures_humidite_temperature";
    } else if (!stylePres.isEmpty()) {
        tableActive = "mesures_presence";
    }

    if (!tableActive.isEmpty()) {
        chargerTable(tableActive);
    }

    ui->tableView_Affichage->setVisible(true);
}

void AdminWindow::closeEvent(QCloseEvent *event)
{
    if (parentWidget() != nullptr) {
        parentWidget()->show();
    }
    event->accept();
}

void AdminWindow::on_pushButton_Fermer_clicked()
{
    if (parentWidget() != nullptr) {
        parentWidget()->show();
    }
    this->close();
}

void AdminWindow::mettreAJourLabelTemperature()
{
    QSqlQuery query(db);
    query.exec("SELECT temperature FROM mesures_humidite_temperature ORDER BY date DESC LIMIT 1");

    if (query.next()) {
        int temperature = query.value(0).toInt();
        QString texte = QString::number(temperature) + " °C";
        ui->label_Temperature->setText(texte);
    } else {
        ui->label_Temperature->setText("-- °C");
    }
}

void AdminWindow::mettreAJourEtatCapteurs()
{
    if (!db.isOpen()) {
        return;
    }
    QSqlQuery query(db);
    query.exec("SELECT luminosite, temperature FROM etat_capteurs LIMIT 1");
    if (!query.next()) {
        return;
    }
    int etatLumiere     = query.value(0).toInt();
    int etatTemperature = query.value(1).toInt();

    if (etatLumiere == 1) {
        ui->bulb_on->setVisible(true);
        ui->bulb_off->setVisible(false);
    } else {
        ui->bulb_on->setVisible(false);
        ui->bulb_off->setVisible(true);
    }

    if (etatTemperature != etatVentilateur) {
        etatVentilateur = etatTemperature;
        mettreAJourVentilateur(etatTemperature);
    }

    QSqlQuery queryPresence(db);
    queryPresence.exec("SELECT presence FROM mesures_presence ORDER BY date DESC LIMIT 1");
    if (queryPresence.next()) {
        int etatPresence = queryPresence.value(0).toInt();

        if (moviePresence == nullptr) {
            return;
        }

        if (etatPresence == 1) {
            // Présence détectée → GIF animé
            ui->label_Presence->setVisible(true);
            ui->label_Presence->setMovie(moviePresence);
            moviePresence->start();

            ui->label_Presence_Statique->setVisible(false);
        } else {
            // Pas de présence → image statique
            moviePresence->stop();
            ui->label_Presence->setVisible(false);

            ui->label_Presence_Statique->setVisible(true);
            QPixmap image(":/images/Ressources/Running_Stickman.png");
            QPixmap imageRedimensionnee = image.scaled(ui->label_Presence_Statique->size(), Qt::KeepAspectRatio);
            ui->label_Presence_Statique->setPixmap(imageRedimensionnee);
        }
    }
}
