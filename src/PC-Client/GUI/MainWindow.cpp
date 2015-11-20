#include "MainWindow.h"
#include "ContactBackupWindow.h"
#include "AppManagementWindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <stdio.h>

void MainWindow::exec_action_exit()
{
	adb_tools->stop_adb_server();
	delete adb_tools;
	exit(0);
}

MainWindow::MainWindow(ADBTools* tools, QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	adb_tools = tools;
    ui->setupUi(this);
	if(adb_tools->is_run())
		ui->statusBar->showMessage("ADB Server已启动", 1000 * 3600);
    //窗体移动到屏幕正中央
    QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width())/2, (pDesk->height() - height())/2);
	
    //将退出菜单项连接到鼠标点击信号(相当于添加鼠标点击事件监听器)
    QObject::connect(ui->action_exit, SIGNAL(triggered()), this, SLOT(exec_action_exit()));
	QObject::connect(ui->button_connect_to_phone, SIGNAL(clicked(bool)), this, SLOT(on_button_connect_to_phone_click()));
	QObject::connect(ui->button_contact_backup, SIGNAL(clicked(bool)), this, SLOT(on_button_contact_backup_click()));
	QObject::connect(ui->button_app_manage, SIGNAL(clicked(bool)), this, SLOT(on_button_app_manage_click()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	exec_action_exit();
}

void MainWindow::on_button_connect_to_phone_click()
{
	string phone_manufacturer = adb_tools->get_phone_manufacturer();
	string phone_model = adb_tools->get_phone_model();
	ui->button_connect_to_phone->setEnabled(false);
	if(!phone_manufacturer.empty() || !phone_model.empty())
	{
		if(!phone_manufacturer.empty() && !phone_model.empty())
			ui->label_phone_info->setText(QString::fromStdString((phone_manufacturer + " " + phone_model)));
		else if(phone_manufacturer.empty())
			ui->label_phone_info->setText(QString::fromStdString(phone_model));
		else
			ui->label_phone_info->setText(QString::fromStdString(phone_manufacturer));
		if(!adb_tools->connect_to_phone())
		{
			QMessageBox::critical(this, "错误", "与手机端守护App连接时发生错误。");
			ui->button_app_manage->setEnabled(false);
			ui->button_contact_backup->setEnabled(false);
			ui->button_sms_backup->setEnabled(false);
		}else {
			ui->statusBar->showMessage("与手机端守护App连接成功。", 1000 * 3600);
			ui->button_app_manage->setEnabled(true);
			ui->button_contact_backup->setEnabled(true);
			ui->button_sms_backup->setEnabled(true);
		}
	}else {
		ui->button_app_manage->setEnabled(false);
		ui->button_contact_backup->setEnabled(false);
		ui->button_sms_backup->setEnabled(false);
		QMessageBox::critical(this, "错误", "无法获取手机信息，请确认您的手机是否已经连上电脑并且已打开USB调试模式。");
	}
	ui->button_connect_to_phone->setEnabled(true);
}

void MainWindow::on_button_contact_backup_click()
{
	ContactBackupWindow* window = new ContactBackupWindow(adb_tools, this);
	window->show();
}

void MainWindow::on_button_app_manage_click()
{
	AppManagementWindow* window = new AppManagementWindow(adb_tools, this);
	window->show();
}

