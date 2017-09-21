#ifndef SMSBACKUPWINDOW_H
#define SMSBACKUPWINDOW_H

#include <QMainWindow>
#include "UI_SMSBackupWindow.h"
#include <QThread>
#include <QProgressDialog>
#include <QStandardItem>
#include "../adb-tools/ADBTools.h"

class SMSBackupWindow : public QMainWindow {
Q_OBJECT
private:
    Ui::SMSBackupWindow *ui;
    ADBTools *adb_tools;
    vector<SMSInfo> sms_list;
    QStandardItem *root_item;
    QProgressDialog *scan_progress_dialog;

    struct SMSGroup {
        QString phone_number;
        vector<int> sms_index;
    };
    vector<SMSGroup> sms_group_list;

    void scan_sms_list();

    void set_tree_model();

    vector<SMSGroup> sms_list_sort();

protected:
    virtual void showEvent(QShowEvent *);

private slots:

    void on_scan_sms_complete(bool);

    void on_button_backup_sms_click();

    void exec_action_rescan_sms_list();

    void on_tree_view_click(const QModelIndex &);

    void on_tree_view_item_check_change(QStandardItem *);

    void on_sms_backup_complete(bool, const QString &);

public:
    SMSBackupWindow(ADBTools *tools, QWidget *parent = 0);

    virtual ~SMSBackupWindow();
};

class ScanSMSThread : public QThread {
Q_OBJECT
private:
    ADBTools *adb_tools;
    vector<SMSInfo> *sms_list;
public:
    ScanSMSThread(ADBTools *tools, vector<SMSInfo> *sms_list, QObject *parent = 0) : QThread(parent) {
        adb_tools = tools;
        this->sms_list = sms_list;
    }

signals:

    void scan_sms_complete(bool);

protected:
    virtual void run();
};

class SMSBackupThread : public QThread {
Q_OBJECT
private:
    ADBTools *adb_tools;
    vector<SMSInfo> *sms_list;
    string backup_path;
public:
    SMSBackupThread(ADBTools *tools, vector<SMSInfo> *sms_list, const string &backup_path, QObject *parent = 0)
            : QThread(parent) {
        adb_tools = tools;
        this->sms_list = sms_list;
        this->backup_path = backup_path;
    }

signals:

    void backup_sms_complete(bool, QString);

protected:
    virtual void run();
};

#endif // SMSBACKUPWINDOW_H
