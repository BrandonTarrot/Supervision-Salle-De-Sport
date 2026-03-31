#include "interface_utilisateur.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>

Interface_Utilisateur::Interface_Utilisateur(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)   // ← Ui::Dialog
{
    ui->setupUi(this);
    rafraichirPresence();
}

Interface_Utilisateur::~Interface_Utilisateur()
{
    delete ui;
}

// Lit la dernière ligne de mesures_presence et affiche le compte
void Interface_Utilisateur::rafraichirPresence()
{
    QSqlQuery query("SELECT presence FROM mesures_presence ORDER BY id DESC LIMIT 1");
    if (query.next())
        ui->label_3->setText(QString("Présentes : %1").arg(query.value(0).toInt()));
    else
        ui->label_3->setText("Présentes : --");
}

// Bouton "Mettre à jour" → relit la BDD
void Interface_Utilisateur::on_pushButton_Update_clicked()
{
    rafraichirPresence();

    QSqlQuery query("SELECT presence FROM mesures_presence ORDER BY id DESC LIMIT 1");
    if (query.next() && query.value(0).toInt() >= m_capaciteMax)
        QMessageBox::warning(this, "Capacité atteinte",
            QString("⚠ %1 personnes sur %2 autorisées !")
                .arg(query.value(0).toInt()).arg(m_capaciteMax));
}

// Bouton "Fermer"
void Interface_Utilisateur::on_pushButton_Cancel_clicked()
{
    accept();
}

// Bouton "Alerter" → modifie le seuil et insère une entrée d'alerte
void Interface_Utilisateur::on_pushButton_Warn_clicked()
{
    bool ok;
    int seuil = QInputDialog::getInt(this, "Seuil d'alerte",
                    "Capacité maximale :", m_capaciteMax, 1, 999, 1, &ok);
    if (!ok) return;

    m_capaciteMax = seuil;

    QSqlQuery query;
    query.prepare("INSERT INTO mesures_presence (presence, date, zone) "
                  "VALUES (:p, :d, :z)");
    query.bindValue(":p", -1);
    query.bindValue(":d", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":z", "Alerte");

    if (!query.exec())
        QMessageBox::critical(this, "Erreur BDD", query.lastError().text());
    else
        QMessageBox::information(this, "OK",
            QString("Seuil : %1 personnes. Alerte enregistrée.").arg(m_capaciteMax));
}

// Bouton "Historique" → affiche les 10 dernières mesures
void Interface_Utilisateur::on_pushButton_History_clicked()
{
    QSqlQuery query("SELECT presence, date, zone "
                    "FROM mesures_presence ORDER BY id DESC LIMIT 10");

    QString texte = "── 10 dernières mesures ──\n\n";
    bool vide = true;

    while (query.next()) {
        vide = false;
        texte += QString("Nb: %1  |  %2  |  Zone: %3\n")
                     .arg(query.value(0).toInt())
                     .arg(query.value(1).toString())
                     .arg(query.value(2).toString());
    }

    if (vide) texte += "(Aucune donnée en base)";
    QMessageBox::information(this, "Historique", texte);
}
