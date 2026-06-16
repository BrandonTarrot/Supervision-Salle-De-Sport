#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QInputDialog>
#include <QSettings>
#include "smtp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Connexion_clicked();
    void on_pushButton_Cancel_clicked();
    void on_checkBox_Show_Password_toggled(bool checked);
    void on_label_Forgot_Password_linkActivated(const QString &);
    void on_label_Backup_Code_linkActivated(const QString &);
    void mettreAJourCompteur();

private:
    Ui::MainWindow *ui;

    int tentativesEchouees = 0;
    int blocsEchoues       = 0;
    int secondesRestantes  = 0;

    QTimer *timerCompteur = nullptr;

    bool verifierIdentifiants(const QString &login, const QString &mdp);
    void demarrerCompteur(int secondes);
    void bloquerDefinitivement();
    void toutReactiver();
    void envoyerAlerteSecurite();
};

#endif
