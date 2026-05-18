#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interface_utilisateur.h"
#include "mot_de_passe_oublie.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>
#include <QDate>
#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>
#include <QPushButton>
#include <QDateTime>
#include <QEvent>
#include <QMouseEvent>
#include <QMap>

// Hash SHA-256 des mots de passe autorisés
static const QMap<QString, QString> HASH_AUTORISES = {
    { "user",           "7f26736a48133dac5258c207789af276e79fee4b51aeb715428b59d66a0ddc8b" },
    { "Administrateur", "REMPLACER_PAR_LE_HASH_SHA256_DU_MOT_DE_PASSE_ADMIN" }
};

// Utilitaire : retourne le hash SHA-256
static QString sha256(const QString &texte)
{
    return QCryptographicHash::hash(
        texte.toUtf8(),
        QCryptographicHash::Sha256
    ).toHex();
}

// Génération du code backup hebdomadaire
QString MainWindow::genererCodeBackup()
{
    const QString CLE_SECRETE = "PresenceSport#2026!";

    QDate today = QDate::currentDate();
    int semaine = today.weekNumber();
    int annee   = today.year();

    QString base = QString("%1-S%2-%3-G%4")
                       .arg(CLE_SECRETE)
                       .arg(semaine, 2, 10, QChar('0'))
                       .arg(annee)
                       .arg(m_backupGeneration);

    QByteArray hash = QCryptographicHash::hash(
        base.toUtf8(),
        QCryptographicHash::Sha256
    );

    QString code = hash.toHex().left(12).toUpper();
    return QString("%1-%2-%3")
        .arg(code.left(4))
        .arg(code.mid(4, 4))
        .arg(code.right(4));
}

// Régénère un nouveau code après utilisation ou échec
void MainWindow::regenererCode()
{
    m_backupGeneration++;
    m_backupUtilise    = false;
    m_codeBackupActuel = genererCodeBackup();

    qDebug() << "=== NOUVEAU CODE BACKUP ===" << m_codeBackupActuel
             << "| Génération #" << m_backupGeneration;

    ui->pushButton_Backup->setToolTip(
        QString("Code backup — Génération #%1\n"
                "Consultez Qt Creator → Application Output")
            .arg(m_backupGeneration));
}

// Constructeur
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);

    // Timer
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    // Label "Mot de passe oublié ?" cliquable
    ui->label_5->setCursor(Qt::PointingHandCursor);
    ui->label_5->setStyleSheet(
        "QLabel { color: #5555cc; text-decoration: underline; }"
        "QLabel:hover { color: #cc0000; }"
    );
    ui->label_5->installEventFilter(this);

    // Style bouton Backup
    ui->pushButton_Backup->setText("Accès Utilisateur");
    ui->pushButton_Backup->setToolTip(
        QString("Code backup — Génération #%1\n"
                "Consultez Qt Creator → Application Output")
            .arg(m_backupGeneration));
    ui->pushButton_Backup->setStyleSheet(
        "QPushButton {"
        "   background-color: #2c3e50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover { background-color: #e74c3c; }"
        "QPushButton:pressed { background-color: #c0392b; }"
    );

    // Génération du premier code backup
    m_codeBackupActuel = genererCodeBackup();
    qDebug() << "=== CODE BACKUP INITIAL ===" << m_codeBackupActuel
             << "| Génération #" << m_backupGeneration;

    // Connexion BDD
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("172.19.6.120");
    db.setPort(3306);
    db.setDatabaseName("supervision_salle_de_sport2");
    db.setUserName("api");
    db.setPassword("api");
    db.setConnectOptions("");

    if (!db.open())
        QMessageBox::critical(this, "Erreur BDD",
            "Impossible de se connecter :\n" + db.lastError().text());
}

// EventFilter → détecte le clic sur le label
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->label_5 && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            mot_de_passe_oublie *dialog =
                new mot_de_passe_oublie(m_codeBackupActuel, this);
            dialog->exec();
            delete dialog;
        }
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

// Destructeur
MainWindow::~MainWindow()
{
    delete ui;
}

