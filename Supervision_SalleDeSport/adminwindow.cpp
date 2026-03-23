#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QDebug>

AdminWindow::AdminWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    setWindowTitle("Page Administrateur - Supervision Salle De Sport");

    // Chargement du logo
    ui->label->setPixmap(QPixmap(":/images/Ressources/AdminLogo.jpg"));

    // Connexion BDD puis chargement des données
    connecterBDD();
    chargerDonnees();
}

// Connexion à MySQL via WAMP
void AdminWindow::connecterBDD()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("bdd_test");
    db.setUserName("root");
    db.setPassword(""); // Vide par défaut sur WAMP

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur BDD",
            "Connexion échouée : " + db.lastError().text());
        qDebug() << "Erreur :" << db.lastError().text();
    } else {
        qDebug() << "Connexion BDD réussie !";
    }
}

// Charge la table température dans le TableView
void AdminWindow::chargerDonnees()
{
    if (!db.isOpen()) return;

    model = new QSqlTableModel(this, db);
    model->setTable("température");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    // Colonnes en français
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Valeur");
    model->setHeaderData(2, Qt::Horizontal, "Unité");
    model->setHeaderData(3, Qt::Horizontal, "Date Mesure");
    model->setHeaderData(4, Qt::Horizontal, "Localisation");

    model->select();

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setColumnHidden(0, true); // Cache la colonne ID

    // Affiche la valeur dans le label au démarrage
    afficherTemperature();
}

// Affiche la dernière valeur de température dans label_Valeur
void AdminWindow::afficherTemperature()
{
    QSqlQuery query("SELECT valeur, unite FROM température ORDER BY id DESC LIMIT 1", db);
    if (query.next()) {
        QString valeur = query.value(0).toString();
        QString unite  = query.value(1).toString();
        ui->label_Valeur->setText(valeur + " " + unite);
    }
}

// Fonction commune pour + et - (variation = +1.0 ou -1.0)
void AdminWindow::modifierTemperature(double variation)
{
    QModelIndex index = ui->tableView->currentIndex();

    if (!index.isValid()) {
        QMessageBox::warning(this, "Attention", "Sélectionnez une ligne !");
        return;
    }

    // Récupère et modifie la valeur colonne 1
    double valeurActuelle = model->data(model->index(index.row(), 1)).toDouble();
    double nouvelleValeur = valeurActuelle + variation;

    model->setData(model->index(index.row(), 1), nouvelleValeur);

    if (model->submitAll()) {
        model->select();
        afficherTemperature(); // Met à jour le label
        qDebug() << "Température mise à jour :" << nouvelleValeur;
    } else {
        QMessageBox::critical(this, "Erreur",
            "Échec : " + model->lastError().text());
        model->revertAll();
    }
}

// Bouton + → augmente de 1°C
void AdminWindow::on_pushButton_Plus_clicked()
{
    modifierTemperature(+1.0);
}

// Bouton - → diminue de 1°C
void AdminWindow::on_pushButton_Moins_clicked()
{
    modifierTemperature(-1.0);
}

AdminWindow::~AdminWindow()
{
    if (db.isOpen()) db.close(); // Ferme la connexion proprement
    delete ui;
}
