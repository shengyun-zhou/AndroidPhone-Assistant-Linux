#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "UI_MainWindow.h"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public slots:
        void exec_action_exit();
    private:
        Ui::MainWindow* ui;
    public:
        explicit MainWindow(QWidget* parent = NULL);
        ~MainWindow();
};

#endif // MAINWINDOW_H