// Formate secondes → mm:ss ou hh:mm:ss
QString MainWindow::formatTemps(int secondes)
{
    int h = secondes / 3600;
    int m = (secondes % 3600) / 60;
    int s = secondes % 60;

    if (h > 0)
        return QString("%1h %2m %3s")
            .arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
    else
        return QString("%1m %2s")
            .arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

// Active ou désactive les champs
void MainWindow::activerInterface(bool actif)
{
    ui->lineEdit_User_Name->setEnabled(actif);
    ui->lineEdit_Password->setEnabled(actif);
    ui->pushButton_Login->setEnabled(actif);
}

// Démarre le blocage progressif
void MainWindow::demarrerBlocage()
{
    m_niveauBlocage++;
    m_tentativesEchouees = 0;

    switch (m_niveauBlocage)
    {
        case 1:  m_secondesRestantes = 60;   break;
        case 2:  m_secondesRestantes = 300;  break;
        default: m_secondesRestantes = 3600; break;
    }

    activerInterface(false);
    m_timer->start();

    QString duree;
    if      (m_niveauBlocage == 1) duree = "1 minute";
    else if (m_niveauBlocage == 2) duree = "5 minutes";
    else                           duree = "1 heure";

    QMessageBox::critical(this, "Accès bloqué",
        QString("Trop de tentatives échouées.\n"
                "Accès bloqué pendant %1.\n\n"
                "Utilisez Accès Utilisateur\n"
                "   ou cliquez sur 'Mot de passe oublié ?'")
            .arg(duree));

    ui->statusbar->setStyleSheet("color: black;");
    ui->statusbar->showMessage(
        QString("Accès bloqué — Réessayez dans : %1")
            .arg(formatTemps(m_secondesRestantes)));
}

// Tick toutes les secondes
void MainWindow::onTimerTick()
{
    m_secondesRestantes--;

    ui->statusbar->showMessage(
        QString("Accès bloqué — Réessayez dans : %1")
            .arg(formatTemps(m_secondesRestantes)));

    if (m_secondesRestantes <= 0)
    {
        m_timer->stop();
        activerInterface(true);
        ui->lineEdit_Password->clear();
        ui->lineEdit_User_Name->clear();
        ui->lineEdit_User_Name->setFocus();
        ui->statusbar->setStyleSheet("color: red; font-weight: bold;");
        ui->statusbar->showMessage("Accès débloqué. Vous pouvez réessayer.");
    }
}

// Gestion des échecs de connexion
void MainWindow::gererEchecConnexion()
{
    m_tentativesEchouees++;

    if (m_tentativesEchouees >= 3)
    {
        demarrerBlocage();
    }
    else
    {
        int restantes = 3 - m_tentativesEchouees;
        QMessageBox::critical(this, "Échec",
            QString("Identifiant ou mot de passe incorrect.\n"
                    "%1 tentative(s) restante(s) avant blocage.")
                .arg(restantes));

        ui->lineEdit_Password->clear();
        ui->lineEdit_User_Name->setFocus();
    }
}

// Bouton Accès Utilisateur (code backup)
void MainWindow::on_pushButton_Backup_clicked()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle("Accès Utilisateur");
    dialog.setLabelText(
        QString("Entrez le code backup administrateur\n"
                "(Génération #%1) :").arg(m_backupGeneration));
    dialog.setTextEchoMode(QLineEdit::Password);
    dialog.setFixedSize(380, 160);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString codeEntre = dialog.textValue().trimmed();

    if (codeEntre == m_codeBackupActuel && !m_backupUtilise)
    {
        m_backupUtilise = true;

        if (m_timer->isActive())
            m_timer->stop();

        m_tentativesEchouees = 0;
        m_niveauBlocage      = 0;
        m_secondesRestantes  = 0;

        activerInterface(true);
        ui->lineEdit_Password->clear();
        ui->lineEdit_User_Name->clear();
        ui->statusbar->showMessage("Code backup utilisé — Accès accordé.");

        QMessageBox::information(this, "Accès accordé",
            QString("Accès débloqué avec succès.\n\n"
                    "Ce code (Génération #%1) est maintenant invalide.\n"
                    "Un nouveau code a été généré automatiquement.\n"
                    "Consultez Qt Creator → Application Output.")
                .arg(m_backupGeneration));

        this->hide();
        Interface_Utilisateur *idialog = new Interface_Utilisateur(this);
        idialog->exec();
        delete idialog;
        this->show();

        regenererCode();
    }
    else if (codeEntre == m_codeBackupActuel && m_backupUtilise)
    {
        QMessageBox::critical(this, "Code déjà utilisé",
            QString("Le code Génération #%1 a déjà été utilisé.\n"
                    "Consultez Qt Creator → Application Output.")
                .arg(m_backupGeneration));
    }
    else
    {
        QMessageBox::critical(this, "Code incorrect",
            "Code backup invalide.\n\n"
            "Pour des raisons de sécurité, le code a été régénéré.\n"
            "Consultez Qt Creator → Application Output.");

        regenererCode();
    }
}

// Bouton Login
void MainWindow::on_pushButton_Login_clicked()
{
    QString id  = ui->lineEdit_User_Name->text().trimmed();
    QString mdp = ui->lineEdit_Password->text();

    if (id.isEmpty() || mdp.isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    // Login inconnu → échec immédiat
    if (!HASH_AUTORISES.contains(id))
    {
        gererEchecConnexion();
        return;
    }

    // Comparaison des hash SHA-256
    QString hashSaisie  = sha256(mdp);
    QString hashAttendu = HASH_AUTORISES.value(id);

    if (hashSaisie == hashAttendu)
    {
        m_tentativesEchouees = 0;
        m_niveauBlocage      = 0;
        ui->statusbar->clearMessage();

        this->hide();
        Interface_Utilisateur *dialog = new Interface_Utilisateur(this);
        dialog->exec();
        delete dialog;
        this->show();
    }
    else
    {
        gererEchecConnexion();
    }
}

// Bouton Annuler
void MainWindow::on_pushButton_Cancel_clicked()
{
    this->close();
}

// Checkbox afficher/masquer mot de passe
void MainWindow::on_checkBox_stateChanged(int state)
{
    ui->lineEdit_Password->setEchoMode(
        state == Qt::Checked ? QLineEdit::Normal : QLineEdit::Password);
}
