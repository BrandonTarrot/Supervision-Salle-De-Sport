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
    ~MainWindow();

    // ← Pour rendre le label "Mot de passe oublié ?" cliquable
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_pushButton_Login_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Backup_clicked();
    void on_checkBox_stateChanged(int state);
    void onTimerTick();

private:
    Ui::MainWindow *ui;

    // ── Compteurs de blocage ──
    int m_tentativesEchouees = 0;
    int m_niveauBlocage      = 0;
    int m_secondesRestantes  = 0;

    // ── Code backup ──
    bool    m_backupUtilise    = false;
    int     m_backupGeneration = 0;
    QString m_codeBackupActuel = "";

    // ── Timer ──
    QTimer *m_timer;

    // ── Méthodes utilitaires ──
    void    demarrerBlocage();
    void    activerInterface(bool actif);
    QString formatTemps(int secondes);
    QString genererCodeBackup();
    void    regenererCode();
};

#endif // MAINWINDOW_H
