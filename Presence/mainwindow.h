#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"   // génère Ui::MainWindow

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Login_clicked();
    void on_pushButton_Cancel_clicked();
    void on_checkBox_stateChanged(int state);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
