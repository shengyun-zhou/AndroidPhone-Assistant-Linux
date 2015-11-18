#include "GUI/MainWindow.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	ADBTools* adb_tools = new ADBTools();
	ADBTools::ADBStartError start_status = adb_tools->start_adb_server();
	if(start_status == ADBTools::ADB_START_PORT_UNAVAILABLE)
	{
		QMessageBox::critical(NULL, "Error", "ADB Server启动失败，请手动关闭其它占用本机5037端口的程序后在重试。");
		return 1;
	}
	else if(start_status != ADBTools::ADB_START_SUCCESSFULLY)
	{
		QMessageBox::critical(NULL, "Error", "ADB Server启动失败。");
		return 1;
	}
	MainWindow main_window(adb_tools);
	main_window.show();
    return application.exec();
}
