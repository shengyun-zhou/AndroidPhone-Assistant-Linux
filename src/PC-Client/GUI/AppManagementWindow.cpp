#include "AppManagementWindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>

AppManagementWindow::AppManagementWindow(ADBTools* tools, QWidget* parent) : QMainWindow(parent)
{
	adb_tools = tools;
	ui = new Ui::AppManagementWindow();
	ui->setupUi(this);
	
	QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width())/2, (pDesk->height() - height())/2);
	
	table_system_app_model = new QStandardItemModel(this);
	table_user_app_model = new QStandardItemModel(this);
	ui->table_system_app->setModel(table_system_app_model);
	ui->table_user_app->setModel(table_user_app_model);
	ui->table_user_app->verticalHeader()->hide();
	ui->table_system_app->verticalHeader()->hide();
	
	QObject::connect(ui->button_apk_browse, SIGNAL(clicked()), this, SLOT(on_button_apk_browse_click()));
	QObject::connect(ui->action_rescan_app_list, SIGNAL(triggered(bool)), this, SLOT(exec_action_rescan_app()));
	QObject::connect(ui->checkbox_select_all_user_app, SIGNAL(stateChanged(int)), this, SLOT(on_checkbox_select_all_user_click(int)));
	QObject::connect(ui->checkbox_select_all_system_app, SIGNAL(stateChanged(int)), this, SLOT(on_checkbox_select_all_system_click(int)));
	QObject::connect(ui->button_backup_user, SIGNAL(clicked(bool)), this, SLOT(on_button_backup_user_click()));
	QObject::connect(ui->button_backup_system, SIGNAL(clicked(bool)), this, SLOT(on_button_backup_system_click()));
	QObject::connect(ui->button_uninstall_user, SIGNAL(clicked(bool)), this, SLOT(on_button_uninstall_user_click()));
	QObject::connect(ui->button_install_apk, SIGNAL(clicked(bool)), this, SLOT(on_button_install_click()));
}

AppManagementWindow::~AppManagementWindow()
{
	delete ui;
}

void AppManagementWindow::exec_action_rescan_app()
{
	exec_scan_app_task();
}

void AppManagementWindow::exec_scan_app_task()
{
	ui->status_bar->showMessage("正在获取手机中所有已安装的应用信息...", 1000 * 3600);
	ui->action_rescan_app_list->setEnabled(false);
	app_list.clear();
	ScanAppThread* thread = new ScanAppThread(adb_tools, &app_list);
	QObject::connect(thread, SIGNAL(scan_complete(bool)), this, SLOT(on_scan_app_complete(bool)));
	thread->start();
}

void AppManagementWindow::on_scan_app_complete(bool is_successful)
{;
	if(!is_successful){
		QMessageBox::critical(this, "错误", "获取手机应用列表失败。");
		ui->status_bar->showMessage("获取手机应用列表失败", 1000 * 3600);
	}
	else 
		ui->status_bar->showMessage("获取手机应用列表完成", 1000 * 3600);
	set_table_model();
	ui->action_rescan_app_list->setEnabled(true);
}

QString get_app_size_str(int64_t app_size_byte)
{
	char result[100];
	string unit = "B";
	double app_size_result;
	if(app_size_byte / 1024 > 0)
	{
		unit = "KB";
		app_size_result = (app_size_byte) / 1024.0;
	}
	if(app_size_result / 1024 > 1.0)
	{
		unit = "MB";
		app_size_result /= 1024;
	}
	if(app_size_result / 1024 > 1.0)
	{
		unit = "GB";
		app_size_result /= 1024;
	}
	sprintf(result, "%.2lf%s", app_size_result, unit.c_str());
	return result;
}

