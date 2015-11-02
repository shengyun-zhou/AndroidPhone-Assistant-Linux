#include "ADBTools.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <glib.h>
#define MAX_SIZE 1024

const char* ADBTools::ADB_PATH  = "./AndroidTools/adb";

ADBTools::ADBTools()
{
    is_running = false;
}

void ADBTools::exec_adb_service_startup(ADBTools* data)
{
    char startup_command[MAX_SIZE];
    sprintf(startup_command, "%s %s", ADB_PATH, "start-server");
    if(system(startup_command) == 0)
    {
        data->is_running = true;
        return ADB_START_SUCCESSFULLY;
    }
    data->is_running = false;
}

ADBTools::ADBStartError ADBTools::start_adb_service()
{
    GThread adb_start_thread = g_thread_create(exec_adb_service_startup, this, TRUE, NULL);
    g_cond_wait_until()

    int test_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(test_socket < 0)
        return ADB_START_UNKNOWN_ERROR;
    sockaddr_in test_addr;
    test_addr.sin_family = AF_INET;
    test_addr.sin_port = htons(5037);
    test_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(bind(test_socket, (sockaddr*)&test_addr, sizeof(test_addr)) < 0)
        return ADB_START_PORT_UNAVAILABLE;
    return ADB_START_UNKNOWN_ERROR;
}
