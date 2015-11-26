#ifndef SMSBACKUPWINDOW_H
#define SMSBACKUPWINDOW_H
#include <QMainWindow>
#include "UI_SMSBackupWindow.h"
#include <QThread>
#include <QProgressDialog>
#include "../adb-tools/ADBTools.h"

class SMSBackupWindow : public QMainWindow
{
	Q_OBJECT
	private:
		Ui::SMSBackupWindow* ui;
		ADBTools* adb_tools;
		vector<SMSInfo> sms_list;
		QProgressDialog* scan_progress_dialog;
		
		struct SMSGroup
		{
			QString phone_number;
			vector<int> sms_index;
		};
		vector<SMSGroup> sms_group_list;
		
		void scan_sms_list();
		void set_tree_model();
		vector<SMSGroup> sms_list_sort();
	protected:
		virtual void showEvent(QShowEvent*);
	private slots:
		void on_scan_sms_complete(bool);
	public:
		SMSBackupWindow(ADBTools* tools,  QWidget* parent = 0);
		virtual ~SMSBackupWindow();
};

class ScanSMSThread : public QThread
{
	Q_OBJECT
	private:
		ADBTools* adb_tools;
		vector<SMSInfo>* sms_list;
	public:
		ScanSMSThread(ADBTools* tools, vector<SMSInfo>* sms_list, QObject* parent = 0) : QThread(parent)
		{
			adb_tools = tools;
			this->sms_list = sms_list;
		}
	signals:
		void scan_sms_complete(bool);
	protected:
		virtual void run();
};

#endif // SMSBACKUPWINDOW_H
