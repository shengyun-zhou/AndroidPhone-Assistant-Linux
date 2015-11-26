#include "SMSBackupWindow.h"
#include <QMessageBox>
#include <QLabel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDesktopWidget>
#include <stdio.h>

SMSBackupWindow::SMSBackupWindow(ADBTools* tools, QWidget* parent): QMainWindow(parent)
{
	adb_tools = tools;
	ui = new Ui::SMSBackupWindow();
	ui->setupUi(this);
	
	QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width())/2, (pDesk->height() - height())/2);
}

SMSBackupWindow::~SMSBackupWindow()
{
	delete ui;
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
	set_tree_model();
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
	QStandardItem* root_item = new QStandardItem(temp);
	root_item->setEditable(false);
	root_item->setCheckable(true);
	root_item->setTristate(true);
	root_item->setCheckState(Qt::PartiallyChecked);
	model->appendRow(root_item);
	if(sms_list.size() <= 0)
		return;
	sms_group_list = sms_list_sort();
	for(int i = 0; i < sms_group_list.size(); i++)
	{
		sprintf(temp, "%s(%d条)", sms_group_list[i].phone_number.toStdString().c_str(), (int)sms_group_list[i].sms_index.size());
		QStandardItem* phone_num_item = new QStandardItem(temp);
		phone_num_item->setEditable(false);
		phone_num_item->setCheckable(true);
		phone_num_item->setTristate(true);
		root_item->appendRow(phone_num_item);
		for(int j = 0; j < sms_group_list[i].sms_index.size(); j++)
		{
			QStandardItem* sms_date_item = new QStandardItem(QString::fromStdString(sms_list[sms_group_list[i].sms_index[j]].get_date_str()));
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
