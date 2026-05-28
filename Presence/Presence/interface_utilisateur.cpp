#include "interface_utilisateur.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QHeaderView>

// Constructeur : affiche les données dès l'ouverture
Interface_Utilisateur::Interface_Utilisateur(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // Affiche les données dès l'ouverture de la fenêtre
    afficherTableau();
}

// Destructeur : libération de la mémoire
Interface_Utilisateur::~Interface_Utilisateur()
{
    delete ui;
}

// Affiche toutes les données dans le tableau
void Interface_Utilisateur::afficherTableau()
{
    QSqlQuery query;

    // Vérification que la requête fonctionne
    if (!query.exec("SELECT id, presence, date, id_zone FROM mesures_presence ORDER BY id DESC"))
    {
        QMessageBox::critical(this, "Erreur SQL", query.lastError().text());
        return;
    }

    // Configuration du tableau
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"ID", "Présence", "Date", "Zone"});
    ui->tableWidget->setRowCount(0); // Réinitialise le tableau

    // Parcours des résultats de la requête
    while (query.next())
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row); // Ajoute une ligne

        // Conversion de la date en format français (jj/mm/aa hh:mm:ss)
        QString dateBrute = query.value("date").toString();
        QDateTime dateHeure = QDateTime::fromString(dateBrute, Qt::ISODate);
        QString dateFormatee = dateHeure.toString("dd/MM/yy hh:mm:ss");

        // Remplissage des colonnes
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("id").toString()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("presence").toString()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(dateFormatee));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("id_zone").toString()));
    }

    // Ajuste automatiquement la taille des colonnes
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

// Bouton "Mettre à jour" : recharge les données depuis la BDD
void Interface_Utilisateur::on_pushButton_Update_clicked()
{
    afficherTableau();

    // Vérifie si la capacité maximale est atteinte
    QSqlQuery query;
    if (!query.exec("SELECT presence FROM mesures_presence ORDER BY id DESC LIMIT 1"))
    {
        QMessageBox::critical(this, "Erreur SQL", query.lastError().text());
        return;
    }

    if (query.next() && query.value(0).toInt() >= m_capaciteMax)
        QMessageBox::warning(this, "Capacité atteinte",
            QString("%1 personnes sur %2 autorisées !")
                .arg(query.value(0).toInt()).arg(m_capaciteMax));
}

// Bouton "Fermer" : ferme la fenêtre
void Interface_Utilisateur::on_pushButton_Cancel_clicked()
{
    accept();
}

// Bouton "Alerter" : modifie le seuil et enregistre une alerte en BDD
void Interface_Utilisateur::on_pushButton_Warn_clicked()
{
    bool ok;

    // Demande à l'utilisateur de définir un seuil maximum
    int seuil = QInputDialog::getInt(this, "Seuil d'alerte",
                    "Capacité maximale :", m_capaciteMax, 1, 999, 1, &ok);
    if (!ok) return; // Annulation

    m_capaciteMax = seuil; // Mise à jour du seuil

    // Insertion d'une alerte dans la BDD
    QSqlQuery query;
    query.prepare("INSERT INTO mesures_presence (presence, date, id_zone) "
                  "VALUES (:p, :d, :z)");
    query.bindValue(":p", -1); // -1 = valeur spéciale pour alerte
    query.bindValue(":d", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":z", "Alerte");

    if (!query.exec())
        QMessageBox::critical(this, "Erreur BDD", query.lastError().text());
    else
        QMessageBox::information(this, "OK",
            QString("Seuil : %1 personnes. Alerte enregistrée.").arg(m_capaciteMax));
}

// Bouton "Historique" : affiche les 10 dernières mesures
void Interface_Utilisateur::on_pushButton_History_clicked()
{
    QSqlQuery query;

    // Récupère les 10 dernières mesures
    if (!query.exec("SELECT presence, date, id_zone FROM mesures_presence ORDER BY id DESC LIMIT 10"))
    {
        QMessageBox::critical(this, "Erreur SQL", query.lastError().text());
        return;
    }

    QString texte = "── 10 dernières mesures ──\n\n";
    bool vide = true;

    // Parcours des résultats
    while (query.next())
    {
        vide = false;

        // Conversion de la date en format français
        QString dateBrute = query.value(1).toString();
        QDateTime dateHeure = QDateTime::fromString(dateBrute, Qt::ISODate);
        QString dateFormatee = dateHeure.toString("dd/MM/yy hh:mm:ss");

        texte += QString("Nb: %1  |  %2  |  Zone: %3\n")
                     .arg(query.value(0).toInt())
                     .arg(dateFormatee)
                     .arg(query.value(2).toString());
    }

    // Si aucune donnée trouvée
    if (vide) texte += "(Aucune donnée en base)";

    // Affichage dans une boîte de dialogue
    QMessageBox::information(this, "Historique", texte);
}
