#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "UI_MainWindow.h"
#include "../adb-tools/ADBTools.h"
#include <QMainWindow>
#include <QLabel>
#include <QThread>
#include <QDialog>

class ConnectionMonitorThread : public QThread
{
	Q_OBJECT
	private:
		ADBTools* adb_tools;
	protected:
		virtual void run();
	signals:
		void connect_complete(bool);
		void disconnect_from_phone();
	public:
		ConnectionMonitorThread(ADBTools* adb_tools)
		{
			this->adb_tools = adb_tools;
		}
};

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
		void on_disconnect_from_phone();
		void on_connect_complete(bool);
    private:
        Ui::MainWindow* ui;
		ADBTools* adb_tools;
		ConnectionMonitorThread* moniter_thread;
		QDialog* connect_dialog;
		
		void start_adb_server(bool is_root = false);
		void set_connect_status(bool connect_status);
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
