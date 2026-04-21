#ifndef SMTP_H
#define SMTP_H

#include <QtNetwork/QSslSocket>
#include <QString>
#include <QDebug>

class Smtp : public QObject
{
    Q_OBJECT

public:
    Smtp(const QString &user, const QString &pass,
         const QString &host, int port = 465);
    void sendMail(const QString &from, const QString &to,
                  const QString &subject, const QString &body);

signals:
    void status(const QString &);

private slots:
    void readyRead();
    void connected();

private:
    QString     user, pass, host;
    int         port;
    QSslSocket *socket;
    QString     from, to, subject, body;

    enum State {
        Init, Ehlo, Auth, User, Pass,
        Mail, Rcpt, Data, Body, Quit, Close
    };
    State state;
};

#endif // SMTP_H
