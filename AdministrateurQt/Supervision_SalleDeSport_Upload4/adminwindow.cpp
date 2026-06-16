#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>


// CONSTRUCTEUR

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    setWindowTitle("Page Administrateur - Supervision Salle De Sport");
    ui->label_Administrateur->setPixmap(QPixmap(":/images/Ressources/AdminLogo.jpg"));

    movieVentilo = new QMovie(":/images/Ressources/ventilo_on.gif");
    ui->label_Fan->setScaledContents(true);
    ui->label_Fan->setVisible(false);

    connecterBDD();

    QSqlQuery queryVentilo(db);
    queryVentilo.exec("SELECT ventilateur FROM mesures_humidite_temperature ORDER BY date DESC LIMIT 1"); //Check etat ventilateur
    if (queryVentilo.next())
    {
        etatVentilateur = queryVentilo.value(0).toInt();
    }
    else
    {
        etatVentilateur = 0;
    }

    chargerTable("mesures_luminosite"); //table par défaut
    afficherWidgetsAffichage(); //495
}

AdminWindow::~AdminWindow() //Destructeur
{
    if (db.isOpen())
    {
        db.close();
    }
    delete ui;
}


// CONNEXION BASE DE DONNEES

void AdminWindow::connecterBDD()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("172.19.6.120");
    db.setPort(3306);
    db.setDatabaseName("supervision_salle_de_sport2");
    db.setUserName("api");
    db.setPassword("api");

    if (!db.open())
    {
        QMessageBox::critical(this, "Erreur BDD",
            "Connexion échouée : " + db.lastError().text());
    }
    else
    {
        mettreAJourLabelTemperature();

        timerEtatCapteurs = new QTimer(this);
        connect(timerEtatCapteurs, SIGNAL(timeout()), this, SLOT(mettreAJourEtatCapteurs()));
        timerEtatCapteurs->start(5000);
        mettreAJourEtatCapteurs();
    }
}


// TABLEVIEW AFFICHAGE

void AdminWindow::chargerTable(const QString &nomTable)
{
    if (!db.isOpen())
    {
        return;
    }

    QSqlQuery query(db);
    QString requete = "SELECT * FROM " + nomTable + " ORDER BY date DESC"; //Requete
    query.exec(requete);

    QStandardItemModel *modeleTableau  = new QStandardItemModel(this);
    QSqlRecord          enregistrement = query.record();

    QStringList nomsColonnes;
    int         nombreColonnes = enregistrement.count();
    for (int i = 0; i < nombreColonnes; i = i + 1)
    {
        QString nomColonne = enregistrement.fieldName(i);
        nomsColonnes.append(nomColonne);
    }
    modeleTableau->setHorizontalHeaderLabels(nomsColonnes);

    while (query.next())
    {
        QList<QStandardItem*> ligne;
        for (int i = 0; i < nombreColonnes; i = i + 1)
        {
            QString nomColonne = enregistrement.fieldName(i);
            QString valeur     = query.value(i).toString();

            if (nomColonne == "date")
            {
                QDateTime dateHeure = query.value(i).toDateTime();
                valeur = dateHeure.toString("dd/MM/yyyy HH:mm:ss");
            }

            if (nomColonne == "id_zone")
            {
                QSqlQuery requeteZone(db);
                requeteZone.prepare("SELECT description FROM zone WHERE id_zone = :id");
                requeteZone.bindValue(":id", query.value(i).toInt());
                if (requeteZone.exec() && requeteZone.next())
                {
                    valeur = requeteZone.value(0).toString();
                }
            }

            QStandardItem *cellule = new QStandardItem(valeur);
            ligne.append(cellule);
        }
        modeleTableau->appendRow(ligne);
    }

    ui->tableView_Display->setModel(modeleTableau);
    ui->tableView_Display->resizeColumnsToContents();
    ui->tableView_Display->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_Display->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->pushButton_Luminosite->setStyleSheet("");
    ui->pushButton_Temperature->setStyleSheet("");
    ui->pushButton_Presence->setStyleSheet("");

    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";

    if (nomTable == "mesures_luminosite")
    {
        ui->pushButton_Luminosite->setStyleSheet(styleActif);
        ui->label_Categorie->setText("Luminosité");
    }
    else if (nomTable == "mesures_humidite_temperature")
    {
        ui->pushButton_Temperature->setStyleSheet(styleActif);
        ui->label_Categorie->setText("Température / Humidité");
    }
    else if (nomTable == "mesures_presence")
    {
        ui->pushButton_Presence->setStyleSheet(styleActif);
        ui->label_Categorie->setText("Présence");
    }
}


