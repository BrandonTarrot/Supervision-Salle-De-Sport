#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>

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
    void on_pushButton_Plus_clicked();   // Augmente la température
    void on_pushButton_Moins_clicked();  // Diminue la température

private:
    Ui::AdminWindow *ui;
    QSqlDatabase db;           // Connexion BDD
    QSqlTableModel *model;     // Modèle de données pour le TableView
    void connecterBDD();                        // Initialise la connexion
    void chargerDonnees();                      // Charge les données dans le TableView
    void modifierTemperature(double variation); // Fonction commune + et -
    void afficherTemperature();                 // Affiche la valeur dans le label
};

#endif // ADMINWINDOW_H
