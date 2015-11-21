#include "GUI/MainWindow.h"
#include <QMessageBox>
#include <QProgressDialog>

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	ADBTools* adb_tools = new ADBTools();
	MainWindow main_window(adb_tools);
	main_window.show();
    return application.exec();
}
