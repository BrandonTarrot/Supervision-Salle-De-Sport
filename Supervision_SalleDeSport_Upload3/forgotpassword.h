#ifndef FORGOTPASSWORD_H
#define FORGOTPASSWORD_H

#include <QDialog>

namespace Ui { class forgotpassword; }

class forgotpassword : public QDialog
{
    Q_OBJECT

public:
    explicit forgotpassword(QWidget *parent = nullptr);
    ~forgotpassword();

private slots:
    void on_pushButton_Confirmer_clicked();
    void on_pushButton_Retour_clicked();


private:
    Ui::forgotpassword *ui;
    QString genererCode();
};

#endif // FORGOTPASSWORD_H
