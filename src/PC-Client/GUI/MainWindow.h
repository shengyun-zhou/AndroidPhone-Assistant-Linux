#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "UI_MainWindow.h"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    private:
        Ui::MainWindow* ui;
    public:
        explicit MainWindow(QWidget* parent = NULL);
        virtual ~MainWindow();
    public slots:
        void exec_action_exit();
};

#endif // MAINWINDOW_H
