//adminwindow.cpp

#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QSslSocket>

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    qDebug() << "SSL supporté :" << QSslSocket::supportsSsl();
    qDebug() << "Version SSL :" << QSslSocket::sslLibraryVersionString();
    setWindowTitle("Page Administrateur - Supervision Salle De Sport");

    ui->label_Logo->setPixmap(QPixmap(":/images/Ressources/AdminLogo.jpg"));

    connecterBDD();

    QSqlQuery queryLampe(db);
    queryLampe.exec("SELECT luminosite FROM mesures_luminosite ORDER BY date DESC LIMIT 1");
    if (queryLampe.next()) {
        int etatLampe = queryLampe.value(0).toInt();
        ui->bulb_on->setVisible(etatLampe == 1);
        ui->bulb_off->setVisible(etatLampe == 0);
    } else {
        ui->bulb_on->setVisible(false);
        ui->bulb_off->setVisible(false);
    }

    QSqlQuery queryVentilo(db);
    queryVentilo.exec("SELECT temperature FROM mesures_humidite_temperature ORDER BY date DESC LIMIT 1");
    if (queryVentilo.next()) {
        etatVentilateur = queryVentilo.value(0).toInt();
    } else {
        etatVentilateur = 0;
    }

    movieVentilo = new QMovie(":/images/Ressources/ventilo_on.gif");
    ui->label_Ventilateur->setScaledContents(true);
    ui->label_Ventilateur->setVisible(false);

    chargerTable("mesures_luminosite");
    afficherWidgetsAffichage();
}

AdminWindow::~AdminWindow()
{
    if (db.isOpen())
        db.close();
    delete ui;
}

void AdminWindow::connecterBDD()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("172.19.6.120"); //127.0.0.0
    db.setPort(3306);
    db.setDatabaseName("supervision_salle_de_sport2");
    db.setUserName("api");
    db.setPassword("api");
    //db.setConnectOptions("MYSQL_OPT_SSL_MODE=SSL_MODE_DISABLED");

    //db = QSqlDatabase::addDatabase("QMYSQL");
    //db.setHostName("localhost");
    //db.setPort(3306);
    //db.setDatabaseName("supervision_salle_de_sport2");
    //db.setUserName("root");
    //db.setPassword("");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur BDD",
            "Connexion échouée : " + db.lastError().text());
        qDebug() << "Erreur BDD :" << db.lastError().text();
    } else {
        qDebug() << "Connexion BDD réussie !";
        mettreAJourLabelTemperature();
    }
}

void AdminWindow::chargerTable(const QString &nomTable)
{
    if (!db.isOpen())
        return;

    QSqlQuery query(db);

    if (nomTable == "mesures_luminosite" || nomTable == "mesures_presence") {
        query.exec(QString("SELECT * FROM %1 ORDER BY date DESC LIMIT 1").arg(nomTable));
    } else {
        query.exec(QString("SELECT * FROM %1 ORDER BY date DESC").arg(nomTable));
    }

    QStandardItemModel *modeleTableau = new QStandardItemModel(this);

    QSqlRecord enregistrement = query.record();
    QStringList nomsColonnes;
    for (int i = 0; i < enregistrement.count(); i++) {
        nomsColonnes << enregistrement.fieldName(i);
    }
    modeleTableau->setHorizontalHeaderLabels(nomsColonnes);

    while (query.next()) {
        QList<QStandardItem*> ligne;
        for (int i = 0; i < enregistrement.count(); i++) {
            ligne << new QStandardItem(query.value(i).toString());
        }
        modeleTableau->appendRow(ligne);
    }

    ui->tableView_Affichage->setModel(modeleTableau);
    ui->tableView_Affichage->resizeColumnsToContents();
    ui->tableView_Affichage->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_Affichage->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QString styleActif = "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }";

    ui->pushButton_Luminosite->setStyleSheet("");
    ui->pushButton_Temperature->setStyleSheet("");
    ui->pushButton_Presence->setStyleSheet("");

    if (nomTable == "mesures_luminosite")
        ui->pushButton_Luminosite->setStyleSheet(styleActif);
    else if (nomTable == "mesures_humidite_temperature")
        ui->pushButton_Temperature->setStyleSheet(styleActif);
    else if (nomTable == "mesures_presence")
        ui->pushButton_Presence->setStyleSheet(styleActif);
}

void AdminWindow::initialiserTableauGestion()
{
    ui->tableWidget_Gestion->setColumnCount(3);
    ui->tableWidget_Gestion->setHorizontalHeaderLabels({"Mesure", "Allumer", "Eteindre"});

    QStringList mesures  = {"Lumières", "Température", "Présence"};
    QStringList tables   = {"mesures_luminosite", "mesures_humidite_temperature", "mesures_presence"};
    QStringList colonnes = {"luminosite", "temperature", "presence"};

    ui->tableWidget_Gestion->setRowCount(mesures.size());

    for (int i = 0; i < mesures.size(); i++) {
        ui->tableWidget_Gestion->setItem(i, 0, new QTableWidgetItem(mesures[i]));

        for (const QString &action : {"allumer", "eteindre"}) {
            QString labelBouton = (action == "allumer") ? "Allumer" : "Eteindre";
            QPushButton *bouton = new QPushButton(labelBouton);
            bouton->setFixedWidth(80);
            bouton->setProperty("action",  action);
            bouton->setProperty("table",   tables[i]);
            bouton->setProperty("colonne", colonnes[i]);
            connect(bouton, &QPushButton::clicked,
                    this, &AdminWindow::on_boutonGestion_clicked);
            int colonne = (action == "allumer") ? 1 : 2;
            ui->tableWidget_Gestion->setCellWidget(i, colonne, bouton);
        }
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
        ui->label_Ventilateur->setPixmap(
            QPixmap(":/images/Ressources/ventilo_off.png")
            .scaled(ui->label_Ventilateur->size(), Qt::KeepAspectRatio));
    }
}

