#include "smtp.h"
#include <QByteArray>


//CONSTRUCTEUR

Smtp::Smtp(const QString &user, const QString &pass, const QString &host,
           const QString &from, const QString &to,
           const QString &subject, const QString &body,
           int port, int timeout)
    : user(user), pass(pass), from(from), to(to),
      subject(subject), body(body), timeout(timeout), state(0)
{
    socket = new QSslSocket(this);

    connect(socket, SIGNAL(connected()),    this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()),    this, SLOT(readyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,   SLOT(errorReceived(QAbstractSocket::SocketError)));

    socket->connectToHostEncrypted(host, port);

    bool connexionReussie = socket->waitForEncrypted(timeout);
    if (!connexionReussie)
    {
        QString messageErreur = "Connexion SSL échouée : " + socket->errorString();
        emit mailSent(false, messageErreur);
    }
}


void Smtp::encode(const QString &str, QString &out)
{
    QByteArray donneesUtf8    = str.toUtf8();
    QByteArray donneesEncodees = donneesUtf8.toBase64();
    out = QString(donneesEncodees);
}



//ENVOI

void Smtp::connected()
{
    socket->waitForReadyRead(timeout);
}

void Smtp::readyRead()
{
    QString reponse = socket->readAll();
    QString codeStr = reponse.left(3);
    int     code    = codeStr.toInt();

    if (state == 0)
    {
        socket->write("EHLO localhost\r\n");
        state = 1;
    }
    else if (state == 1)
    {
        if (code == 250)
        {
            socket->write("AUTH LOGIN\r\n");
            state = 2;
        }
    }
    else if (state == 2)
    {
        if (code == 334)
        {
            QString utilisateurEncode;
            encode(user, utilisateurEncode);
            QString commande = utilisateurEncode + "\r\n";
            socket->write(commande.toUtf8());
            state = 3;
        }
    }
    else if (state == 3)
    {
        if (code == 334)
        {
            QString motDePasseEncode;
            encode(pass, motDePasseEncode);
            QString commande = motDePasseEncode + "\r\n";
            socket->write(commande.toUtf8());
            state = 4;
        }
    }
    else if (state == 4)
    {
        if (code == 235)
        {
            QString commande = "MAIL FROM:<" + from + ">\r\n";
            socket->write(commande.toUtf8());
            state = 5;
        }
        else
        {
            emit mailSent(false, "Authentification échouée");
            socket->write("QUIT\r\n");
        }
    }
    else if (state == 5)
    {
        if (code == 250)
        {
            QString commande = "RCPT TO:<" + to + ">\r\n";
            socket->write(commande.toUtf8());
            state = 6;
        }
    }
    else if (state == 6)
    {
        if (code == 250)
        {
            socket->write("DATA\r\n");
            state = 7;
        }
    }
    else if (state == 7)
    {
        if (code == 354)
        {
            QString mail = "";
            mail = mail + "From: "    + from    + "\r\n";
            mail = mail + "To: "      + to      + "\r\n";
            mail = mail + "Subject: " + subject + "\r\n";
            mail = mail + "\r\n";
            mail = mail + body + "\r\n.\r\n";
            socket->write(mail.toUtf8());
            state = 8;
        }
    }
    else if (state == 8)
    {
        if (code == 250)
        {
            emit mailSent(true, "Mail envoyé avec succès !");
        }
        else
        {
            QString messageErreur = "Erreur envoi : " + reponse;
            emit mailSent(false, messageErreur);
        }
        socket->write("QUIT\r\n");
    }

    socket->waitForReadyRead(timeout);
}


//ERREUR RESEAU

void Smtp::errorReceived(QAbstractSocket::SocketError)
{
    QString messageErreur = "Erreur réseau : " + socket->errorString();
    emit mailSent(false, messageErreur);
}
