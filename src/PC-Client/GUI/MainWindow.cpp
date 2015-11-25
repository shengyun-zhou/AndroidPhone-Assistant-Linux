#include "MainWindow.h"
#include "ContactBackupWindow.h"
#include "AppManagementWindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include <QInputDialog>
#include <stdio.h>
#include <unistd.h>
#include "../tools/CommandTools.h"

void MainWindow::exec_action_exit()
{
	adb_tools->stop_adb_server();
	delete adb_tools;
	exit(0);
}

void MainWindow::exec_action_root_reboot()
{
	start_adb_server(true);
}

void MainWindow::exec_action_about()
{
	QMessageBox::about(this, "关于", "手机助手 for Linux\n"
						"License:GPL v3\n"
						"Copyright © 2015 buptsse-zero");
}

MainWindow::MainWindow(ADBTools* tools, QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	adb_tools = tools;
    ui->setupUi(this);
	
	moniter_thread = NULL;
	connect_dialog = NULL;
	if(geteuid() == 0 || getegid() == 0)
	{
		ui->action_root->setEnabled(false);
		setWindowTitle(windowTitle() + " [正在以root用户身份运行]");
	}
	
	ui->button_contact_backup->setIcon(QIcon(":icon/contact-list.svg"));
	ui->icon_phone->setPixmap(QPixmap(":icon/phone.svg"));
	ui->button_app_manage->setIcon(QIcon(":icon/app-list.svg"));
	ui->button_sms_backup->setIcon(QIcon(":icon/sms-list.svg"));
	ui->button_connect_to_phone->setIcon(QIcon(":icon/connect.svg"));
	
    //窗体移动到屏幕正中央
    QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width())/2, (pDesk->height() - height())/2);
	
    //将退出菜单项连接到鼠标点击信号(相当于添加鼠标点击事件监听器)
    QObject::connect(ui->action_exit, SIGNAL(triggered()), this, SLOT(exec_action_exit()));
	QObject::connect(ui->action_about, SIGNAL(triggered(bool)), this, SLOT(exec_action_about()));
	QObject::connect(ui->action_root, SIGNAL(triggered(bool)), this, SLOT(exec_action_root_reboot()));
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

void MainWindow::set_connect_status(bool connect_status)
{
	if(connect_status)
	{
		ui->statusBar->showMessage("与手机端守护App连接成功");
	}else{
		ui->label_phone_info->setText("请连接您的Android手机");
		ui->statusBar->showMessage("与手机端守护App连接失败");
	}
	ui->button_app_manage->setEnabled(connect_status);
	ui->button_contact_backup->setEnabled(connect_status);
	ui->button_sms_backup->setEnabled(connect_status);
}