void AdminWindow::on_boutonGestion_clicked()
{
    QPushButton *bouton = qobject_cast<QPushButton*>(sender());
    if (!bouton || !db.isOpen()) return;

    QString table   = bouton->property("table").toString();
    QString colonne = bouton->property("colonne").toString();
    QString action  = bouton->property("action").toString();
    int etat        = (action == "allumer") ? 1 : 0;

    // ── La température contrôle uniquement le ventilateur visuel ──
    // On ne touche pas à la BDD pour cette mesure
    if (table == "mesures_humidite_temperature") {
        etatVentilateur = etat;
        mettreAJourVentilateur(etat);
        return; // ← on sort sans faire d'INSERT
    }

    // ── Pour luminosité et présence : INSERT en BDD ────────────────
    QSqlQuery query(db);
    query.prepare(
        QString("INSERT INTO %1 (%2, date, zone) VALUES (:etat, NOW(), 1)")
        .arg(table).arg(colonne)
    );
    query.bindValue(":etat", etat);

    if (query.exec()) {
        qDebug() << table << ":" << (etat == 1 ? "Allumé" : "Éteint");
        chargerTable(table);

        if (table == "mesures_luminosite") {
            ui->bulb_on->setVisible(etat == 1);
            ui->bulb_off->setVisible(etat == 0);
        }
    } else {
        QMessageBox::critical(this, "Erreur",
            "Échec de la requête : " + query.lastError().text());
    }
}

void AdminWindow::chargerHistorique()
{
    if (!db.isOpen())
        return;

    QSqlQuery query(db);
    query.exec(
        "SELECT 'Luminosité' AS type, luminosite AS valeur, date FROM mesures_luminosite "
        "UNION ALL "
        "SELECT 'Présence' AS type, presence AS valeur, date FROM mesures_presence "
        "UNION ALL "
        "SELECT 'Température' AS type, temperature AS valeur, date FROM mesures_humidite_temperature "
        "ORDER BY date DESC "
        "LIMIT 100"
    );

    QStandardItemModel *modeleHistorique = new QStandardItemModel(this);
    modeleHistorique->setHorizontalHeaderLabels({"Type", "Valeur", "Date / Heure"});

    while (query.next()) {
        QString type   = query.value(0).toString();
        QString valeur = query.value(1).toString();
        QString date   = query.value(2).toDateTime().toString("dd/MM/yyyy HH:mm:ss");

        if (type == "Luminosité")
            valeur = (query.value(1).toInt() == 1) ? "Allumé" : "Éteint";
        else if (type == "Présence")
            valeur = (query.value(1).toInt() == 1) ? "Présent" : "Absent";

        QList<QStandardItem*> ligne;
        ligne << new QStandardItem(type);
        ligne << new QStandardItem(valeur);
        ligne << new QStandardItem(date);
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

    ui->pushButton_Affichage->setStyleSheet(
        "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }");
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

    ui->pushButton_Gestion->setStyleSheet(
        "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }");
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

    ui->pushButton_Historique->setStyleSheet(
        "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }");
    ui->pushButton_Affichage->setStyleSheet("");
    ui->pushButton_Gestion->setStyleSheet("");

    if (etatVentilateur == 1) {
        ui->label_Ventilateur->setVisible(true);
        ui->label_Ventilateur->setMovie(movieVentilo);
        movieVentilo->start();
    }
}

void AdminWindow::on_pushButton_Affichage_clicked()  { afficherWidgetsAffichage(); }
void AdminWindow::on_pushButton_Gestion_clicked()    { afficherWidgetsGestion(); }
void AdminWindow::on_pushButton_Historique_clicked() { afficherWidgetsHistorique(); }

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

    QTimer::singleShot(500, this, [this]() {
        QString tableActive = "";

        if (!ui->pushButton_Luminosite->styleSheet().isEmpty())
            tableActive = "mesures_luminosite";
        else if (!ui->pushButton_Temperature->styleSheet().isEmpty())
            tableActive = "mesures_humidite_temperature";
        else if (!ui->pushButton_Presence->styleSheet().isEmpty())
            tableActive = "mesures_presence";

        if (!tableActive.isEmpty())
            chargerTable(tableActive);

        ui->tableView_Affichage->setVisible(true);
    });
}

void AdminWindow::closeEvent(QCloseEvent *event)
{
    if (parentWidget())
        parentWidget()->show();
    event->accept();
}

void AdminWindow::on_pushButton_Fermer_clicked()
{
    if (parentWidget())
        parentWidget()->show();
    this->close();
}

void AdminWindow::mettreAJourLabelTemperature()
{
    QSqlQuery q(db);
    q.exec("SELECT temperature FROM mesures_humidite_temperature ORDER BY date DESC LIMIT 1");
    if (q.next()) {
        ui->label_Temperature->setText(QString("%1 °C").arg(q.value(0).toInt()));
    } else {
        ui->label_Temperature->setText("-- °C");
    }
}
