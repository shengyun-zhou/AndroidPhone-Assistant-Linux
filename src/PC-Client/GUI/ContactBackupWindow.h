#ifndef CONTACTBACKUPWINDOW_H
#define CONTACTBACKUPWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "../adb-tools/ADBTools.h"
#include "UI_ContactBackupWindow.h"

class ContactBackupWindow : public QMainWindow
{
	Q_OBJECT
	private:
		ADBTools* adb_tools;
		Ui::ContactBackupWindow* ui;
		QStandardItemModel *model;
		
		vector<QCheckBox*> item_checkbox;
		vector<ContactInfo> contact_list;
		void scan_contact_list();
	public:
		ContactBackupWindow(ADBTools* adb_tools, QWidget* parent = NULL);
		~ContactBackupWindow();
	public slots:
		void exec_action_rescan();
		void on_checkbox_select_all_click();
		void on_button_backup_click();
	protected:
		virtual void showEvent(QShowEvent* e);
};

#endif // CONTACTBACKUPWINDOW_H
