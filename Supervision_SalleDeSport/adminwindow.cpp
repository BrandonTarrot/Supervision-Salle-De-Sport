#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QDebug>

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    setWindowTitle("Page Administrateur - Supervision Salle De Sport");

    // Chargement du logo
    ui->label_Logo->setPixmap(QPixmap(":/images/Ressources/AdminLogo.jpg"));

    // Connexion BDD
    connecterBDD();
    // Init ventilateur
    movieVentilo = new QMovie(":/images/Ressources/ventilo_on.gif");
    ui->label_Ventilateur->setScaledContents(true);
    ui->label_Ventilateur->setVisible(false); // caché par défaut

    // Démarre sur Affichage par défaut
    chargerTable("mesures_luminosite");
    afficherWidgetsAffichage();
}

void AdminWindow::connecterBDD()
{
    // ═══════════════════════════════════════════════════
    // CONNEXION LOCALE (WAMP) ← active actuellement
    // ═══════════════════════════════════════════════════
    db = QSqlDatabase::addDatabase("QMYSQL");  // ← pas de "QSqlDatabase" devant db !
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("supervision_salle_de_sport2");
    db.setUserName("root");
    db.setPassword("");

    // ═══════════════════════════════════════════════════
    // CONNEXION RASPBERRY PI ← désactivée pour l'instant
    // ═══════════════════════════════════════════════════
    // db = QSqlDatabase::addDatabase("QMYSQL");
    // db.setHostName("172.19.6.120");
    // db.setPort(3306);
    // db.setDatabaseName("supervision_salle_de_sport2");
    // db.setUserName("api");
    // db.setPassword("api");
    // db.setConnectOptions("MYSQL_OPT_SSL_MODE=SSL_MODE_DISABLED");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur BDD",
            "Connexion échouée : " + db.lastError().text());
        qDebug() << "Erreur :" << db.lastError().text();
    } else {
        qDebug() << "Connexion BDD réussie !";
    }
}

