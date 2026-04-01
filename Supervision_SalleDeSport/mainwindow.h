#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_pushButton_Login_clicked();             // Vérifie les identifiants
    void on_pushButton_Cancel_clicked();            // Demande confirmation puis quitte
    void on_checkBox_AfficherMDP_toggled(bool checked); // Affiche/masque le mot de passe

    void on_label_4_linkActivated(const QString &link);

    void on_label_Password_Forgot_linkActivated(const QString &link);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

//mainwindow.h
