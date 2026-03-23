#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "adminwindow.h"
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Masque les caractères du mot de passe
    ui->lineEdit_Password_2->setEchoMode(QLineEdit::Password);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Vérifie les identifiants et ouvre la page Admin si correct
void MainWindow::on_pushButton_Login_clicked()
{
    QString username = ui->lineEdit_Username_2->text().trimmed();
    QString password = ui->lineEdit_Password_2->text();

    if (username == "" && password == "") {
        QMessageBox::information(this, "Connexion", "Connexion Effectuée !");
        AdminWindow *adminWin = new AdminWindow();
        adminWin->show();
        this->close();
    } else {
        QMessageBox::warning(this, "Erreur", "Non valide");
        ui->lineEdit_Password_2->clear();
        ui->lineEdit_Password_2->setFocus();
    }
}

// Demande confirmation avant de quitter
void MainWindow::on_pushButton_Cancel_clicked()
{
    QMessageBox::StandardButton reponse = QMessageBox::question(
        this, "Quitter",
        "Voulez-vous vraiment quitter ?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reponse == QMessageBox::Yes) {
        QApplication::quit();
    }
}
