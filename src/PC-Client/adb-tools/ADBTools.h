#ifndef ADBTOOLS_H
#define ADBTOOLS_H

#include <string>
#include <glib.h>
#include <vector>
#include "../info-provider/ContactInfo.h"
#include "../info-provider/SMSInfo.h"
#include "../info-provider/AppInfo.h"

using namespace std;

class ADBTools
{
    private:
        static const int ANDROID_SERVER_PORT = 42687;
        static const int STARTUP_TIME_OUT = 15;
        static const char* ADB_LOG_FILE;
        static const char* BUILD_PROP_PATH;
        static const char* MESSAGE_READ_CONTACTS;
        static const char* MESSAGE_READ_SMS;
        static const char* MESSAGE_READ_APP_INFO;

        bool is_running;
        bool connected_flag;
        int connect_socket;
        GCond task_cond;

        static void exec_adb_server_startup(ADBTools* data);
        static void exec_socket_daemon(ADBTools* data);
        static string parse_value(const string& key_val_pair);
        static string parse_key(const string& key_val_pair);
    public:
        static const char* ADB_PATH;
        enum ADBStartError{
            ADB_START_SUCCESSFULLY,
            ADB_START_TIMEOUT,
            ADB_START_PORT_UNAVAILABLE,
            ADB_START_UNKNOWN_ERROR
        };

        ADBTools();
        ADBStartError start_adb_server();
        void stop_adb_server();
        string get_phone_manufacturer();
        string get_phone_model();

        bool is_run()
        {
            return is_running;
        }

        bool is_connected()
        {
            return connected_flag;
        }

        bool install_apk(const char* apk_file_path);
        bool connect_to_phone();
        bool get_contacts_list(vector<ContactInfo>& contacts_list);
        bool get_sms_list(vector<SMSInfo>& sms_list);
        bool get_app_list(vector<AppInfo>& app_list);
};

#endif // ADBTOOLS_H