// CHARGEMENT TABLEVIEW HISTORIQUE

void AdminWindow::chargerHistorique()
{
    if (!db.isOpen())
    {
        return;
    }

    QStandardItemModel *modeleHistorique = new QStandardItemModel(this);
    QStringList entetes;
    entetes.append("Type");
    entetes.append("Valeur");
    entetes.append("Date / Heure");
    modeleHistorique->setHorizontalHeaderLabels(entetes);

    QSqlQuery queryPresence(db);
    queryPresence.exec("SELECT presence, date FROM mesures_presence ORDER BY date ASC");

    int presencePrecedente = -1;
    while (queryPresence.next())
    {
        int     presence = queryPresence.value(0).toInt();
        QString date     = queryPresence.value(1).toDateTime().toString("dd/MM/yyyy HH:mm:ss");

        if (presencePrecedente == 0 && presence == 1)
        {
            QList<QStandardItem*> ligne;
            ligne.append(new QStandardItem("Présence"));
            ligne.append(new QStandardItem("Présence détectée"));
            ligne.append(new QStandardItem(date));
            modeleHistorique->appendRow(ligne);
        }
        presencePrecedente = presence;
    }

    QSqlQuery queryTemp(db);
    queryTemp.exec("SELECT temperature, humidite, date FROM mesures_humidite_temperature ORDER BY date ASC");

    int tempPrecedente = -1;
    while (queryTemp.next())
    {
        int     temperature = queryTemp.value(0).toInt();
        int     humidite    = queryTemp.value(1).toInt();
        QString date        = queryTemp.value(2).toDateTime().toString("dd/MM/yyyy HH:mm:ss");

        if (tempPrecedente != -1 && temperature != tempPrecedente)
        {
            QString valeur = QString::number(tempPrecedente) + "°C -> "
                           + QString::number(temperature)   + "°C  ("
                           + QString::number(humidite)      + "%)";

            QList<QStandardItem*> ligne;
            ligne.append(new QStandardItem("Température"));
            ligne.append(new QStandardItem(valeur));
            ligne.append(new QStandardItem(date));
            modeleHistorique->appendRow(ligne);
        }
        tempPrecedente = temperature;
    }

    QSqlQuery queryVentilo(db);
    queryVentilo.exec("SELECT ventilateur, date FROM mesures_humidite_temperature ORDER BY date ASC");

    int ventiloPrecedent = -1;
    while (queryVentilo.next())
    {
        int     ventilo = queryVentilo.value(0).toInt();
        QString date    = queryVentilo.value(1).toDateTime().toString("dd/MM/yyyy HH:mm:ss");

        if (ventiloPrecedent != -1 && ventilo != ventiloPrecedent)
        {
            QString valeur = "";
            if (ventilo == 1)
            {
                valeur = "Ventilateur allumé";
            }
            else
            {
                valeur = "Ventilateur éteint";
            }

            QList<QStandardItem*> ligne;
            ligne.append(new QStandardItem("Ventilateur"));
            ligne.append(new QStandardItem(valeur));
            ligne.append(new QStandardItem(date));
            modeleHistorique->appendRow(ligne);
        }
        ventiloPrecedent = ventilo;
    }

    modeleHistorique->sort(2, Qt::DescendingOrder);

    ui->tableView_History->setModel(modeleHistorique);
    ui->tableView_History->resizeColumnsToContents();
    ui->tableView_History->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


// TABLEAU GESTION — BOUTONS DYNAMIQUES

void AdminWindow::initialiserTableauGestion()
{
    ui->tableWidget_Management->setColumnCount(3);

    QStringList entetes;
    entetes.append("Mesure");
    entetes.append("Allumer");
    entetes.append("Eteindre");
    ui->tableWidget_Management->setHorizontalHeaderLabels(entetes);

    QStringList mesures;
    mesures.append("Ventilateur");
    mesures.append("Lumières");
    mesures.append("Présence");

    QStringList tables;
    tables.append("mesures_humidite_temperature");
    tables.append("mesures_luminosite");
    tables.append("mesures_presence");

    QStringList colonnes;
    colonnes.append("ventilateur");
    colonnes.append("luminosite");
    colonnes.append("presence");

    ui->tableWidget_Management->setRowCount(mesures.size());

    for (int i = 0; i < mesures.size(); i = i + 1)
    {
        QTableWidgetItem *itemMesure = new QTableWidgetItem(mesures.at(i));
        ui->tableWidget_Management->setItem(i, 0, itemMesure);

        if (mesures.at(i) == "Présence" || mesures.at(i) == "Lumières")
        {
            QTableWidgetItem *itemAuto = new QTableWidgetItem("Automatique");
            itemAuto->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Management->setItem(i, 1, itemAuto);
            ui->tableWidget_Management->setSpan(i, 1, 1, 2);
            continue;
        }

        QPushButton *boutonAllumer = new QPushButton("Allumer");
        boutonAllumer->setFixedWidth(80);
        boutonAllumer->setProperty("action", "allumer");
        boutonAllumer->setProperty("table", tables.at(i));
        boutonAllumer->setProperty("colonne", colonnes.at(i));
        connect(boutonAllumer, SIGNAL(clicked()), this, SLOT(on_boutonGestion_clicked()));
        ui->tableWidget_Management->setCellWidget(i, 1, boutonAllumer);

        QPushButton *boutonEteindre = new QPushButton("Eteindre");
        boutonEteindre->setFixedWidth(80);
        boutonEteindre->setProperty("action", "eteindre");
        boutonEteindre->setProperty("table", tables.at(i));
        boutonEteindre->setProperty("colonne", colonnes.at(i));
        connect(boutonEteindre, SIGNAL(clicked()), this, SLOT(on_boutonGestion_clicked()));
        ui->tableWidget_Management->setCellWidget(i, 2, boutonEteindre);
    }

    ui->tableWidget_Management->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_Management->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


// VENTILATEUR GIF

void AdminWindow::mettreAJourVentilateur(int etat)
{
    ui->label_Fan->setVisible(true);

    if (etat == 1)
    {
        ui->label_Fan->setMovie(movieVentilo);
        movieVentilo->start();
    }
    else
    {
        movieVentilo->stop();
        QPixmap image(":/images/Ressources/ventilo_off.png");
        QPixmap imageRedimensionnee = image.scaled(ui->label_Fan->size(), Qt::KeepAspectRatio);
        ui->label_Fan->setPixmap(imageRedimensionnee);
    }
}


// BOUTON GESTION — SLOT UNIQUE ALLUMER / ETEINDRE

void AdminWindow::on_boutonGestion_clicked()
{
    QPushButton *bouton = qobject_cast<QPushButton*>(sender());
    if (bouton == nullptr)
    {
        return;
    }
    if (!db.isOpen())
    {
        return;
    }

    QString table   = bouton->property("table").toString();
    QString colonne = bouton->property("colonne").toString();
    QString action  = bouton->property("action").toString();

    int etat = 0;
    if (action == "allumer")
    {
        etat = 1;
    }
    else
    {
        etat = 0;
    }

    if (table == "mesures_humidite_temperature")
    {
        etatVentilateur = etat;
        mettreAJourVentilateur(etat);

        QSqlQuery queryVentilo(db);
        queryVentilo.prepare("UPDATE mesures_humidite_temperature SET ventilateur = :etat ORDER BY date DESC LIMIT 1");
        queryVentilo.bindValue(":etat", etat);
        queryVentilo.exec();

        return;
    }

    QSqlQuery query(db);
    QString   requeteInsert = "INSERT INTO " + table + " (" + colonne + ", date, id_zone) VALUES (:etat, NOW(), 1)";
    query.prepare(requeteInsert);
    query.bindValue(":etat", etat);

    if (query.exec())
    {
        chargerTable(table);
    }
    else
    {
        QMessageBox::critical(this, "Erreur",
            "Échec de la requête : " + query.lastError().text());
    }
}


// LABELS TEMPERATURE ET HUMIDITE

void AdminWindow::mettreAJourLabelTemperature()
{
    QSqlQuery query(db);
    query.exec("SELECT temperature, humidite FROM mesures_humidite_temperature ORDER BY date DESC LIMIT 1");

    if (query.next())
    {
        int temperature = query.value(0).toInt();
        int humidite    = query.value(1).toInt();

        QString texteTemp = QString::number(temperature) + " °C";
        QString texteHumi = QString::number(humidite)    + " %";

        ui->label_Temperature->setText(texteTemp);
        ui->label_Etat_Humidite->setText(texteHumi);
    }
    else
    {
        ui->label_Temperature->setText("-- °C");
        ui->label_Etat_Humidite->setText("-- %");
    }
}


// MISE A JOUR ETAT CAPTEURS — TIMER 5 SECONDES

void AdminWindow::mettreAJourEtatCapteurs()
{
    if (!db.isOpen())
    {
        return;
    }

    mettreAJourLabelTemperature();

    QSqlQuery queryPresence(db);
    queryPresence.exec("SELECT presence FROM mesures_presence ORDER BY date DESC LIMIT 1");
    if (queryPresence.next())
    {
        int etatPresence = queryPresence.value(0).toInt();

        if (etatPresence == 1)
        {
            QPixmap imageVerte(":/images/Ressources/Green_Figure.png");
            QPixmap imageRedimensionnee = imageVerte.scaled(ui->label_Figure_Green->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->label_Figure_Green->setPixmap(imageRedimensionnee);
            ui->label_Figure_Green->setVisible(true);
            ui->label_Figure_Red->setVisible(false);
            ui->Label_bulb_on->setVisible(true);
            ui->Label_bulb_off->setVisible(false);
            ui->label_Etat_Luminosite->setText("Allumé");
            ui->label_Etat_Presence->setText("Détecté");
        }
        else
        {
            QPixmap imageRouge(":/images/Ressources/Red_Figure.png");
            QPixmap imageRedimensionnee = imageRouge.scaled(ui->label_Figure_Red->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->label_Figure_Red->setPixmap(imageRedimensionnee);
            ui->label_Figure_Red->setVisible(true);
            ui->label_Figure_Green->setVisible(false);
            ui->Label_bulb_on->setVisible(false);
            ui->Label_bulb_off->setVisible(true);
            ui->label_Etat_Luminosite->setText("Éteint");
            ui->label_Etat_Presence->setText("Non Détecté");
        }
    }

    QSqlQuery query(db);
    query.exec("SELECT ventilateur FROM mesures_humidite_temperature ORDER BY date DESC LIMIT 1");
    if (!query.next())
    {
        return;
    }

    int etatTemperature = query.value(0).toInt();

    if (etatTemperature != etatVentilateur)
    {
        etatVentilateur = etatTemperature;
        mettreAJourVentilateur(etatTemperature);
    }
}


// NAVIGATION ONGLETS AFFICHAGE - GESTION - HISTORIQUE

void AdminWindow::afficherWidgetsAffichage()
{
    ui->tableView_Display->setVisible(true);
    ui->pushButton_Luminosite->setVisible(true);
    ui->pushButton_Temperature->setVisible(true);
    ui->pushButton_Presence->setVisible(true);
    ui->pushButton_Refresh->setVisible(true);
    ui->tableWidget_Management->setVisible(false);
    ui->tableView_History->setVisible(false);

    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";
    ui->pushButton_Display->setStyleSheet(styleActif);
    ui->pushButton_Management->setStyleSheet("");
    ui->pushButton_History->setStyleSheet("");

    if (etatVentilateur == 1)
    {
        ui->label_Fan->setVisible(true);
        ui->label_Fan->setMovie(movieVentilo);
        movieVentilo->start();
    }
}

void AdminWindow::afficherWidgetsGestion()
{
    ui->tableView_Display->setVisible(false);
    ui->pushButton_Luminosite->setVisible(false);
    ui->pushButton_Temperature->setVisible(false);
    ui->pushButton_Presence->setVisible(false);
    ui->pushButton_Refresh->setVisible(false);
    ui->tableView_History->setVisible(false);
    ui->tableWidget_Management->setVisible(true);

    initialiserTableauGestion();

    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";
    ui->pushButton_Management->setStyleSheet(styleActif);
    ui->pushButton_Display->setStyleSheet("");
    ui->pushButton_History->setStyleSheet("");

    mettreAJourVentilateur(etatVentilateur);
}

void AdminWindow::afficherWidgetsHistorique()
{
    ui->tableView_Display->setVisible(false);
    ui->pushButton_Luminosite->setVisible(false);
    ui->pushButton_Temperature->setVisible(false);
    ui->pushButton_Presence->setVisible(false);
    ui->pushButton_Refresh->setVisible(false);
    ui->tableWidget_Management->setVisible(false);
    ui->tableView_History->setVisible(true);

    chargerHistorique();

    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";
    ui->pushButton_History->setStyleSheet(styleActif);
    ui->pushButton_Display->setStyleSheet("");
    ui->pushButton_Management->setStyleSheet("");

    if (etatVentilateur == 1)
    {
        ui->label_Fan->setVisible(true);
        ui->label_Fan->setMovie(movieVentilo);
        movieVentilo->start();
    }
}


// BOUTONS ONGLETS

void AdminWindow::on_pushButton_Display_clicked()
{
    afficherWidgetsAffichage();
}

void AdminWindow::on_pushButton_Management_clicked()
{
    afficherWidgetsGestion();
}

void AdminWindow::on_pushButton_History_clicked()
{
    afficherWidgetsHistorique();
}


// BOUTONS CAPTEURS

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


// BOUTON ACTUALISER

void AdminWindow::on_pushButton_Refresh_clicked()
{
    ui->tableView_Display->setVisible(false);
    QTimer::singleShot(500, this, SLOT(reafficherTableau()));
}

void AdminWindow::reafficherTableau()
{
    QString tableActive = "";

    QString styleLumi = ui->pushButton_Luminosite->styleSheet();
    QString styleTemp = ui->pushButton_Temperature->styleSheet();
    QString stylePres = ui->pushButton_Presence->styleSheet();

    if (!styleLumi.isEmpty())
    {
        tableActive = "mesures_luminosite";
    }
    else if (!styleTemp.isEmpty())
    {
        tableActive = "mesures_humidite_temperature";
    }
    else if (!stylePres.isEmpty())
    {
        tableActive = "mesures_presence";
    }

    if (!tableActive.isEmpty())
    {
        chargerTable(tableActive);
    }

    ui->tableView_Display->setVisible(true);
}


// FERMETURE FENETRE

void AdminWindow::closeEvent(QCloseEvent *event)
{
    if (parentWidget() != nullptr)
    {
        parentWidget()->show();
    }
    event->accept();
}

void AdminWindow::on_pushButton_Close_clicked()
{
    if (parentWidget() != nullptr)
    {
        parentWidget()->show();
    }
    this->close();
}
