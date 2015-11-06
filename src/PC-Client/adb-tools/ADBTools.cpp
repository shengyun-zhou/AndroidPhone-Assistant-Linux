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

using namespace std;

const char* ADBTools::ADB_PATH  = "./AndroidTools/adb";
const char* ADBTools::BUILD_PROP_PATH = "/system/build.prop";

ADBTools::ADBTools()
{
    is_running = false;
    connected_flag = false;
    task_thread = NULL;
}

string ADBTools::parse_value(const string& key_val_pair)
{
    //printf("key_value:%s\n", key_val_pair.c_str());
    int start, end;
    int i;
    for(i = key_val_pair.length() - 1; i >= 0 && key_val_pair[i] == ' '; i--);
    end = i;
    for(; i >= 0; i--)
    {
        if(key_val_pair[i] == '=')
        {
            for(start = i + 1; key_val_pair[start] == ' '; start++);
            //printf("value=%s\n", key_val_pair.substr(start, end - start + 1).c_str());
            return key_val_pair.substr(start, end - start + 1);
        }
    }
    return "";
}

string ADBTools::parse_key(const string& key_val_pair)
{
    int start,end;
    int i;
    for(i = 0; i < key_val_pair.length() && key_val_pair[i] == ' '; i++);
    start = i;
    for(; i < key_val_pair.length(); i++)
    {
        if(key_val_pair[i] == '=')
        {
            for(end = i - 1; key_val_pair[end] == ' '; end--);
            return key_val_pair.substr(start, end - start + 1);
        }
    }
    return "";
}

void ADBTools::exec_adb_server_startup(ADBTools* data)
{
    char startup_command[MAX_SIZE];
    sprintf(startup_command, "%s %s", ADB_PATH, "start-server");
    vector<string> test_result;
    if(CommandTools::exec_command(startup_command, test_result))
        data->is_running = true;
    else
        data->is_running = false;
    g_cond_signal(&data->task_cond);
}

ADBTools::ADBStartError ADBTools::start_adb_server()
{
    task_thread = g_thread_new("ADB-startup", (GThreadFunc)exec_adb_server_startup, this);
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

void ADBTools::stop_adb_server()
{
    char command[MAX_SIZE];
    sprintf(command, "%s kill-server", ADB_PATH);
    system(command);
    is_running = false;
    connected_flag = false;
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

string ADBTools::get_phone_manufacturer()
{
    if(!is_running)
        return "";
    vector<string> result;
    char command[MAX_SIZE];
    sprintf(command, "%s shell cat %s|grep ro.product.manufacturer", ADB_PATH, BUILD_PROP_PATH);
    if(!CommandTools::exec_command(command, result))
        return "";
    int i;
    for(i = 0; i < result.size(); i++)
    {
        string key = parse_key(result[i]);
        if(key == "ro.product.manufacturer")
            return parse_value(result[i]);
    }
    return "";
}

string ADBTools::get_phone_model()
{
    if(!is_running)
        return "";
    vector<string> result;
    char command[MAX_SIZE];
    sprintf(command, "%s shell cat %s|grep ro.product.model", ADB_PATH, BUILD_PROP_PATH);
    if(!CommandTools::exec_command(command, result))
        return "";
    int i;
    for(i = 0; i < result.size(); i++)
    {
        string key = parse_key(result[i]);
        if(key == "ro.product.model")
            return parse_value(result[i]);
    }
    return "";
}

bool ADBTools::connect_to_phone()
{
    if(connected_flag)
        return true;
    int i;
    int test_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(test_socket < 0)
        return false;
    sockaddr_in test_addr;
    test_addr.sin_family = AF_INET;
    test_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    for(i = 20000; i <= 65535; i++)
    {
        test_addr.sin_port = htons(i);
        if(bind(test_socket, (sockaddr*)&test_addr, sizeof(test_addr)) < 0)
            continue;
        printf("Available port:%d\n", i);
        close(test_socket);
        break;
    }
    if(i > 65535)
    {
        close(test_socket);
        return false;
    }
    char command[MAX_SIZE];
    sprintf(command, "%s forward tcp:%d tcp:%d", ADB_PATH, i, ANDROID_SERVER_PORT);
    if(system(command) != 0)
        return false;
    connect_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(connect_socket < 0)
    {
        printf("%s\n", strerror(errno));
        return false;
    }
    if(connect(connect_socket, (sockaddr*)&test_addr, sizeof(test_addr)) < 0)
    {
        printf("%s\n", strerror(errno));
        return false;
    }
    if(send(connect_socket, "Hello,Phone!\n", strlen("Hello,Phone!\n"), 0) < 0)
    {
        printf("%s\n", strerror(errno));
        return false;
    }
    char temp[MAX_SIZE];
    int len;
    if((len = recv(connect_socket, temp, sizeof(temp), 0)) < 0)
    {
        printf("%s\n", strerror(errno));
        return false;
    }
    temp[len] = '\0';
    printf("Receive from phone:%s\n", temp);
    return true;
}
