#include "ContactBackupWindow.h"
#include "../tools/SMSAndContactBackup.h"
#include "../tools/GTKTools.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <stdio.h>

ContactBackupWindow::ContactBackupWindow(ADBTools* adb_tools, QWidget* parent) : QMainWindow(parent)
{
	ui = new Ui::ContactBackupWindow();
	this->adb_tools = adb_tools;
	
	ui->setupUi(this);
	
	QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width())/2, (pDesk->height() - height())/2);
	
	ui->table_contact_list->verticalHeader()->hide();
	model = new QStandardItemModel(ui->table_contact_list);
    ui->table_contact_list->setModel(model);
    ui->table_contact_list->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	
	QObject::connect(ui->checkbox_select_all, SIGNAL(stateChanged(int)), this, SLOT(on_checkbox_select_all_click()));
	QObject::connect(ui->button_backup, SIGNAL(clicked(bool)), this, SLOT(on_button_backup_click()));
	QObject::connect(ui->action_rescan_contact_list, SIGNAL(triggered(bool)), this, SLOT(exec_action_rescan()));
}

ContactBackupWindow::~ContactBackupWindow()
{
	delete ui;
	delete model;
}

void ContactBackupWindow::showEvent(QShowEvent* e)
{
	QWidget::showEvent(e);
	scan_contact_list();
}

void ContactBackupWindow::scan_contact_list()
{
	scan_progress_dialog = new QProgressDialog(this);
	QLabel* label = new QLabel("正在读取手机中的联系人信息...\n"
								"如果您的手机对此进行拦截，请选择允许。");
	scan_progress_dialog->setLabel(label);
	scan_progress_dialog->setMinimumWidth(500);
	scan_progress_dialog->setWindowTitle(this->windowTitle());
	scan_progress_dialog->setMinimum(0);
	scan_progress_dialog->setMaximum(0);
	scan_progress_dialog->setCancelButton(NULL);
	contact_list.clear();
	ScanContactThread* thread = new ScanContactThread(adb_tools, &contact_list, this);
	QObject::connect(thread, SIGNAL(scan_contact_complete(bool)), this, SLOT(on_scan_contact_complete(bool)));
	thread->start();
	scan_progress_dialog->exec();
}

void ContactBackupWindow::on_scan_contact_complete(bool success)
{
	if(!success)
	{
		QMessageBox::critical(this, "错误", "读取手机中联系人列表时发生错误。");
		return;
	}
	scan_progress_dialog->close();
	delete scan_progress_dialog;
	set_table_model();
}

void ContactBackupWindow::set_table_model()
{
	item_checkbox.clear();
	char temp[1024];
	sprintf(temp, "共读取到 %d 条联系人记录", (int)contact_list.size());
	ui->label_contact_amount->setText(temp);
	model->clear();
	model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("联系人姓名"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("号码"));
	model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit(""));
	for(int i = 0; i < contact_list.size(); i++)
	{
		item_checkbox.push_back(new QCheckBox(this));
		QStandardItem* display_name_item = new QStandardItem(contact_list[i].get_display_name().c_str());
		display_name_item->setEditable(false);
		QStandardItem* phone_number_item = new QStandardItem(contact_list[i].get_phone_number().c_str());
		phone_number_item->setEditable(false);
		model->setItem(i, 0, display_name_item);
		model->setItem(i, 1, phone_number_item);
		ui->table_contact_list->setIndexWidget(model->index(i, 2), item_checkbox[i]);
	}
	ui->table_contact_list->setColumnWidth(0, (ui->table_contact_list->width() - 50 - 5) / 3);
    ui->table_contact_list->setColumnWidth(1, (ui->table_contact_list->width() - 50 - 5) / 3 * 2);
	ui->table_contact_list->setColumnWidth(2, 50);
}

void ContactBackupWindow::on_checkbox_select_all_click()
{
	if(ui->checkbox_select_all->checkState() == Qt::Checked)
	{
		for(int i = 0; i < item_checkbox.size(); i++)
			item_checkbox[i]->setCheckState(Qt::Checked);
	}
	else
	{
		for(int i = 0; i < item_checkbox.size(); i++)
			item_checkbox[i]->setCheckState(Qt::Unchecked);
	}
}

void ContactBackupWindow::on_button_backup_click()
{
	vector<ContactInfo> backup_list;
	for(int i = 0; i < item_checkbox.size(); i++)
	{
		if(item_checkbox[i]->checkState() == Qt::Checked)
			backup_list.push_back(contact_list[i]);
	}
	if(backup_list.empty())
	{
		QMessageBox::information(this, "提示", "请选择至少一条联系人记录来备份。");
		return;
	}
	QString db_file_path = QString::fromStdString(GTKTools::get_save_file_name("选择备份文件保存位置", "DB Files(*.db)", "*.db"));
	if(!db_file_path.isNull() && !db_file_path.isEmpty())
	{
		if(!SMSAndContactBackup::export_contact(db_file_path.toStdString().c_str(), backup_list))
			QMessageBox::critical(this, "错误", "联系人备份失败。");
		else
			QMessageBox::information(this, "提示", QString("联系人备份完毕。\n备份文件：") + db_file_path);
	}
}

void ContactBackupWindow::exec_action_rescan()
{
	scan_contact_list();
}

void ScanContactThread::run()
{
	emit scan_contact_complete(adb_tools->get_contacts_list(*contact_list));
}

