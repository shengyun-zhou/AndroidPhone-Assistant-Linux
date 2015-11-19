#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "UI_MainWindow.h"
#include "../adb-tools/ADBTools.h"
#include <QMainWindow>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public slots:
        void exec_action_exit();
		void on_button_connect_to_phone_click();
		void on_button_contact_backup_click();
    private:
        Ui::MainWindow* ui;
		ADBTools* adb_tools;
	protected:
		virtual void closeEvent(QCloseEvent* e);
    public:
        explicit MainWindow(ADBTools* adb_tools, QWidget* parent = NULL);
        ~MainWindow();
};

#endif // MAINWINDOW_H
