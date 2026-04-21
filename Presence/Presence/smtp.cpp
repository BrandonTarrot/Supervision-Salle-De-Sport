#include "smtp.h"

// ══════════════════════════════════════════════
//  Constructeur
// ══════════════════════════════════════════════
Smtp::Smtp(const QString &user, const QString &pass,
           const QString &host, int port)
    : user(user), pass(pass), host(host), port(port), state(Init)
{
    socket = new QSslSocket(this);

    connect(socket, SIGNAL(connected()),  this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()),  this, SLOT(readyRead()));
}

// ══════════════════════════════════════════════
//  Lancement de la connexion
// ══════════════════════════════════════════════
void Smtp::sendMail(const QString &from, const QString &to,
                    const QString &subject, const QString &body)
{
    this->from    = from;
    this->to      = to;
    this->subject = subject;
    this->body    = body;
    state         = Init;

    socket->connectToHostEncrypted(host, port);
}

// ══════════════════════════════════════════════
//  Connexion etablie
// ══════════════════════════════════════════════
void Smtp::connected()
{
    qDebug() << "Connecte au serveur SMTP.";
}

// ══════════════════════════════════════════════
//  Machine d'etats SMTP complete
// ══════════════════════════════════════════════
void Smtp::readyRead()
{
    QString response = socket->readAll();
    qDebug() << "Serveur :" << response.trimmed();

    int code = response.left(3).toInt();

    switch (state) {

        // ── Banniere (220) -> EHLO ──
        case Init:
            if (code == 220) {
                socket->write("EHLO localhost\r\n");
                state = Ehlo;
            } else {
                emit status("Init failed: " + response);
            }
            break;

        // ── EHLO OK (250) -> AUTH LOGIN ──
        case Ehlo:
            if (code == 250) {
                socket->write("AUTH LOGIN\r\n");
                state = Auth;
            } else {
                emit status("Ehlo failed: " + response);
            }
            break;

        // ── AUTH LOGIN (334) -> identifiant ──
        case Auth:
            if (code == 334) {
                socket->write(user.toUtf8().toBase64() + "\r\n");
                state = User;
            } else {
                emit status("Auth failed: " + response);
            }
            break;

        // ── Identifiant envoye (334) -> mot de passe ──
        case User:
            if (code == 334) {
                socket->write(pass.toUtf8().toBase64() + "\r\n");
                state = Pass;
            } else {
                emit status("User failed: " + response);
            }
            break;

        // ── Authentification reussie (235) -> MAIL FROM ──
        case Pass:
            if (code == 235) {
                socket->write(QString("MAIL FROM:<%1>\r\n").arg(from).toUtf8());
                state = Mail;
            } else {
                emit status("Authentication failed: " + response);
            }
            break;

        // ── MAIL FROM OK (250) -> RCPT TO ──
        case Mail:
            if (code == 250) {
                socket->write(QString("RCPT TO:<%1>\r\n").arg(to).toUtf8());
                state = Rcpt;
            } else {
                emit status("Mail failed: " + response);
            }
            break;

        // ── RCPT TO OK (250) -> DATA ──
        case Rcpt:
            if (code == 250) {
                socket->write("DATA\r\n");
                state = Data;
            } else {
                emit status("Rcpt failed: " + response);
            }
            break;

        // ── DATA OK (354) -> Corps du message ──
        case Data:
            if (code == 354) {
                QString message =
                    QString("From: %1\r\n"
                            "To: %2\r\n"
                            "Subject: %3\r\n"
                            "MIME-Version: 1.0\r\n"
                            "Content-Type: text/plain; charset=UTF-8\r\n"
                            "\r\n"
                            "%4\r\n"
                            ".\r\n")
                        .arg(from)
                        .arg(to)
                        .arg(subject)
                        .arg(body);

                socket->write(message.toUtf8());
                state = Body;
            } else {
                emit status("Data failed: " + response);
            }
            break;

        // ── Message envoye (250) -> QUIT ──
        case Body:
            if (code == 250) {
                socket->write("QUIT\r\n");
                state = Quit;
            } else {
                emit status("Body failed: " + response);
            }
            break;

        // ── QUIT OK (221) -> fermeture ──
        case Quit:
            if (code == 221) {
                socket->disconnectFromHost();
                emit status("Message sent");
                state = Close;
            } else {
                emit status("Quit failed: " + response);
            }
            break;

        default:
            break;
    }
}