void MainWindow::on_button_connect_to_phone_click()
{
	if(adb_tools->is_connected())
		return;
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
		connect_dialog = new QProgressDialog(this);
		QLabel* label = new QLabel(this);
		label->setText(QString("正在连接手机") + ui->label_phone_info->text() + "...");
		dynamic_cast<QProgressDialog*>(connect_dialog)->setLabel(label);
		dynamic_cast<QProgressDialog*>(connect_dialog)->setMinimum(0);
		dynamic_cast<QProgressDialog*>(connect_dialog)->setMaximum(0);
		connect_dialog->setWindowTitle(this->windowTitle());
		connect_dialog->setMinimumWidth(500);
		
		moniter_thread = new ConnectionMonitorThread(adb_tools);
		QObject::connect(moniter_thread, SIGNAL(connect_complete(bool)), this, SLOT(on_connect_complete(bool)));
		QObject::connect(moniter_thread, SIGNAL(disconnect_from_phone()), this, SLOT(on_disconnect_from_phone()));
		moniter_thread->start();
		connect_dialog->exec();
		if(!adb_tools->is_connected() && moniter_thread->isRunning())
		{
			moniter_thread->terminate();
			//delete moniter_thread;
			moniter_thread = NULL;
			adb_tools->disconnect_from_phone();
			set_connect_status(false);
		}
	}else {
		QMessageBox::critical(this, "错误", "无法获取手机信息，请确认您的手机是否已经连上电脑并且已打开USB调试模式。\n"
							  "如果您确认您的手机已打开调试模式仍无法获取信息，请尝试以root用户身份重启ADB Server后再重试连接。");
		set_connect_status(false);
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

void MainWindow::start_adb_server(bool is_root)
{
	QLabel* label = new QLabel(this);
	QProgressDialog progress_dialog(this);
	progress_dialog.setLabel(label);
	progress_dialog.setWindowTitle(this->windowTitle());
	progress_dialog.setMinimumWidth(500);
	progress_dialog.setMinimum(0);
	progress_dialog.setMaximum(0);
	progress_dialog.setCancelButton(NULL);
	progress_dialog.setLabelText("正在启动ADB Server...");
	
	ADBStartThread* thread = new ADBStartThread(adb_tools, &progress_dialog);
	bool reboot_adb_flag = false;
	const char* login_user = getlogin();
	while(true)
	{
		if(is_root)
		{
			bool ok = false;
			QString root_passwrod = QInputDialog::getText(this, "提升权限认证", QString("此操作需要获取root权限。\n") +
									"请输入当前登录用户 " + login_user + " 的密码:", QLineEdit::Password, "", &ok);
			if(!ok)
			{
				if(!reboot_adb_flag)
					return;
				is_root = false;
				progress_dialog.setLabelText("正在以普通用户身份重新启动ADB Server...");
				continue;
			}
			thread->setIsRoot(true);
			thread->setRootPassword(root_passwrod);
		} else {
			thread->setIsRoot(false);
			thread->setRootPassword("");
		}
		if(moniter_thread)
		{
			moniter_thread->stop_monitor();
			delete moniter_thread;
			moniter_thread = NULL;
		}
		thread->start();
		progress_dialog.exec();
		while(thread->isRunning())
			QThread::usleep(100);
		
		ADBTools::ADBStartError start_status = thread->get_adb_start_result();
		if(start_status == ADBTools::ADB_START_SUCCESSFULLY)
		{
			if(is_root)
				ui->action_root->setEnabled(false);
			break;
		}
		if(start_status == ADBTools::ADB_START_ROOT_WRONG_PASSWORD)
		{
			QMessageBox::critical(NULL, "错误", "用户认证失败，请重新输入密码。");
			reboot_adb_flag = true;
			continue;
		}
		if(start_status == ADBTools::ADB_START_PORT_UNAVAILABLE)
		{
			QMessageBox::critical(NULL, "错误", "ADB Server启动失败，请手动关闭其它占用本机5037端口的程序后再重试。");
			exit(1);
		}
		else if(start_status != ADBTools::ADB_START_SUCCESSFULLY)
		{
			QMessageBox::critical(NULL, "错误", "ADB Server启动失败。");
			exit(1);
		}
	}
	set_connect_status(false);
	ui->statusBar->showMessage("ADB Server已启动");
	delete label;
}

void MainWindow::showEvent(QShowEvent*)
{
	start_adb_server();
}

void ADBStartThread::run()
{
	QThread::msleep(300);
	start_result = adb_tools->start_adb_server(is_root, root_password.toStdString());
	progress_dialog->close();
}

void ConnectionMonitorThread::run()
{
	stop_flag = false;
	QThread::msleep(200);
	if(!adb_tools->connect_to_phone())
	{
		emit connect_complete(false);
		return;
	}
	else
		emit connect_complete(true);
	while(true)
	{
		if(stop_flag)
			return;
		if(!adb_tools->is_connected())
		{
			emit disconnect_from_phone();
			return;
		}
		QThread::msleep(500);
	}
}

void ConnectionMonitorThread::stop_monitor()
{
	stop_flag = true;
	while(!isFinished())
		msleep(50);
}

void MainWindow::on_connect_complete(bool connect_result)
{
	if(connect_dialog)
	{
		connect_dialog->close();
		delete connect_dialog;
		connect_dialog = NULL;
	}
	set_connect_status(connect_result);
	if(!connect_result)
		QMessageBox::critical(this, "错误", "与手机端守护App连接时发生错误。");
}

void MainWindow::on_disconnect_from_phone()
{
	QMessageBox* message_box = new QMessageBox(QMessageBox::Information, this->windowTitle(), "与手机端守护App的连接已经中断。",
												QMessageBox::Ok, NULL);
	message_box->show();
	while(moniter_thread->isRunning())
		QThread::msleep(50);
	delete moniter_thread;
	moniter_thread = NULL;
	set_connect_status(false);
}
