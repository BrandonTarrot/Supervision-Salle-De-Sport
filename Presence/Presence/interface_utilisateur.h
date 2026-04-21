#ifndef INTERFACE_UTILISATEUR_H
#define INTERFACE_UTILISATEUR_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include "ui_Interface_Utilisateur.h" // Interface générée par Qt Designer

// ── Déclaration de la classe Interface_Utilisateur ──
class Interface_Utilisateur : public QDialog
{
    Q_OBJECT // Permet l'utilisation des signaux et slots Qt

public:
    // Constructeur (initialise la fenêtre utilisateur)
    explicit Interface_Utilisateur(QWidget *parent = nullptr);

    // Destructeur (libération mémoire)
    ~Interface_Utilisateur();

private slots:
    // Bouton "Mettre à jour" → recharge les données depuis la BDD
    void on_pushButton_Update_clicked();

    // Bouton "Fermer"
    void on_pushButton_Cancel_clicked();

    // Bouton "Alerter" → modifie le seuil de capacité et enregistre une alerte
    void on_pushButton_Warn_clicked();

    // Bouton "Historique" → affiche les dernières données
    void on_pushButton_History_clicked();

private:
    Ui::Dialog *ui; // Pointeur vers l'interface graphique

    int m_capaciteMax = 30; // Capacité maximale autorisée

    // Fonction interne pour afficher les données dans le tableau
    void afficherTableau();
};

#endif // INTERFACE_UTILISATEUR_H
