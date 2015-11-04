#include "ADBTools.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "../tools/CommandTools.h"
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
    vector<string> test;
    CommandTools::exec_command(startup_command, test);
    g_cond_signal(&data->task_cond);
}

ADBTools::ADBStartError ADBTools::start_adb_service()
{
    task_thread = g_thread_new("ADB-startup", (GThreadFunc)exec_adb_service_startup, this);
    g_mutex_init(&task_mutex);
    g_cond_init(&task_cond);
    gboolean ret = g_cond_wait_until(&task_cond, &task_mutex, g_get_monotonic_time() + STARTUP_TIME_OUT * G_TIME_SPAN_SECOND);
    g_thread_unref(task_thread);
    task_thread = NULL;
    if(!ret)
    {
        printf("ADB Server startup timeout.\n");
        return ADB_START_TIMEOUT;
    }
    else if(is_running)
        return ADB_START_SUCCESSFULLY;

    int test_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(test_socket < 0)
        return ADB_START_UNKNOWN_ERROR;
    sockaddr_in test_addr;
    test_addr.sin_family = AF_INET;
    test_addr.sin_port = htons(5037);
    test_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    int bind_ret = bind(test_socket, (sockaddr*)&test_addr, sizeof(test_addr));
    close(test_socket);
    if(bind_ret < 0){
        printf("5037 port is unavailable.\n");
        return ADB_START_PORT_UNAVAILABLE;
    }
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