void AppManagementWindow::set_table_model()
{
	QStringList header_list = QStringList();
	header_list.append("应用名");
	header_list.append("版本");
	header_list.append("大小");
	header_list.append("");
	
	item_checkbox_user_app.clear();
	item_checkbox_system_app.clear();
	item_app_point_system_app.clear();
	item_app_point_user_app.clear();
	table_user_app_model->clear();
	table_user_app_model->setColumnCount(header_list.size());
	table_system_app_model->clear();
	table_system_app_model->setColumnCount(header_list.size());
	
	table_user_app_model->setHorizontalHeaderLabels(header_list);
	table_system_app_model->setHorizontalHeaderLabels(header_list);
	for(int i = 0; i < app_list.size(); i++)
	{
		QPixmap app_icon_image;
		app_icon_image.loadFromData((const uchar*)g_bytes_get_data(app_list[i].get_app_icon_bytes(), NULL), 
									g_bytes_get_size(app_list[i].get_app_icon_bytes()));
		if(app_list[i].is_system_app())
		{
			table_system_app_model->setItem(item_checkbox_system_app.size(), 0, 
											new QStandardItem(QIcon(app_icon_image),
											QString::fromStdString(app_list[i].get_app_name())));
			table_system_app_model->setItem(item_checkbox_system_app.size(), 1, 
											new QStandardItem(QString::fromStdString(app_list[i].get_app_version())));
			table_system_app_model->setItem(item_checkbox_system_app.size(), 2,
											new QStandardItem(get_app_size_str(app_list[i].get_app_size())));
			item_checkbox_system_app.push_back(new QCheckBox(this));
			ui->table_system_app->setIndexWidget(table_system_app_model->index(item_checkbox_system_app.size() - 1, 3),
												 item_checkbox_system_app[item_checkbox_system_app.size() - 1]);
			item_app_point_system_app.push_back(i);
		} else{
			table_user_app_model->setItem(item_checkbox_user_app.size(), 0, 
											new QStandardItem(QIcon(app_icon_image),
												QString::fromStdString(app_list[i].get_app_name())));
			table_user_app_model->setItem(item_checkbox_user_app.size(), 1, 
											new QStandardItem(QString::fromStdString(app_list[i].get_app_version())));
			table_user_app_model->setItem(item_checkbox_user_app.size(), 2,
											new QStandardItem(get_app_size_str(app_list[i].get_app_size())));
			item_checkbox_user_app.push_back(new QCheckBox(this));
			ui->table_user_app->setIndexWidget(table_user_app_model->index(item_checkbox_user_app.size() - 1, 3),
												 item_checkbox_user_app[item_checkbox_user_app.size() - 1]);
			item_app_point_user_app.push_back(i);
		}
		ui->table_user_app->setColumnWidth(0, (ui->table_user_app->width() - 50) / 6 * 3);
		ui->table_user_app->setColumnWidth(1, (ui->table_user_app->width() - 50) / 6 * 2);
		ui->table_user_app->setColumnWidth(2, (ui->table_user_app->width() - 50) / 6 * 1);
		ui->table_user_app->setColumnWidth(3, 35);
		ui->table_system_app->setColumnWidth(0, (ui->table_user_app->width() - 50) / 6 * 3);
		ui->table_system_app->setColumnWidth(1, (ui->table_user_app->width() - 50) / 6 * 2);
		ui->table_system_app->setColumnWidth(2, (ui->table_user_app->width() - 50) / 6 * 1);
		ui->table_system_app->setColumnWidth(3, 35);
	}
}

void AppManagementWindow::on_button_apk_browse_click()
{
	QString file_path = QFileDialog::getOpenFileName(this, "浏览APK文件", "", "APK File(*.apk)");
	if(!file_path.isNull() && !file_path.isEmpty())
		ui->lineedit_apk_file_path->setText(file_path);
}

void AppManagementWindow::showEvent(QShowEvent* e)
{
	QWidget::showEvent(e);
	exec_scan_app_task();
}

void AppManagementWindow::on_checkbox_select_all_user_click(int state)
{
	for(int i = 0; i < item_checkbox_user_app.size(); i++)
		item_checkbox_user_app[i]->setCheckState((Qt::CheckState)state);
}

void AppManagementWindow::on_checkbox_select_all_system_click(int state)
{
	for(int i = 0; i < item_checkbox_system_app.size(); i++)
		item_checkbox_system_app[i]->setCheckState((Qt::CheckState)state);
}

void AppManagementWindow::exec_backup_app_task(vector<int>& index_list, QString& output_dir)
{
	BackupAppThread* thread = new BackupAppThread(adb_tools, app_list, index_list, output_dir);
	QObject::connect(thread, SIGNAL(backup_complete(QString,bool,QString,bool)),
					this, SLOT(on_backup_app_complete(QString,bool,QString,bool)));
	ui->button_backup_system->setEnabled(false);
	ui->button_backup_user->setEnabled(false);
	ui->button_uninstall_user->setEnabled(false);
	thread->start();
}

void AppManagementWindow::exec_uninstall_app_task(vector< int >& index_list)
{
	UninstallAppThread* thread = new UninstallAppThread(adb_tools, app_list, index_list);
	QObject::connect(thread, SIGNAL(uninstall_complete(QString,bool,QString,bool)),
					 this, SLOT(on_uninstall_complete(QString,bool,QString,bool)));
	ui->button_backup_system->setEnabled(false);
	ui->button_backup_user->setEnabled(false);
	ui->button_uninstall_user->setEnabled(false);
	thread->start();
}

void AppManagementWindow::exec_install_app_task(QString& apk_path)
{
	InstallAppThread* thread = new InstallAppThread(adb_tools, apk_path);
	QObject::connect(thread, SIGNAL(install_complete(QString,bool)), this, SLOT(on_install_complete(QString,bool)));
	ui->button_install_apk->setEnabled(false);
	thread->start();
}

void AppManagementWindow::on_backup_app_complete(QString current_app, bool backup_success, QString next_app, bool finish)
{
	if(!backup_success){
		QMessageBox::critical(this, "错误", QString("备份应用 ") + current_app + " 失败。");
	}
	if(finish){
		ui->status_bar->showMessage("应用备份结束");
		ui->button_backup_system->setEnabled(true);
		ui->button_backup_user->setEnabled(true);
		ui->button_uninstall_user->setEnabled(true);
		return;
	}
	ui->status_bar->showMessage(QString("正在备份应用") + next_app + "...");
}

