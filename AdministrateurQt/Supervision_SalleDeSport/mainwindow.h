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
    void on_pushButton_Login_clicked();
    void on_pushButton_Cancel_clicked();
    void on_checkBox_AfficherMDP_toggled(bool checked);
    void on_label_Password_Forgot_linkActivated(const QString &);
    void on_label_Backup_linkActivated(const QString &);
    void mettreAJourCompteur();

private:
    Ui::MainWindow *ui;
    int tentativesEchouees = 0;
    int blocsEchoues       = 0;
    int secondesRestantes  = 0;
    QTimer *timerCompteur  = nullptr;

    void demarrerCompteur(int secondes);
    void bloquerDefinitivement();
    void toutReactiver();
    void envoyerAlerteSecurite();
};

#endif // MAINWINDOW_H
