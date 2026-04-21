//smtp.cpp

#include "smtp.h"
#include <QByteArray>

Smtp::Smtp(const QString &user, const QString &pass, const QString &host,
           const QString &from, const QString &to,
           const QString &subject, const QString &body,
           int port, int timeout)
    : user(user), pass(pass), from(from), to(to),
      subject(subject), body(body), timeout(timeout), state(0)
{
    socket = new QSslSocket(this);
    connect(socket, &QSslSocket::connected,  this, &Smtp::connected);
    connect(socket, &QSslSocket::readyRead,  this, &Smtp::readyRead);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error),
            this, &Smtp::errorReceived);
    socket->connectToHostEncrypted(host, port);
    if (!socket->waitForEncrypted(timeout))
        emit mailSent(false, "Connexion SSL échouée : " + socket->errorString());
}

void Smtp::encode(const QString &str, QString &out)
{
    out = QByteArray(str.toUtf8()).toBase64();
}

void Smtp::connected()
{
    socket->waitForReadyRead(timeout);
}

void Smtp::readyRead()
{
    QString response = socket->readAll();
    int code = response.left(3).toInt();

    switch (state) {
    case 0: // Greeting reçu
        socket->write("EHLO localhost\r\n");
        state = 1;
        break;
    case 1: // EHLO OK
        if (code == 250) {
            socket->write("AUTH LOGIN\r\n");
            state = 2;
        }
        break;
    case 2: // AUTH LOGIN
        if (code == 334) {
            QString encoded;
            encode(user, encoded);
            socket->write((encoded + "\r\n").toUtf8());
            state = 3;
        }
        break;
    case 3: // Username envoyé
        if (code == 334) {
            QString encoded;
            encode(pass, encoded);
            socket->write((encoded + "\r\n").toUtf8());
            state = 4;
        }
        break;
    case 4: // Password envoyé
        if (code == 235) {
            socket->write(("MAIL FROM:<" + from + ">\r\n").toUtf8());
            state = 5;
        } else {
            emit mailSent(false, "Authentification échouée");
            socket->write("QUIT\r\n");
        }
        break;
    case 5: // MAIL FROM OK
        if (code == 250) {
            socket->write(("RCPT TO:<" + to + ">\r\n").toUtf8());
            state = 6;
        }
        break;
    case 6: // RCPT TO OK
        if (code == 250) {
            socket->write("DATA\r\n");
            state = 7;
        }
        break;
    case 7: // DATA OK
        if (code == 354) {
            QString mail =
                "From: " + from + "\r\n"
                "To: "   + to   + "\r\n"
                "Subject: " + subject + "\r\n"
                "\r\n" + body + "\r\n.\r\n";
            socket->write(mail.toUtf8());
            state = 8;
        }
        break;
    case 8: // Mail envoyé
        if (code == 250) {
            emit mailSent(true, "Mail envoyé avec succès !");
        } else {
            emit mailSent(false, "Erreur envoi : " + response);
        }
        socket->write("QUIT\r\n");
        break;
    }
    socket->waitForReadyRead(timeout);
}

void Smtp::errorReceived(QAbstractSocket::SocketError)
{
    emit mailSent(false, "Erreur réseau : " + socket->errorString());
}