void AppManagementWindow::on_uninstall_complete(QString current_app, bool uninstall_success, QString next_app, bool finish)
{
	if(!uninstall_success){
		QMessageBox::critical(this, "错误", QString("卸载应用 ") + current_app + " 失败。");
	}
	if(finish){
		ui->status_bar->showMessage("应用卸载结束");
		ui->button_backup_system->setEnabled(true);
		ui->button_backup_user->setEnabled(true);
		ui->button_uninstall_user->setEnabled(true);
		exec_scan_app_task();
		return;
	}
	ui->status_bar->showMessage(QString("正在卸载应用") + next_app + "...");
}

void AppManagementWindow::on_install_complete(QString apk_file, bool success)
{
	if(!success)
	{
		QMessageBox::critical(this, "错误", "apk文件" + apk_file + "安装失败。");
		ui->status_bar->showMessage("应用安装失败");
	}
	else
		ui->status_bar->showMessage("应用安装成功");
	ui->button_install_apk->setEnabled(true);
}

void AppManagementWindow::on_button_backup_user_click()
{
	vector<int> index_list;
	for(int i = 0; i < item_checkbox_user_app.size(); i++)
	{
		if(item_checkbox_user_app[i]->checkState() == Qt::Checked)
			index_list.push_back(item_app_point_user_app[i]);
	}
	if(!index_list.empty()){
		QString output_dir = QFileDialog::getExistingDirectory();
		if(!output_dir.isNull() && !output_dir.isEmpty())
			exec_backup_app_task(index_list, output_dir);
	}
}

void AppManagementWindow::on_button_backup_system_click()
{
	vector<int> index_list;
	for(int i = 0; i < item_checkbox_system_app.size(); i++)
	{
		if(item_checkbox_system_app[i]->checkState() == Qt::Checked)
			index_list.push_back(item_app_point_system_app[i]);
	}
	if(!index_list.empty()){
		QString output_dir = QFileDialog::getExistingDirectory();
		if(!output_dir.isNull() && !output_dir.isEmpty())
			exec_backup_app_task(index_list, output_dir);
	}
}

void AppManagementWindow::on_button_uninstall_user_click()
{
	vector<int> index_list;
	for(int i = 0; i < item_checkbox_user_app.size(); i++)
	{
		if(item_checkbox_user_app[i]->checkState() == Qt::Checked)
			index_list.push_back(item_app_point_user_app[i]);
	}
	if(!index_list.empty()){
		if(QMessageBox::question(this, "确认", "确定卸载所选应用？") == QMessageBox::Yes)
			exec_uninstall_app_task(index_list);
	}
}

void AppManagementWindow::on_button_install_click()
{
	QString apk_file = ui->lineedit_apk_file_path->text();
	if(!apk_file.isNull() && !apk_file.isEmpty())
		exec_install_app_task(apk_file);
}

void ScanAppThread::run()
{
	emit scan_complete(adb_tools->get_app_list(*app_list));
}

void BackupAppThread::run()
{
	emit backup_complete("", true, QString::fromStdString(app_list[index_list[0]].get_app_name()), false);
	for(int i = 0; i < index_list.size(); i++)
	{
		string pkg_name = app_list[index_list[i]].get_app_package_name();
		string app_name = app_list[index_list[i]].get_app_name();
		string output_path = output_dir.toStdString() + '/' + app_name + ".apk";
		if(!adb_tools->get_app_apk_file(pkg_name, output_path))
		{
			emit backup_complete(QString::fromStdString(app_name), false, "", true);
			return;
		}
		else if(i < index_list.size() - 1)
			emit backup_complete(QString::fromStdString(app_name), true, 
								 QString::fromStdString(app_list[index_list[i]].get_app_name()), false);
		else
			emit backup_complete(QString::fromStdString(app_name), true, 
								 QString::fromStdString(app_list[index_list[i]].get_app_name()), true);
	}
}

void UninstallAppThread::run()
{
	emit uninstall_complete("", true, QString::fromStdString(app_list[index_list[0]].get_app_name()), false);
	for(int i = 0; i < index_list.size(); i++)
	{
		string pkg_name = app_list[index_list[i]].get_app_package_name();
		string app_name = app_list[index_list[i]].get_app_name();
		if(!adb_tools->uninstall_apk(pkg_name))
		{
			emit uninstall_complete(QString::fromStdString(app_name), false, "", true);
			return;
		}
		else if(i < index_list.size() - 1)
			emit uninstall_complete(QString::fromStdString(app_name), true, 
								 QString::fromStdString(app_list[index_list[i]].get_app_name()), false);
		else
			emit uninstall_complete(QString::fromStdString(app_name), true, 
								 QString::fromStdString(app_list[index_list[i]].get_app_name()), true);
	}
}

void InstallAppThread::run()
{
	emit install_complete(apk_path, adb_tools->install_apk(apk_path.toStdString()));
}

