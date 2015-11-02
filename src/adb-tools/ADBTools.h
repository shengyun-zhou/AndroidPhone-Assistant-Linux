#ifndef ADBTOOLS_H
#define ADBTOOLS_H

#include <glib.h>

class ADBTools
{
    private:
        bool is_running;
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
};

#endif // ADBTOOLS_H
