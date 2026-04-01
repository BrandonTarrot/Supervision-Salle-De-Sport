#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "adminwindow.h"
#include <QMessageBox>
#include <QApplication>
#include "forgotpassword.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Masque les caractères du mot de passe au démarrage
    ui->lineEdit_Password_2->setEchoMode(QLineEdit::Password);
    ui->pushButton_Login->setCursor(Qt::PointingHandCursor);
    ui->pushButton_Cancel->setCursor(Qt::PointingHandCursor);
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

    if (reponse == QMessageBox::Yes)
        QApplication::quit();
}

// Bascule entre mot de passe visible et masqué
void MainWindow::on_checkBox_AfficherMDP_toggled(bool checked)
{
    ui->lineEdit_Password_2->setEchoMode(
        checked ? QLineEdit::Normal : QLineEdit::Password);
}

//mainwindow.cpp


void MainWindow::on_label_Password_Forgot_linkActivated(const QString &/*link*/)
{
    forgotpassword *dlg = new forgotpassword(this);
    dlg->exec();
}
