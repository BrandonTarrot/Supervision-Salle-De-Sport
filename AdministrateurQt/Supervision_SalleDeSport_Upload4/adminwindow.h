#ifndef ADMINWINDOW_H     //Eviter Répétitions
#define ADMINWINDOW_H

#include <QMainWindow>           //Fondations
#include <QtSql/QSqlDatabase>    //Base de données
#include <QtSql/QSqlTableModel>  //Lecture BDD
#include <QTimer>                //Loop
#include <QStandardItemModel>    //Charger les tableaux
#include <QDateTime>             //Date du jour
#include <QMovie>                //GIF
#include <QCloseEvent>           //Fermer Fenêtre

QT_BEGIN_NAMESPACE
namespace Ui { class AdminWindow; } //Heritage
QT_END_NAMESPACE

class AdminWindow : public QMainWindow
{
    Q_OBJECT //

public:
    AdminWindow(QWidget *parent = nullptr); //Initialisation
    ~AdminWindow(); //Destructeur

private slots: //Widgets Page Administrateur
    void on_pushButton_Display_clicked();
    void on_pushButton_Management_clicked();
    void on_pushButton_History_clicked();
    void on_pushButton_Luminosite_clicked();
    void on_pushButton_Temperature_clicked();
    void on_pushButton_Presence_clicked();
    void on_pushButton_Refresh_clicked();
    void on_pushButton_Close_clicked();
    void on_boutonGestion_clicked();
    void reafficherTableau();
    void mettreAJourEtatCapteurs();

private:
    Ui::AdminWindow *ui;

    QSqlDatabase  db;
    QMovie       *movieVentilo    = nullptr;
    int           etatVentilateur = 0;
    QTimer       *timerEtatCapteurs;

    void connecterBDD();
    void chargerTable(const QString &nomTable);
    void chargerHistorique();
    void initialiserTableauGestion();
    void mettreAJourLabelTemperature();
    void mettreAJourVentilateur(int etat);
    void afficherWidgetsAffichage();
    void afficherWidgetsGestion();
    void afficherWidgetsHistorique();
    void closeEvent(QCloseEvent *event) override;
};

#endif
