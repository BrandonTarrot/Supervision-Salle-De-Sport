#ifndef MOT_DE_PASSE_OUBLIE_H
#define MOT_DE_PASSE_OUBLIE_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class mot_de_passe_oublie : public QDialog
{
    Q_OBJECT

public:
    explicit mot_de_passe_oublie(const QString &codeBackup, QWidget *parent = nullptr);
    ~mot_de_passe_oublie();

private slots:
    void on_confirmer();

private:
    QString      m_codeBackup;
    QPushButton *m_btnConfirmer;
};

#endif // MOT_DE_PASSE_OUBLIE_H
