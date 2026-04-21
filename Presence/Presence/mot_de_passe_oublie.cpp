#include "mot_de_passe_oublie.h"
#include "smtp.h"

#include <QMessageBox>
#include <QDate>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>

// ══════════════════════════════════════════════
//  Constructeur
// ══════════════════════════════════════════════
mot_de_passe_oublie::mot_de_passe_oublie(const QString &codeBackup, QWidget *parent)
    : QDialog(parent), m_codeBackup(codeBackup)
{
    setWindowTitle("Recuperation Admin");
    setFixedSize(400, 180);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(20, 20, 20, 20);

    // ── Titre ──
    QLabel *labelTitre = new QLabel("Recuperation d'acces administrateur", this);
    labelTitre->setAlignment(Qt::AlignCenter);
    labelTitre->setStyleSheet(
        "QLabel {"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
    );
    layout->addWidget(labelTitre);

    // ── Email destinataire (non modifiable) ──
    QLabel *labelMail = new QLabel(
        "Le code sera envoye a :", this);
    labelMail->setAlignment(Qt::AlignLeft);
    layout->addWidget(labelMail);

    QLabel *labelAdresse = new QLabel(
        "supervision.salledesport@gmail.com", this);
    labelAdresse->setAlignment(Qt::AlignCenter);
    labelAdresse->setStyleSheet(
        "QLabel {"
        "   background-color: #f0f0f0;"
        "   color: #2c3e50;"
        "   border: 1px solid #cccccc;"
        "   border-radius: 4px;"
        "   padding: 5px;"
        "   font-weight: bold;"
        "}"
    );
    layout->addWidget(labelAdresse);

    // ── Bouton Envoyer ──
    m_btnConfirmer = new QPushButton("Envoyer le code par mail", this);
    m_btnConfirmer->setFixedHeight(36);
    m_btnConfirmer->setStyleSheet(
        "QPushButton {"
        "   background-color: #2c3e50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #3498db; }"
        "QPushButton:pressed { background-color: #2980b9; }"
        "QPushButton:disabled { background-color: #95a5a6; }"
    );
    layout->addWidget(m_btnConfirmer);

    setLayout(layout);

    // ── Connexion du bouton ──
    connect(m_btnConfirmer, &QPushButton::clicked,
            this, &mot_de_passe_oublie::on_confirmer);
}

// ══════════════════════════════════════════════
//  Destructeur
// ══════════════════════════════════════════════
mot_de_passe_oublie::~mot_de_passe_oublie() {}

// ══════════════════════════════════════════════
//  Bouton Confirmer -> envoi du mail
// ══════════════════════════════════════════════
void mot_de_passe_oublie::on_confirmer()
{
    m_btnConfirmer->setEnabled(false);
    m_btnConfirmer->setText("Envoi en cours...");

    // ── Corps du mail ──
    QString corps = QString(
        "Bonjour,\n\n"
        "Suite a votre demande de recuperation d'acces,\n"
        "voici votre code backup administrateur :\n\n"
        "    ->  %1\n\n"
        "Ce code est a usage unique.\n"
        "Un nouveau code sera générer apres utilisation.\n\n"
        "Date de la demande : %2\n\n"
        "-- Systeme de supervision Salle de Sport"
    ).arg(m_codeBackup)
     .arg(QDate::currentDate().toString("dd/MM/yyyy"));

    // ── Création et envoi via SMTP ──
    Smtp *smtp = new Smtp(
        "supervision.salledesport@gmail.com",
        "znufrpddovfwcven",
        "smtp.gmail.com",
        465
    );

    // ── Connexion du signal status ──
    connect(smtp, &Smtp::status, this, [this, smtp](const QString &msg) {

        qDebug() << "SMTP status :" << msg;

        if (msg == "Message sent") {
            QMessageBox::information(this, "Mail envoye",
                "Le code backup a ete envoye avec succes a :\n"
                "supervision.salledesport@gmail.com\n\n"
                "Verifiez votre boite mail.");
            smtp->deleteLater();
            this->accept();
        } else {
            QMessageBox::warning(this, "Erreur d'envoi",
                "L'envoi a echoue.\n\n"
                "Verifiez :\n"
                "- La connexion internet\n"
                "- Les DLLs OpenSSL 32 bits dans le dossier debug\n"
                "- Qt Creator -> Application Output\n\n"
                "Detail : " + msg);
            smtp->deleteLater();
            m_btnConfirmer->setEnabled(true);
            m_btnConfirmer->setText("Envoyer le code par mail");
        }
    });

    // ── Envoi ──
    smtp->sendMail(
        "supervision.salledesport@gmail.com",
        "supervision.salledesport@gmail.com",
        "Code Backup - Supervision Salle de Sport",
        corps
    );
}
