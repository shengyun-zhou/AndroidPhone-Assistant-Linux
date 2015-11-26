#include "SMSBackupWindow.h"
#include <QMessageBox>
#include <QLabel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <stdio.h>

SMSBackupWindow::SMSBackupWindow(ADBTools* tools, QWidget* parent): QMainWindow(parent)
{
	adb_tools = tools;
	ui = new Ui::SMSBackupWindow();
	ui->setupUi(this);
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
}

int find_father(int array[], int index)
{
	
}

vector< SMSBackupWindow::SMSGroup > SMSBackupWindow::sms_list_sort()
{

}

void SMSBackupWindow::set_tree_model()
{
	QStandardItemModel* model = new QStandardItemModel(ui->tree_sms_list);
	model->setColumnCount(2);
	model->setHorizontalHeaderLabels(QStringList() << "项目" << "短信内容");
	ui->tree_sms_list->setModel(model);
	char temp[100];
	sprintf(temp, "所有短信(%d条)", (int)sms_list.size());
	QStandardItem* root_item = new QStandardItem(temp);
}

void SMSBackupWindow::showEvent(QShowEvent*)
{
	scan_sms_list();
}

void ScanSMSThread::run()
{
	emit scan_sms_complete(adb_tools->get_sms_list(*sms_list));
}
