#ifndef APPMANAGEMENTWINDOW_H
#define APPMANAGEMENTWINDOW_H

#include "UI_AppManagementWindow.h"
#include "../adb-tools/ADBTools.h"
#include <QThread>
#include <QStandardItemModel>
#include <QCheckBox>
#include <QProgressBar>

using namespace std;

class AppManagementWindow : public QMainWindow
{
	Q_OBJECT
	private:
		vector<AppInfo> app_list;
		Ui::AppManagementWindow* ui;
		ADBTools* adb_tools;
		QProgressBar* loop_progress;
		QStandardItemModel* table_user_app_model;
		QStandardItemModel* table_system_app_model;
		vector<QCheckBox*> item_checkbox_user_app;
		vector<int> item_app_point_user_app;
		vector<QCheckBox*> item_checkbox_system_app;
		vector<int> item_app_point_system_app;
		
		void exec_scan_app_task();
		void exec_backup_app_task(vector<int>& index_list, QString& output_dir);
		void exec_uninstall_app_task(vector<int>& index_list);
		void exec_install_app_task(QString& apk_path);
		void set_table_model();
		
	protected:
		virtual void showEvent(QShowEvent*);
	private slots:
		void on_button_apk_browse_click();
		void on_scan_app_complete(bool);
		void on_backup_app_complete(QString, bool, QString, bool);
		void on_uninstall_complete(QString, bool, QString, bool);
		void on_install_complete(QString, bool);
		void exec_action_rescan_app();
		void on_checkbox_select_all_user_click(int);
		void on_checkbox_select_all_system_click(int);
		void on_button_backup_user_click();
		void on_button_backup_system_click();
		void on_button_uninstall_user_click();
		void on_button_install_click();
	public:
		AppManagementWindow(ADBTools* tools, QWidget* parent = NULL);
		~AppManagementWindow();
};

class ScanAppThread : public QThread
{
	Q_OBJECT
	private:
		vector<AppInfo>* app_list;
		ADBTools* adb_tools;
	public:
		ScanAppThread(ADBTools* adb_tools, vector<AppInfo>* app_list)
		{
			this->app_list = app_list;
			this->adb_tools = adb_tools;
		}
		protected:
			virtual void run();
		signals:
			void scan_complete(bool);
};

class BackupAppThread : public QThread
{
	Q_OBJECT
	private:
		vector<AppInfo> app_list;
		vector<int> index_list;
		ADBTools* adb_tools;
		QString output_dir;
	public:
		BackupAppThread(ADBTools* adb_tools, vector<AppInfo>& app_list, vector<int>& index_list, QString& output_dir)
		{
			this->app_list = app_list;
			this->index_list = index_list;
			this->adb_tools = adb_tools;
			this->output_dir = output_dir;
		}
		protected:
			virtual void run();
		signals:
			void backup_complete(QString, bool, QString, bool);
};

class UninstallAppThread : public QThread
{
	Q_OBJECT
	private:
		vector<AppInfo> app_list;
		vector<int> index_list;
		ADBTools* adb_tools;
	public:
		UninstallAppThread(ADBTools* adb_tools, vector<AppInfo>& app_list, vector<int>& index_list)
		{
			this->app_list = app_list;
			this->index_list = index_list;
			this->adb_tools = adb_tools;
		}
		protected:
			virtual void run();
		signals:
			void uninstall_complete(QString, bool, QString, bool);
};


class InstallAppThread : public QThread
{
	Q_OBJECT
	private:
		ADBTools* adb_tools;
		QString apk_path;
	public:
		InstallAppThread(ADBTools* adb_tools, QString& apk_path)
		{
			this->adb_tools = adb_tools;
			this->apk_path = apk_path;
		}
		protected:
			virtual void run();
		signals:
			void install_complete(QString, bool);
};
#endif // APPMANAGEMENTWINDOW_H
