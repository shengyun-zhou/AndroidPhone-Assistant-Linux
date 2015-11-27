#include "SMSBackupWindow.h"
#include <QMessageBox>
#include <QLabel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDesktopWidget>
#include <stdio.h>
#include "../tools/GTKTools.h"
#include "../tools/SMSAndContactBackup.h"

SMSBackupWindow::SMSBackupWindow(ADBTools* tools, QWidget* parent): QMainWindow(parent)
{
	adb_tools = tools;
	ui = new Ui::SMSBackupWindow();
	ui->setupUi(this);
	ui->splitter->setStretchFactor(0, 4);
	ui->splitter->setStretchFactor(1, 1);
	
	QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width())/2, (pDesk->height() - height())/2);
	
	QObject::connect(ui->button_backup_sms, SIGNAL(clicked(bool)), this, SLOT(on_button_backup_sms_click()));
	QObject::connect(ui->action_rescan_sms, SIGNAL(triggered()), this, SLOT(exec_action_rescan_sms_list()));
	QObject::connect(ui->tree_sms_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_tree_view_click(QModelIndex)));
}

SMSBackupWindow::~SMSBackupWindow()
{
	delete ui;
}

void SMSBackupWindow::exec_action_rescan_sms_list()
{
	scan_sms_list();
}

void SMSBackupWindow::scan_sms_list()
{
	scan_progress_dialog = new QProgressDialog(this);
	QLabel* label = new QLabel("正在读取手机中的短信记录...\n"
								"如果您的手机对此进行拦截，请选择允许。");
	scan_progress_dialog->setLabel(label);
	scan_progress_dialog->setMinimumWidth(500);
	scan_progress_dialog->setWindowTitle(this->windowTitle());
	scan_progress_dialog->setMinimum(0);
	scan_progress_dialog->setMaximum(0);
	scan_progress_dialog->setCancelButton(NULL);
	sms_list.clear();
	ScanSMSThread* thread = new ScanSMSThread(adb_tools, &sms_list, this);
	QObject::connect(thread, SIGNAL(scan_sms_complete(bool)), this, SLOT(on_scan_sms_complete(bool)));
	thread->start();
	ui->centralwidget->setEnabled(false);
	scan_progress_dialog->exec();
}

void SMSBackupWindow::on_scan_sms_complete(bool success)
{
	scan_progress_dialog->close();
	delete scan_progress_dialog;
	if(!success)
		QMessageBox::critical(this, "错误", "读取短信记录时出现错误。");
	char temp[100];
	sprintf(temp, "读取完成，共读取到 %d 条短信记录", (int)sms_list.size());
	ui->status_bar->showMessage(temp);
	ui->info_area_sms->clear();
	set_tree_model();
	ui->centralwidget->setEnabled(true);
}

void SMSBackupWindow::on_button_backup_sms_click()
{
	vector<SMSInfo> sms_backup_list;
	if(!root_item->hasChildren())
		return;
	for(int i = 0; i < root_item->rowCount(); i++)
	{
		QStandardItem* phone_num_item = root_item->child(i);
		for(int j = 0; j < phone_num_item->rowCount(); j++)
		{
			if(phone_num_item->child(j)->checkState() == Qt::Checked)
				sms_backup_list.push_back(sms_list[sms_group_list[i].sms_index[j]]);
		}
	}
	if(sms_backup_list.empty())
		return;
	string sms_backup_path = GTKTools::get_save_file_name("选择短信备份文件位置", "DB Files(*.db)", "*.db");
	if(sms_backup_path.empty())
		return;
	scan_progress_dialog = new QProgressDialog(this);
	QLabel* label = new QLabel("正在备份短信中...");
	scan_progress_dialog->setLabel(label);
	scan_progress_dialog->setMinimumWidth(500);
	scan_progress_dialog->setWindowTitle(this->windowTitle());
	scan_progress_dialog->setMinimum(0);
	scan_progress_dialog->setMaximum(0);
	scan_progress_dialog->setCancelButton(NULL);
	SMSBackupThread* thread = new SMSBackupThread(adb_tools, &sms_backup_list, sms_backup_path, scan_progress_dialog);
	QObject::connect(thread, SIGNAL(backup_sms_complete(bool,QString)), this, SLOT(on_sms_backup_complete(bool,QString)));
	thread->start();
	scan_progress_dialog->exec();
}

void SMSBackupWindow::on_sms_backup_complete(bool success, const QString& backup_path)
{
	scan_progress_dialog->close();
	delete scan_progress_dialog;
	if(success)
		QMessageBox::information(this, "提示", QString("短信备份完成，备份文件:") + backup_path);
	else
		QMessageBox::critical(this, "错误", "备份短信时发生错误。");
}

void SMSBackupWindow::on_tree_view_click(const QModelIndex& index)
{
	if(index.parent().row() < 0 || index.parent().parent().row() < 0)
	{
		ui->info_area_sms->clear();
		return;
	}
	int i = index.parent().row();
	int j = index.row();
	char temp[2048];
	sprintf(temp, "<h4><img src=\"%s\"><u>%s</u>&nbsp;&nbsp;" 
				  "<img src=\"%s\"><u>%s</u></h4>%s",
		":/icon/clock.svg",
		sms_list[sms_group_list[i].sms_index[j]].get_date_str().c_str(),
		":/icon/person.svg",
		sms_list[sms_group_list[i].sms_index[j]].get_phone_number().c_str(),
		sms_list[sms_group_list[i].sms_index[j]].get_sms_body().c_str());
	ui->info_area_sms->setText(temp);
}

bool check_change_flag = false;

