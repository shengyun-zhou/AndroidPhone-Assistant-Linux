#include "ADBTools.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define MAX_SIZE 1024

const char* ADBTools::ADB_PATH  = "./AndroidTools/adb";

ADBTools::ADBTools()
{
    is_running = false;
    task_thread = NULL;
}

void ADBTools::exec_adb_service_startup(ADBTools* data)
{
    char startup_command[MAX_SIZE];
    sprintf(startup_command, "%s %s", ADB_PATH, "start-server");
    if(system(startup_command) == 0)
        data->is_running = true;
    data->is_running = false;
    g_cond_signal(&data->task_cond);
}

ADBTools::ADBStartError ADBTools::start_adb_service()
{
    task_thread = g_thread_new("ADBStartupThread", (GThreadFunc)exec_adb_service_startup, this);
    g_mutex_init(&task_mutex);
    g_cond_init(&task_cond);
    gboolean ret = g_cond_wait_until(&task_cond, &task_mutex, g_get_monotonic_time() + STARTUP_TIME_OUT * G_TIME_SPAN_SECOND);
    g_thread_unref(task_thread);
    task_thread = NULL;
    if(!ret)
        return ADB_START_TIMEOUT;
    else if(is_running)
        return ADB_START_SUCCESSFULLY;
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

bool ADBTools::install_apk(const char* apk_file_path)
{
    if(!is_running)
        return false;
    char install_command[MAX_SIZE];
    sprintf(install_command, "%s %s %s", ADB_PATH, "install", apk_file_path);
    if(system(install_command) == 0)
        return true;
    return false;
}