// Charge la table demandée dans le TableView
void AdminWindow::chargerTable(const QString &nomTable)
{
    if (!db.isOpen()) return;

    model = new QSqlTableModel(this, db);
    model->setTable(nomTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    ui->tableView_Affichage->setModel(model);
    ui->tableView_Affichage->resizeColumnsToContents();
    ui->tableView_Affichage->setSelectionBehavior(QAbstractItemView::SelectRows);
}

// Initialise le tableau Gestion avec boutons Allumer/Eteindre
void AdminWindow::initialiserTableauGestion()
{
    ui->tableWidget_Gestion->setColumnCount(3);
    ui->tableWidget_Gestion->setHorizontalHeaderLabels({"Mesure", "Allumer", "Eteindre"});

    // ← Ligne 70 : ajouter/supprimer des mesures ici
    QStringList mesures  = {"Lumières", "Température", "Présence"};
    // ← Ligne 72 : tables BDD correspondantes
    QStringList tables   = {"mesures_luminosite", "mesures_humidite_temperature", "mesures_presence"};
    // ← Ligne 74 : colonnes à modifier
    QStringList colonnes = {"luminosite", "temperature", "presence"};

    ui->tableWidget_Gestion->setRowCount(mesures.size());

    for (int i = 0; i < mesures.size(); i++) {
        // Colonne 0 : nom de la mesure
        ui->tableWidget_Gestion->setItem(i, 0, new QTableWidgetItem(mesures[i]));

        // Colonne 1 : bouton Allumer
        QPushButton *btnAllumer = new QPushButton("Allumer");
        btnAllumer->setProperty("action", "allumer");
        btnAllumer->setProperty("table", tables[i]);
        btnAllumer->setProperty("colonne", colonnes[i]);
        connect(btnAllumer, &QPushButton::clicked, this, &AdminWindow::on_boutonGestion_clicked);
        ui->tableWidget_Gestion->setCellWidget(i, 1, btnAllumer);

        // Colonne 2 : bouton Eteindre
        QPushButton *btnEteindre = new QPushButton("Eteindre");
        btnEteindre->setProperty("action", "eteindre");
        btnEteindre->setProperty("table", tables[i]);
        btnEteindre->setProperty("colonne", colonnes[i]);
        btnAllumer->setFixedWidth(80);
        btnEteindre->setFixedWidth(80);
        connect(btnEteindre, &QPushButton::clicked, this, &AdminWindow::on_boutonGestion_clicked);
        ui->tableWidget_Gestion->setCellWidget(i, 2, btnEteindre);
    }

    ui->tableWidget_Gestion->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_Gestion->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void AdminWindow::on_boutonGestion_clicked()
{
    QPushButton *bouton = qobject_cast<QPushButton*>(sender());
    if (!bouton || !db.isOpen()) return;

    QString table   = bouton->property("table").toString();
    QString colonne = bouton->property("colonne").toString();
    int etat        = (bouton->property("action").toString() == "allumer") ? 1 : 0;

    QSqlQuery query(db);
    query.prepare(QString("UPDATE %1 SET %2 = :etat, date = NOW() WHERE zone = 1")
                  .arg(table).arg(colonne));
    query.bindValue(":etat", etat);

    if (query.exec()) {
        qDebug() << table << ":" << (etat == 1 ? "Allumé" : "Éteint");

        // ← Gestion du ventilateur si c'est la table température
        if (table == "mesures_humidite_temperature") {
            if (etat == 1) {
                ui->label_Ventilateur->setVisible(true);
                ui->label_Ventilateur->setMovie(movieVentilo);
                movieVentilo->start();
            } else {
                movieVentilo->stop();
                ui->label_Ventilateur->setPixmap(
                    QPixmap(":/images/Ressources/ventilo_off.png").scaled(
                        ui->label_Ventilateur->size(), Qt::KeepAspectRatio));
            }
        }
    } else {
        QMessageBox::critical(this, "Erreur", "Échec : " + query.lastError().text());
    }
}

// Charge l'historique des modifications dans tableView_Historique
void AdminWindow::chargerHistorique()
{
    if (!db.isOpen()) return;

    // Récupère les dernières modifications de toutes les tables avec heure
    QSqlQuery query(
        "SELECT 'Luminosité' AS type, luminosite AS valeur, date FROM mesures_luminosite "
        "UNION ALL "
        "SELECT 'Présence' AS type, presence AS valeur, date FROM mesures_presence "
        "ORDER BY date DESC "
        "LIMIT 50", db
    );

    // Remplissage manuel du tableView_Historique
    ui->tableView_Historique->setModel(nullptr);

    QStandardItemModel *historiqueModel = new QStandardItemModel(this);
    historiqueModel->setHorizontalHeaderLabels({"Type", "Valeur", "Date / Heure"});

    while (query.next()) {
        QString type   = query.value(0).toString();
        QString valeur = query.value(1).toInt() == 1 ? "Allumé / Présence" : "Éteint / Absent";
        QString date   = query.value(2).toDateTime().toString("dd/MM/yyyy HH:mm:ss");

        QList<QStandardItem*> ligne;
        ligne << new QStandardItem(type)
              << new QStandardItem(valeur)
              << new QStandardItem(date);
        historiqueModel->appendRow(ligne);
    }

    ui->tableView_Historique->setModel(historiqueModel);
    ui->tableView_Historique->resizeColumnsToContents();
    ui->tableView_Historique->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// Affiche widgets Affichage, cache le reste
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
}

void AdminWindow::afficherWidgetsGestion()
{
    ui->tableView_Affichage->setVisible(false);
    ui->pushButton_Luminosite->setVisible(false);
    ui->pushButton_Temperature->setVisible(false);
    ui->pushButton_Presence->setVisible(false);
    ui->pushButton_Actualiser->setVisible(false);
    ui->tableWidget_Gestion->setVisible(true);
    ui->tableView_Historique->setVisible(false);

    initialiserTableauGestion();

    // Vérifie l'état actuel du ventilateur en BDD
    QSqlQuery q("SELECT temperature FROM mesures_humidite_temperature WHERE zone = 1 LIMIT 1", db);
    if (q.next()) {
        int etat = q.value(0).toInt();
        ui->label_Ventilateur->setVisible(true);
        if (etat == 1) {
            ui->label_Ventilateur->setMovie(movieVentilo);
            movieVentilo->start();
        } else {
            movieVentilo->stop();
            ui->label_Ventilateur->setPixmap(
                QPixmap(":/images/Ressources/ventilo_off.png").scaled(
                    ui->label_Ventilateur->size(), Qt::KeepAspectRatio));
        }
    }

    ui->pushButton_Gestion->setStyleSheet(
        "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }");
    ui->pushButton_Affichage->setStyleSheet("");
    ui->pushButton_Historique->setStyleSheet("");
}

// Affiche widgets Historique, cache le reste
void AdminWindow::afficherWidgetsHistorique()
{
    ui->tableView_Affichage->setVisible(false);
    ui->pushButton_Luminosite->setVisible(false);
    ui->pushButton_Temperature->setVisible(false);
    ui->pushButton_Presence->setVisible(false);
    ui->pushButton_Actualiser->setVisible(false);
    ui->tableWidget_Gestion->setVisible(false);
    ui->tableView_Historique->setVisible(true);

    chargerHistorique(); // Charge les données au moment d'afficher

    ui->pushButton_Historique->setStyleSheet(
        "QPushButton { background-color: #5B7FD4; color: white; font-weight: bold; }");
    ui->pushButton_Affichage->setStyleSheet("");
    ui->pushButton_Gestion->setStyleSheet("");
}

// Boutons onglets
void AdminWindow::on_pushButton_Affichage_clicked()  { afficherWidgetsAffichage(); }
void AdminWindow::on_pushButton_Gestion_clicked()    { afficherWidgetsGestion(); }
void AdminWindow::on_pushButton_Historique_clicked() { afficherWidgetsHistorique(); }

// Boutons bas → changent la table affichée
void AdminWindow::on_pushButton_Luminosite_clicked()  { chargerTable("mesures_luminosite"); }
void AdminWindow::on_pushButton_Temperature_clicked() { chargerTable("mesures_humidite_temperature"); }
void AdminWindow::on_pushButton_Presence_clicked()    { chargerTable("mesures_presence"); }

// Bouton Actualiser avec effet flash (disparaît 500ms puis réapparaît)
void AdminWindow::on_pushButton_Actualiser_clicked()
{
    if (!model) return;

    // Cache le TableView
    ui->tableView_Affichage->setVisible(false);

    // Réaffiche après 500ms avec données rafraîchies
    QTimer::singleShot(500, this, [this]() {
        model->select();
        ui->tableView_Affichage->resizeColumnsToContents();
        ui->tableView_Affichage->setVisible(true);
    });
}

AdminWindow::~AdminWindow()
{
    if (db.isOpen()) db.close();
    delete ui;
}

//adminwindow.cpp
