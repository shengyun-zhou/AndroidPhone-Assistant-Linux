#include "MainWindow.h"
#include <QApplication>
#include <QDesktopWidget>

void MainWindow::exec_action_exit()
{
    this->destroy();
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //窗体移动到屏幕正中央
    QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width())/2, (pDesk->height() - height())/2);

    ui->action_exit->connect(ui->action_exit, SIGNAL(triggered()), this, SLOT(MainWindow::exec_action_exit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
