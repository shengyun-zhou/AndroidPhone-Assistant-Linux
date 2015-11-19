#include "ContactBackupWindow.h"
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
	model = new QStandardItemModel();
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
	if(!adb_tools->is_connected())
	{
		QMessageBox::critical(this, "错误", "与手机端守护App的连接已断开。");
		return;
	}
	if(!adb_tools->get_contacts_list(contact_list))
		QMessageBox::critical(this, "错误", "读取手机中联系人列表时发生错误。");
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
		model->setItem(i, 0, new QStandardItem(contact_list[i].get_display_name().c_str()));
		model->setItem(i, 1, new QStandardItem(contact_list[i].get_phone_number().c_str()));
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
	QString db_file_path = QFileDialog::getSaveFileName(this, "选择备份文件保存位置", "contact_backup.db", "DB Files(*.db)");
	if(!db_file_path.isNull() && !db_file_path.isEmpty())
		printf("file path:%s\n", db_file_path.toStdString().c_str());
}

void ContactBackupWindow::exec_action_rescan()
{
	scan_contact_list();
}

