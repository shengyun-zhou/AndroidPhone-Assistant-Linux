#ifndef ADBTOOLS_H
#define ADBTOOLS_H

#include <glib.h>

class ADBTools
{
    private:
        static const int STARTUP_TIME_OUT = 15;

        bool is_running;
        static void exec_adb_service_startup(ADBTools* data);
        GThread* task_thread;
        GCond task_cond;
        GMutex task_mutex;
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
