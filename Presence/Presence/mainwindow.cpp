#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interface_utilisateur.h"
#include "mot_de_passe_oublie.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QLineEdit>
#include <QDate>
#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>
#include <QEvent>
#include <QMouseEvent>
#include <QMap>

// Liste des utilisateurs autorisés et leurs mots de passe en hash SHA-256
static const QMap<QString, QString> HASH_AUTORISES = {
    { "user",           "7f26736a48133dac5258c207789af276e79fee4b51aeb715428b59d66a0ddc8b" },
    { "Administrateur", "REMPLACER_PAR_LE_HASH_SHA256_DU_MOT_DE_PASSE_ADMIN"               }
};

// Convertit un texte en hash SHA-256
static QString sha256(const QString &texte)
{
    return QCryptographicHash::hash(texte.toUtf8(), QCryptographicHash::Sha256).toHex();
}

// Génère un code de secours unique basé sur la date actuelle
QString MainWindow::genererCodeBackup()
{
    QDate today = QDate::currentDate();

    QString base = QString("PresenceSport#2026!-S%1-%2-G%3")
                       .arg(today.weekNumber(), 2, 10, QChar('0'))
                       .arg(today.year())
                       .arg(m_backupGeneration);

    QString code = QCryptographicHash::hash(base.toUtf8(), QCryptographicHash::Sha256)
                       .toHex().left(12).toUpper();

    return QString("%1-%2-%3").arg(code.left(4)).arg(code.mid(4, 4)).arg(code.right(4));
}

// Recrée un nouveau code de secours après utilisation ou échecif (!HASH_AUTORISES.contains(id))
void MainWindow::regenererCode()
{
    m_backupGeneration++;
    m_backupUtilise    = false;
    m_codeBackupBrut   = genererCodeBackup();       // code lisible envoyé par mail
    m_codeBackupActuel = sha256(m_codeBackupBrut);  // hash stocké en mémoire

    qDebug() << "=== NOUVEAU CODE BACKUP (Hash) ===" << m_codeBackupActuel
             << "| Génération #" << m_backupGeneration;

    ui->pushButton_Backup->setToolTip(
        QString("Code backup — Génération #%1\nConsultez Qt Creator → Application Output")
            .arg(m_backupGeneration));
}

// Constructeur : initialisation de la fenêtre, du timer, du style et de la BDD
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Mot de passe masqué par défaut
    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);

    // Timer pour le décompte du blocage (1 tick par seconde)
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    // Label "Mot de passe oublié ?" rendu cliquable
    ui->label_5->setCursor(Qt::PointingHandCursor);
    ui->label_5->setStyleSheet(
        "QLabel { color: #5555cc; text-decoration: underline; }"
        "QLabel:hover { color: #cc0000; }");
    ui->label_5->installEventFilter(this);

    // Style du bouton d'accès de secours
    ui->pushButton_Backup->setText("Accès Utilisateur");
    ui->pushButton_Backup->setStyleSheet(
        "QPushButton         { background-color: #2c3e50; color: white; border: none;"
        "                      border-radius: 6px; font-size: 12px; font-weight: bold; padding: 6px 12px; }"
        "QPushButton:hover   { background-color: #e74c3c; }"
        "QPushButton:pressed { background-color: #c0392b; }");

    // Génération du premier code de secours au lancement
    m_codeBackupBrut   = genererCodeBackup();
    m_codeBackupActuel = sha256(m_codeBackupBrut);
    qDebug() << "=== CODE BACKUP INITIAL (Hash) ===" << m_codeBackupActuel
             << "| Génération #" << m_backupGeneration;

    // Connexion à la base de données MySQL
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

