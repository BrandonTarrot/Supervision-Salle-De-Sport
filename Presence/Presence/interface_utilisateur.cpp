#include "interface_utilisateur.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QHeaderView>

// ── Constructeur de la fenêtre utilisateur ──
Interface_Utilisateur::Interface_Utilisateur(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // Affiche les données dès l'ouverture de la fenêtre
    afficherTableau();
}

// ── Destructeur ──
Interface_Utilisateur::~Interface_Utilisateur()
{
    delete ui; // Libération mémoire
}

// ── Fonction pour afficher toutes les données dans le tableau ──
void Interface_Utilisateur::afficherTableau()
{
    // Requête SQL pour récupérer les données triées par ID décroissant
    QSqlQuery query("SELECT id, presence, date, zone FROM mesures_presence ORDER BY id DESC");

    // Configuration du tableau
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"ID", "Présence", "Date", "Zone"});
    ui->tableWidget->setRowCount(0); // Réinitialise le tableau

    // Parcours des résultats de la requête
    while (query.next()) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row); // Ajoute une ligne

        // Remplissage des colonnes
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("presence").toString()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("date").toString()));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("zone").toString()));
    }

    // Ajuste automatiquement la taille des colonnes
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

// ── Bouton "Mettre à jour" ──
void Interface_Utilisateur::on_pushButton_Update_clicked()
{
    afficherTableau(); // Recharge les données depuis la BDD

    // Vérifie la dernière valeur de présence
    QSqlQuery query("SELECT presence FROM mesures_presence ORDER BY id DESC LIMIT 1");
    if (query.next() && query.value(0).toInt() >= m_capaciteMax)
        QMessageBox::warning(this, "Capacité atteinte",
            QString("⚠ %1 personnes sur %2 autorisées !")
                .arg(query.value(0).toInt()).arg(m_capaciteMax));
}

// ── Bouton "Fermer" ──
void Interface_Utilisateur::on_pushButton_Cancel_clicked()
{
    accept(); // Ferme la fenêtre (équivalent à "OK")
}

// ── Bouton "Alerter" ──
void Interface_Utilisateur::on_pushButton_Warn_clicked()
{
    bool ok;

    // Demande à l'utilisateur de définir un seuil maximum
    int seuil = QInputDialog::getInt(this, "Seuil d'alerte",
                    "Capacité maximale :", m_capaciteMax, 1, 999, 1, &ok);
    if (!ok) return; // Annulation

    m_capaciteMax = seuil; // Mise à jour du seuil

    // Préparation d'une requête pour insérer une alerte
    QSqlQuery query;
    query.prepare("INSERT INTO mesures_presence (presence, date, zone) "
                  "VALUES (:p, :d, :z)");

    // Valeurs associées
    query.bindValue(":p", -1); // -1 = valeur spéciale pour alerte
    query.bindValue(":d", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":z", "Alerte");

    // Exécution de la requête
    if (!query.exec())
        QMessageBox::critical(this, "Erreur BDD", query.lastError().text());
    else
        QMessageBox::information(this, "OK",
            QString("Seuil : %1 personnes. Alerte enregistrée.").arg(m_capaciteMax));
}

// ── Bouton "Historique" ──
void Interface_Utilisateur::on_pushButton_History_clicked()
{
    // Récupère les 10 dernières mesures
    QSqlQuery query("SELECT presence, date, zone "
                    "FROM mesures_presence ORDER BY id DESC LIMIT 10");

    QString texte = "── 10 dernières mesures ──\n\n";
    bool vide = true;

    // Parcours des résultats
    while (query.next()) {
        vide = false;

        texte += QString("Nb: %1  |  %2  |  Zone: %3\n")
                     .arg(query.value(0).toInt())
                     .arg(query.value(1).toString())
                     .arg(query.value(2).toString());
    }

    // Si aucune donnée
    if (vide) texte += "(Aucune donnée en base)";

    // Affichage dans une boîte de dialogue
    QMessageBox::information(this, "Historique", texte);
}
