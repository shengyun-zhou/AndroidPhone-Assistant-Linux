#include "MainWindow.h"
#include <QApplication>
#include <QDesktopWidget>

void MainWindow::exec_action_exit()
{
    this->close();
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //窗体移动到屏幕正中央
    QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width())/2, (pDesk->height() - height())/2);
	
    //将退出菜单项连接到鼠标点击信号(相当于添加鼠标点击事件监听器)
    QObject::connect(ui->action_exit, SIGNAL(triggered()), this, SLOT(exec_action_exit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