// Intercepte le clic sur le label "Mot de passe oublié ?"
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->label_5 && event->type() == QEvent::MouseButtonPress
        && static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton)
    {
        // Ouvre la fenêtre de récupération avec le code lisible
        mot_de_passe_oublie *dialog = new mot_de_passe_oublie(m_codeBackupBrut, this);
        dialog->exec();
        delete dialog;
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

// Destructeur : libération de la mémoire
MainWindow::~MainWindow()
{
    delete ui;
}

// Formate des secondes en texte lisible (ex : 1m 30s)
QString MainWindow::formatTemps(int secondes)
{
    int h   = secondes / 3600;
    int m   = (secondes % 3600) / 60;
    int sec = secondes % 60;

    if (h > 0)
        return QString("%1h %2m %3s").arg(h).arg(m, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));

    return QString("%1m %2s").arg(m, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
}

// Active ou désactive les champs et le bouton de connexion
void MainWindow::activerInterface(bool actif)
{
    ui->lineEdit_User_Name->setEnabled(actif);
    ui->lineEdit_Password->setEnabled(actif);
    ui->pushButton_Login->setEnabled(actif);
}

// Bloque l'accès temporairement (1min → 5min → 1h)
void MainWindow::demarrerBlocage()
{
    m_niveauBlocage++;
    m_tentativesEchouees = 0;

    // Durée du blocage selon le niveau
    if (m_niveauBlocage == 1)      m_secondesRestantes = 60;
    else if (m_niveauBlocage == 2) m_secondesRestantes = 300;
    else                           m_secondesRestantes = 3600;

    activerInterface(false);
    m_timer->start();

    QString duree;
    if (m_niveauBlocage == 1)      duree = "1 minute";
    else if (m_niveauBlocage == 2) duree = "5 minutes";
    else                           duree = "1 heure";

    QMessageBox::critical(this, "Accès bloqué",
        QString("Trop de tentatives échouées.\n"
                "Accès bloqué pendant %1.\n\n"
                "Utilisez Accès Utilisateur\n"
                "   ou cliquez sur 'Mot de passe oublié ?'").arg(duree));

    ui->statusbar->setStyleSheet("color: black;");
    ui->statusbar->showMessage("Accès bloqué — Réessayez dans : " + formatTemps(m_secondesRestantes));
}

// S'exécute chaque seconde et débloque quand le compteur atteint 0
void MainWindow::onTimerTick()
{
    m_secondesRestantes--;
    ui->statusbar->showMessage("Accès bloqué — Réessayez dans : " + formatTemps(m_secondesRestantes));

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

// Comptabilise les échecs et bloque après 3 tentatives
void MainWindow::gererEchecConnexion()
{
    m_tentativesEchouees++;

    if (m_tentativesEchouees >= 3)
    {
        demarrerBlocage();
        return;
    }

    QMessageBox::critical(this, "Échec",
        QString("Identifiant ou mot de passe incorrect.\n"
                "%1 tentative(s) restante(s) avant blocage.")
            .arg(3 - m_tentativesEchouees));

    ui->lineEdit_Password->clear();
    ui->lineEdit_User_Name->setFocus();
}

// Bouton "Accès Utilisateur" : vérifie le code de secours
void MainWindow::on_pushButton_Backup_clicked()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle("Accès Utilisateur");
    dialog.setLabelText(
        QString("Entrez le code backup administrateur\n(Génération #%1) :")
            .arg(m_backupGeneration));
    dialog.setTextEchoMode(QLineEdit::Password);
    dialog.setFixedSize(380, 160);

    if (dialog.exec() != QDialog::Accepted)
        return;

    // On hache la saisie pour comparer avec le hash stocké
    QString codeEntre = sha256(dialog.textValue().trimmed());

    if (codeEntre == m_codeBackupActuel && !m_backupUtilise)
    {
        // Code correct → accès accordé
        m_backupUtilise      = true;
        m_tentativesEchouees = 0;
        m_niveauBlocage      = 0;
        m_secondesRestantes  = 0;

        if (m_timer->isActive())
            m_timer->stop();

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

        regenererCode(); // nouveau code généré après utilisation
    }
    else if (codeEntre == m_codeBackupActuel && m_backupUtilise)
    {
        // Code déjà utilisé
        QMessageBox::critical(this, "Code déjà utilisé",
            QString("Le code Génération #%1 a déjà été utilisé.\n"
                    "Consultez Qt Creator → Application Output.")
                .arg(m_backupGeneration));
    }
    else
    {
        // Code incorrect → nouveau code généré par sécurité
        QMessageBox::critical(this, "Code incorrect",
            "Code backup invalide.\n\n"
            "Pour des raisons de sécurité, le code a été régénéré.\n"
            "Consultez Qt Creator → Application Output.");
        regenererCode();
    }
}

// Bouton "Connecter" : vérifie l'identifiant et le mot de passe
void MainWindow::on_pushButton_Login_clicked()
{
    QString id  = ui->lineEdit_User_Name->text().trimmed();
    QString mdp = ui->lineEdit_Password->text();

    // Vérification que les champs sont remplis
    if (id.isEmpty() || mdp.isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    // Identifiant inconnu → échec
    if (!HASH_AUTORISES.contains(id))
    {
        gererEchecConnexion();
        return;
    }

    // Mot de passe correct → ouverture de la fenêtre principale
    if (sha256(mdp) == HASH_AUTORISES.value(id))
    {
        m_tentativesEchouees = 0;
        m_niveauBlocage      = 0;
        ui->statusbar->clearMessage();
        ui->lineEdit_User_Name->clear(); // vide les champs après connexion
        ui->lineEdit_Password->clear();

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

// Bouton "Annuler" : ferme l'application
void MainWindow::on_pushButton_Cancel_clicked()
{
    this->close();
}

// Case à cocher : affiche ou masque le mot de passe
void MainWindow::on_checkBox_stateChanged(int state)
{
    if (state == Qt::Checked)
        ui->lineEdit_Password->setEchoMode(QLineEdit::Normal);
    else
        ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
}
