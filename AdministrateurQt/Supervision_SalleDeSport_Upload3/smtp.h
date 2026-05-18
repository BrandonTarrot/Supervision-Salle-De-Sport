#ifndef SMTP_H
#define SMTP_H

#include <QObject>
#include <QSslSocket>

class Smtp : public QObject
{
    Q_OBJECT
public:
    Smtp(const QString &user, const QString &pass, const QString &host,
         const QString &from, const QString &to,
         const QString &subject, const QString &body,
         int port = 465, int timeout = 5000);

signals:
    void mailSent(bool success, const QString &message);

private slots:
    void connected();
    void readyRead();
    void errorReceived(QAbstractSocket::SocketError);

private:
    QSslSocket *socket;
    QString user, pass, from, to, subject, body;
    int timeout, state;
    void encode(const QString &str, QString &out);
};

#endif // SMTP_H
