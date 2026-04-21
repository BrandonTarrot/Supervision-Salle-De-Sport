#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <QTimer>  // rafraîchissement
#include <QStandardItemModel>
#include <QDateTime>
#include <QMovie>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class AdminWindow; }
QT_END_NAMESPACE

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

private slots:
    void on_pushButton_Affichage_clicked();   // Onglet Affichage
    void on_pushButton_Gestion_clicked();     // Onglet Gestion
    void on_pushButton_Historique_clicked();  // Onglet Historique
    void on_pushButton_Luminosite_clicked();  // Charge table luminosité
    void on_pushButton_Temperature_clicked(); // Charge table température
    void on_pushButton_Presence_clicked();    // Charge table présence
    void on_pushButton_Actualiser_clicked();  // Actualise avec effet flash
    void on_boutonGestion_clicked();          // Slot unique boutons Gestion
    void on_pushButton_Fermer_clicked();

private:
    Ui::AdminWindow *ui;
    QSqlDatabase db;
    QMovie *movieVentilo;
    void connecterBDD();
    void chargerTable(const QString &nomTable);
    void afficherWidgetsAffichage();
    void afficherWidgetsGestion();
    void afficherWidgetsHistorique();
    void initialiserTableauGestion();
    void chargerHistorique();              // Charge les modifications récentes
    void mettreAJourVentilateur(int etat); // ← ajouter avec les autres fonctions privées
    void closeEvent(QCloseEvent *event) override;
    int etatVentilateur = 0; // ← mémorise l'état
    void mettreAJourLabelTemperature();
};

#endif // ADMINWINDOW_H

//adminwindow.h
