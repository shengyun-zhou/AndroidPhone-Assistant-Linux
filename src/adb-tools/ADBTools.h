#ifndef ADBTOOLS_H
#define ADBTOOLS_H

#include <glib.h>
#include <string>

using namespace std;

class ADBTools
{
    private:
        static const int STARTUP_TIME_OUT = 15;
        static const char* ADB_LOG_FILE;
        static const char* BUILD_PROP_PATH;

        bool is_running;
        GThread* task_thread;
        GCond task_cond;
        GMutex task_mutex;

        static void exec_adb_server_startup(ADBTools* data);
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
        string get_phone_manufacturer();
        string get_phone_model();

        bool is_run()
        {
            return is_running;
        }

        bool install_apk(const char* apk_file_path);
};

#endif // ADBTOOLS_H
