#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "UI_MainWindow.h"
#include "../adb-tools/ADBTools.h"
#include <QMainWindow>
#include <QDialog>
#include <QLabel>
#include <QThread>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public slots:
        void exec_action_exit();
		void exec_action_root_reboot();
		void exec_action_about();
		void on_button_connect_to_phone_click();
		void on_button_contact_backup_click();
		void on_button_app_manage_click();
    private:
        Ui::MainWindow* ui;
		ADBTools* adb_tools;
		
		void start_adb_server(bool is_root = false);
	protected:
		virtual void closeEvent(QCloseEvent* e);
		virtual void showEvent(QShowEvent*);
    public: 
        explicit MainWindow(ADBTools* adb_tools, QWidget* parent = NULL);
        ~MainWindow();
};

class ADBStartThread : public QThread
{
	private:
		ADBTools* adb_tools;
		ADBTools::ADBStartError start_result;
		QDialog* progress_dialog;
		bool is_root;
		QString root_password;
	public:
		ADBTools::ADBStartError get_adb_start_result()
		{
			return start_result;
		}
		
		ADBStartThread(ADBTools* tools, QDialog* dialog)
		{
			adb_tools = tools;
			progress_dialog = dialog;
			this->is_root = false;
			this->root_password = "";
		}
		
		void setIsRoot(bool is_root)
		{
			this->is_root = is_root;
		}
		
		void setRootPassword(const QString& root_password)
		{
			this->root_password = root_password;
		}
	protected:	
		virtual void run();
};

#endif // MAINWINDOW_H
