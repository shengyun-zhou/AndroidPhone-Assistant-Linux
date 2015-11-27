#ifndef CONTACTBACKUPWINDOW_H
#define CONTACTBACKUPWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QProgressDialog>
#include <QThread>
#include "../adb-tools/ADBTools.h"
#include "UI_ContactBackupWindow.h"

class ContactBackupWindow : public QMainWindow
{
	Q_OBJECT
	private:
		ADBTools* adb_tools;
		Ui::ContactBackupWindow* ui;
		QProgressDialog* scan_progress_dialog;
		QStandardItemModel *model;
		
		vector<QCheckBox*> item_checkbox;
		vector<ContactInfo> contact_list;
		void scan_contact_list();
		void set_table_model();
	public:
		ContactBackupWindow(ADBTools* adb_tools, QWidget* parent = NULL);
		~ContactBackupWindow();
	private slots:
		void on_scan_contact_complete(bool);
		void exec_action_rescan();
		void on_checkbox_select_all_click();
		void on_button_backup_click();
		void on_contact_backup_complete(bool, const QString&);
	protected:
		virtual void showEvent(QShowEvent* e);
};

class ScanContactThread : public QThread
{
	Q_OBJECT
	private:
		ADBTools* adb_tools;
		vector<ContactInfo>* contact_list;
	public:
		ScanContactThread(ADBTools* tools, vector<ContactInfo>* contact_list, QObject* parent = 0) : QThread(parent)
		{
			adb_tools = tools;
			this->contact_list = contact_list;
		}
	signals:
		void scan_contact_complete(bool);
	protected:
		virtual void run();
};

class ContactBackupThread : public QThread
{
	Q_OBJECT
	private:
		ADBTools* adb_tools;
		vector<ContactInfo> contact_list;
		string backup_path;
	public:
		ContactBackupThread(ADBTools* tools, vector<ContactInfo>& contact_list, const string& backup_path, QObject* parent = 0) : QThread(parent)
		{
			adb_tools = tools;
			this->contact_list = contact_list;
			this->backup_path = backup_path;
		}
	signals:
		void contact_backup_complete(bool, QString);
	protected:
		virtual void run();
};

#endif // CONTACTBACKUPWINDOW_H
