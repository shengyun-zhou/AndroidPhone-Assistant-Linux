#ifndef ADBTOOLS_H
#define ADBTOOLS_H

#include <glib.h>
#include <string>

class ADBTools
{
    private:
        static const int STARTUP_TIME_OUT = 15;
        static const char* ADB_LOG_FILE;

        bool is_running;
        GThread* task_thread;
        GCond task_cond;
        GMutex task_mutex;

        static void exec_adb_service_startup(ADBTools* data);
    public:
        static const char* ADB_PATH;
        enum ADBStartError{
            ADB_START_SUCCESSFULLY,
            ADB_START_TIMEOUT,
            ADB_START_PORT_UNAVAILABLE,
            ADB_START_UNKNOWN_ERROR
        };

        ADBTools();
        ADBStartError start_adb_service();

        bool is_run()
        {
            return is_running;
        }

        bool install_apk(const char* apk_file_path);
};

#endif // ADBTOOLS_H
