#include "mainwindow.h"
#include "interface_utilisateur.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);

    // ── Connexion à la base de données ──
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("supervision_salle_de_sport");
    db.setUserName("root");
    db.setPassword("");


    if (!db.open())
        QMessageBox::critical(this, "Erreur BDD",
            "Impossible de se connecter :\n" + db.lastError().text());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Login_clicked()
{
    QString id  = ui->lineEdit_User_Name->text().trimmed();
    QString mdp = ui->lineEdit_Password->text();

    if (id.isEmpty() || mdp.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    // ── Identifiants simples ──
    if (id == "admin" && mdp == "1234") {
        this->hide();
        Interface_Utilisateur *dialog = new Interface_Utilisateur(this);
        dialog->exec();
        delete dialog;
        this->show();
    } else {
        QMessageBox::critical(this, "Échec", "Identifiant ou mot de passe incorrect.");
        ui->lineEdit_Password->clear();
        ui->lineEdit_User_Name->setFocus();
    }
}

void MainWindow::on_pushButton_Cancel_clicked()
{
    this->close();
}

void MainWindow::on_checkBox_stateChanged(int state)
{
    ui->lineEdit_Password->setEchoMode(
        state == Qt::Checked ? QLineEdit::Normal : QLineEdit::Password);
}
