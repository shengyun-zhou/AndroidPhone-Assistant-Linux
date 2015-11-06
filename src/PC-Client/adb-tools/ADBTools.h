#ifndef ADBTOOLS_H
#define ADBTOOLS_H

#include <glib.h>
#include <string>

using namespace std;

class ADBTools
{
    private:
        static const int ANDROID_SERVER_PORT = 42687;
        static const int STARTUP_TIME_OUT = 15;
        static const char* ADB_LOG_FILE;
        static const char* BUILD_PROP_PATH;

        bool is_running;
        bool connected_flag;
        GThread* task_thread;
        GCond task_cond;
        GMutex task_mutex;
        int connect_socket;

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
};

#endif // ADBTOOLS_H