void set_parent_checkstate(QStandardItem* item)
{
	if(item == NULL)
		return;
	if(!item->hasChildren())
		return;
	bool not_select_flag = false;
	bool select_flag = false;
	//printf("item text:%s\n", item->text().toStdString().c_str());
	for(int i = 0; i < item->rowCount(); i++)
	{
		if(item->child(i)->checkState() == Qt::PartiallyChecked)
			not_select_flag = select_flag = true;
		else if(item->child(i)->checkState() == Qt::Unchecked)
			not_select_flag = true;
		else
			select_flag = true;
	}
	if(not_select_flag && !select_flag)
	{
		if(item->checkState() != Qt::Unchecked)
			check_change_flag = true;
		item->setCheckState(Qt::Unchecked);
	}
	else if(!not_select_flag && select_flag)
	{
		if(item->checkState() != Qt::Checked)
			check_change_flag = true;
		item->setCheckState(Qt::Checked);
	}
	else if(not_select_flag && select_flag && item->isTristate())
	{
		if(item->checkState() != Qt::PartiallyChecked)
			check_change_flag = true;
		item->setCheckState(Qt::PartiallyChecked);
	}
	set_parent_checkstate(item->parent());
}

void set_child_checkstate(QStandardItem* item)
{
	if(!item->hasChildren())
		return;
	for(int i = 0; i < item->rowCount(); i++)
	{
		if(item->child(i)->checkState() != item->checkState())
			check_change_flag = true;
		item->child(i)->setCheckState(item->checkState());
		set_child_checkstate(item->child(i));
	}
}

void SMSBackupWindow::on_tree_view_item_check_change(QStandardItem* item)
{
	if(check_change_flag)
	{
		check_change_flag = false;
		return;
	}
	if(item->checkState() == Qt::PartiallyChecked)
	{
		item->setCheckState(Qt::Checked);
		return;
	}
	set_child_checkstate(item);
	set_parent_checkstate(item->parent());
}

vector<SMSBackupWindow::SMSGroup> SMSBackupWindow::sms_list_sort()
{
	vector<SMSGroup> result;
	if(sms_list.size() <= 0)
		return result;
	int* group_flag = new int[sms_list.size()];
	for(int i = 0; i < sms_list.size(); i++)
		group_flag[i] = -1;
	int group_count = 0;
	for(int i = 0; i < sms_list.size(); i++)
	{
		if(group_flag[i] >= 0)
			continue;
		group_flag[i] = group_count;
		SMSGroup new_group;
		new_group.sms_index = vector<int>();
		new_group.phone_number = QString::fromStdString(sms_list[i].get_phone_number());
		new_group.sms_index.push_back(i);
		for(int j = i + 1; j < sms_list.size(); j++)
		{
			if(group_flag[j] >= 0)
				continue;
			if(sms_list[j].get_phone_number() == sms_list[i].get_phone_number())
			{
				group_flag[j] = group_count;
				new_group.sms_index.push_back(j);
			}
		}
		group_count++;
		result.push_back(new_group);
	}
	delete group_flag;
	return result;
}

void SMSBackupWindow::set_tree_model()
{
	QStandardItemModel* model = new QStandardItemModel(ui->tree_sms_list);
	model->setColumnCount(2);
	model->setHorizontalHeaderLabels(QStringList() << "项目" << "短信内容");
	ui->tree_sms_list->setModel(model);
	ui->tree_sms_list->setColumnWidth(0, ui->tree_sms_list->width() / 2);
	ui->tree_sms_list->setColumnWidth(1, ui->tree_sms_list->width() / 2);
	char temp[100];
	sprintf(temp, "所有短信(%d条)", (int)sms_list.size());
	root_item = new QStandardItem(QIcon(":/icon/sms-list.svg") ,temp);
	model->appendRow(root_item);
	root_item->setEditable(false);
	if(sms_list.size() <= 0)
		return;
	root_item->setCheckable(true);
	root_item->setTristate(true);
	QObject::connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_tree_view_item_check_change(QStandardItem*)));
	sms_group_list = sms_list_sort();
	for(int i = 0; i < sms_group_list.size(); i++)
	{
		sprintf(temp, "%s(%d条)", sms_group_list[i].phone_number.toStdString().c_str(), (int)sms_group_list[i].sms_index.size());
		QStandardItem* phone_num_item = new QStandardItem(QIcon(":/icon/person.svg"), temp);
		phone_num_item->setEditable(false);
		phone_num_item->setCheckable(true);
		phone_num_item->setTristate(true);
		root_item->appendRow(phone_num_item);
		for(int j = 0; j < sms_group_list[i].sms_index.size(); j++)
		{
			QStandardItem* sms_date_item = new QStandardItem(QIcon(":/icon/clock.svg"), QString::fromStdString(sms_list[sms_group_list[i].sms_index[j]].get_date_str()));
			sms_date_item->setEditable(false);
			sms_date_item->setCheckable(true);
			QStandardItem* sms_body_item = new QStandardItem(QString::fromStdString(sms_list[sms_group_list[i].sms_index[j]].get_sms_body()));
			sms_body_item->setEditable(false);
			phone_num_item->appendRow(sms_date_item);
			phone_num_item->setChild(j, 1, sms_body_item);
		}
	}
}

void SMSBackupWindow::showEvent(QShowEvent*)
{
	scan_sms_list();
}

void ScanSMSThread::run()
{
	emit scan_sms_complete(adb_tools->get_sms_list(*sms_list));
}

void SMSBackupThread::run()
{
	emit backup_sms_complete(SMSAndContactBackup::export_sms(backup_path.c_str(), *sms_list), QString::fromStdString(backup_path));
}
