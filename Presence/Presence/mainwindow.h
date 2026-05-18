#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    bool eventFilter(QObject *obj, QEvent *event) override;
    ~MainWindow();

private slots:
    void on_pushButton_Login_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Backup_clicked();
    void on_checkBox_stateChanged(int state);
    void onTimerTick();

private:
    Ui::MainWindow *ui;

    int m_tentativesEchouees = 0;
    int m_niveauBlocage      = 0;
    int m_secondesRestantes  = 0;

    bool    m_backupUtilise    = false;
    int     m_backupGeneration = 0;
    QString m_codeBackupActuel = "";

    QTimer *m_timer;

    void    demarrerBlocage();
    void    activerInterface(bool actif);
    void    gererEchecConnexion();
    QString formatTemps(int secondes);
    QString genererCodeBackup();
    void    regenererCode();
};

#endif // MAINWINDOW_H
