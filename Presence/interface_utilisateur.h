#ifndef INTERFACE_UTILISATEUR_H
#define INTERFACE_UTILISATEUR_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>

// Le fichier .ui s'appelle Interface_Utilisateur.ui
// → Qt génère : ui_Interface_Utilisateur.h
// → L'objet racine dans Qt Designer s'appelle "Dialog"
// → Qt génère : namespace Ui { class Dialog { ... }; }
#include "ui_Interface_Utilisateur.h"

class Interface_Utilisateur : public QDialog
{
    Q_OBJECT

public:
    explicit Interface_Utilisateur(QWidget *parent = nullptr);
    ~Interface_Utilisateur();

private slots:
    void on_pushButton_Update_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Warn_clicked();
    void on_pushButton_History_clicked();

private:
    Ui::Dialog *ui;      // ← Ui::Dialog car l'objet racine Qt Designer = "Dialog"

    int m_capaciteMax = 30;

    void rafraichirPresence();
};

#endif // INTERFACE_UTILISATEUR_H
