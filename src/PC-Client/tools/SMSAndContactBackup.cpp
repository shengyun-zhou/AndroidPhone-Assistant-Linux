#include "SMSAndContactBackup.h"
#include <unistd.h>
#include <sys/stat.h>

#define MAX_SIZE 4096

bool SMSAndContactBackup::export_sms(const char *pdbName, vector<SMSInfo> &sms_Vector) {
    sqlite3 *db = NULL;
    char *zErrMsg = NULL;
    int rc;
    int result;
    const char *sSQL1 =
            "create table SMS(date long primary key, phone_number varchar(30), sms_body varchar(1000));";
    //打开指定的数据库文件,如果不存在将创建一个同名的数据库文件
    if (access(pdbName, F_OK | W_OK) == 0)
        unlink(pdbName);
    rc = sqlite3_open(pdbName, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s/n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    sqlite3_exec(db, sSQL1, 0, 0, &zErrMsg);
    //插入数据
    char insert_sql[MAX_SIZE];
    for (int i = 0; i < sms_Vector.size(); i++) {
        sprintf(insert_sql, "insert into SMS values(%ld,'%s','%s');", sms_Vector[i].get_date(),
                sms_Vector[i].get_phone_number().c_str(), sms_Vector[i].get_sms_body().c_str());
        sqlite3_exec(db, insert_sql, NULL, NULL, &zErrMsg);
    }
    sqlite3_close(db);
    return true;
}

bool SMSAndContactBackup::export_contact(const char *pdbName, vector<ContactInfo> &contact_Vector) {
    sqlite3 *db = NULL;
    char *zErrMsg = 0;
    int rc;
    int result;
    const char *sSQL2 =
            "create table Contact(display_name varchar(50), phone_number varchar(50));";
    //打开指定的数据库文件,如果不存在将创建一个同名的数据库文件

    if (access(pdbName, F_OK | W_OK) == 0)
        unlink(pdbName);
    rc = sqlite3_open(pdbName, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s/n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }
    sqlite3_exec(db, sSQL2, 0, 0, &zErrMsg);
    char insert_sql[MAX_SIZE];
    //插入数据
    for (int i = 0; i < contact_Vector.size(); i++) {
        sprintf(insert_sql, "insert into Contact values('%s','%s');", contact_Vector[i].get_display_name().c_str(),
                contact_Vector[i].get_phone_number().c_str());
        sqlite3_exec(db, insert_sql, 0, 0, &zErrMsg);
    }
    return true;
}