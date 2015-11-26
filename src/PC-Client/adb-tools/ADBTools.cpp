#include "ADBTools.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../tools/CommandTools.h"
#include "../tools/SocketTools.h"
#include <unistd.h>
#include <fcntl.h>
#define MAX_SIZE 1024

using namespace std;

const char* ADBTools::ADB_PATH  = "./AndroidTools/adb";
const char* ADBTools::ANDROID_SERVER_APK_PATH = "./AndroidTools/PhoneAssistant.apk";
const char* ADBTools::BUILD_PROP_PATH = "/system/build.prop";
const char* ADBTools::MESSAGE_READ_CONTACTS = "msg:read_contacts";
const char* ADBTools::MESSAGE_READ_SMS = "msg:read_sms";
const char* ADBTools::MESSAGE_READ_APP_INFO = "msg:read_app_info";
const char* ADBTools::MESSAGE_HEADER_GET_APP_APK_FILE = "msg:get_app_apk_file";

const char* ADBTools::ANDROID_SERVER_PACKAGE_NAME = "com.buptsse.zero.phoneassistant";
const char* ADBTools::ANDROID_SERVER_MAIN_ACTIVITY = "MainActivity";

ADBTools::ADBTools()
{
    is_running = false;
    connected_flag = false;
}

string ADBTools::parse_value(const string& key_val_pair)
{
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
	if(!data->is_root) {
		if(system(startup_command) == 0)
			data->is_running = true;
	}else {
		CommandTools::SUDO_COMMAND_EXEC_RESULT exec_result;
		exec_result = CommandTools::exec_sudo_command(startup_command, data->root_password.c_str());
		if(exec_result == CommandTools::COMMAND_EXEC_WRONG_PASSWORD)
			data->is_wrong_password = true;
		else if(exec_result == CommandTools::COMMAND_EXEC_SUCCESSFUL)
			data->is_running = true;
	}
    g_cond_signal(&data->task_cond);
}

ADBTools::ADBStartError ADBTools::start_adb_server(bool is_root, const string& root_password)
{
	this->is_root = is_root;
	this->root_password = root_password;
	is_wrong_password = false;
	if(is_running)
		stop_adb_server();
    GMutex task_mutex;
    g_mutex_init(&task_mutex);
    g_cond_init(&task_cond);
    GThread* task_thread = g_thread_new("ADB-startup", (GThreadFunc)exec_adb_server_startup, this);
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

	if(is_wrong_password)
		return ADB_START_ROOT_WRONG_PASSWORD;
	
    if(!SocketTools::is_local_port_available(5037)){
        printf("5037 port is unavailable.\n");
        return ADB_START_PORT_UNAVAILABLE;
    }
    return ADB_START_UNKNOWN_ERROR;
}

void ADBTools::stop_adb_server()
{
    if(connected_flag)
        close(connect_socket);
    char command[MAX_SIZE];
    sprintf(command, "%s kill-server", ADB_PATH);
    system(command);
	fprintf(stderr, "ADB Server has stopped.\n");
    is_running = false;
    connected_flag = false;
}

bool ADBTools::install_apk(const string& apk_file_path)
{
    if(!is_running)
        return false;
    char install_command[MAX_SIZE];
    sprintf(install_command, "%s %s '%s'", ADB_PATH, "install", apk_file_path.c_str());
    if(system(install_command) == 0)
        return true;
    return false;
}

bool ADBTools::uninstall_apk(const string& app_package_name)
{
	if(!is_running)
		return false;
	char uninstall_command[MAX_SIZE];
		sprintf(uninstall_command, "%s %s %s", ADB_PATH, "uninstall", app_package_name.c_str());
    if(system(uninstall_command) == 0)
		return true;
    return false;
}

string ADBTools::get_phone_manufacturer()
{
    if(!is_running)
        return "";
    vector<string> result;
    char command[MAX_SIZE];
    sprintf(command, "%s shell 'cat %s|grep ro.product.manufacturer'", ADB_PATH, BUILD_PROP_PATH);
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
    if(!is_running)
        return false;
    if(connected_flag)
        return true;
    int i;
    for(i = 20000; i <= 65535; i++)
    {
        if(!SocketTools::is_local_port_available(i))
            continue;
        printf("Available port:%d\n", i);
        break;
    }
    if(i > 65535)
         return false;
	
    char start_activity_cmd[MAX_SIZE];
    sprintf(start_activity_cmd, "am start -n '%s/%s.%s' -a android.intent.action.MAIN -c android.intent.category.LAUNCHER --ez start-service true",
	    ANDROID_SERVER_PACKAGE_NAME, ANDROID_SERVER_PACKAGE_NAME, ANDROID_SERVER_MAIN_ACTIVITY);
    
    if(!CommandTools::exec_adb_shell_command(ADB_PATH, start_activity_cmd))
    {
		if(!install_apk(ANDROID_SERVER_APK_PATH))
		{
			uninstall_apk(ANDROID_SERVER_PACKAGE_NAME);
			if(!install_apk(ANDROID_SERVER_APK_PATH))
				return false;
			if(!CommandTools::exec_adb_shell_command(ADB_PATH, start_activity_cmd))
				return false;
		}
		if(!CommandTools::exec_adb_shell_command(ADB_PATH, start_activity_cmd))
			return false;
    }
    
    g_usleep(200 * 1000);
	
	connect_socket = SocketTools::create_socket();
    if(connect_socket < 0)
		return false;
	connect_socket = dup(connect_socket);
	
	char command[MAX_SIZE];
    sprintf(command, "%s forward tcp:%d tcp:%d", ADB_PATH, i, ANDROID_SERVER_PORT);
    if(system(command) != 0)
	{
		close(connect_socket);
		return false;
	}
	
	if(!SocketTools::connect_to_server(connect_socket, "127.0.0.1", i))
	{
		close(connect_socket);
		return false;
	}
	
    if(!SocketTools::send_urgent_data(connect_socket))
	{
		close(connect_socket);
		return false;
	}
	
    connected_flag = true;
    g_thread_new("Phone-Assistant-Daemon", (GThreadFunc)exec_socket_daemon, this);
    return true;
}

void ADBTools::disconnect_from_phone()
{
	if(!is_running || !connected_flag)
		return;
	close(connect_socket);
	connected_flag = false;
}

void ADBTools::exec_socket_daemon(ADBTools* data)
{
    while(true)
    {
        if(!SocketTools::send_urgent_data(data->connect_socket))
        {
            close(data->connect_socket);
            data->connected_flag = false;
            return;
        }
        sleep(2);
    }
}

bool ADBTools::get_contacts_list(vector<ContactInfo>& contacts_list)
{
    if(!is_running || !connected_flag)
        return false;
    if(!SocketTools::send_msg(connect_socket, MESSAGE_READ_CONTACTS))
        return false;
    string str_list_size;
    if(!SocketTools::receive_msg(connect_socket, str_list_size))
        return false;
    if(!(parse_key(str_list_size) == "ContactListSize"))
        return false;
    int list_size = atoi(parse_value(str_list_size).c_str());
    printf("Total %d records in the contacts list.\n", list_size);
    contacts_list.clear();
    int i, j;
    for(i = 0; i < list_size; i++)
    {
        string display_name = "";
        string phone_number = "";
        int64_t contact_id = -1;
        for(j = 1; j <= 3; j++)
        {
            string buf, key;
            if(!SocketTools::receive_msg(connect_socket, buf))
                return false;
            key = parse_key(buf);
            if(key == "ContactID")
                contact_id = atoll(parse_value(buf).c_str());
            else if(key == "DisplayName")
                display_name = parse_value(buf);
            else if(key == "PhoneNumber")
                phone_number = parse_value(buf);
        }
        if(contact_id < 0)
            return false;
        //printf("contact id=%lld,display name=%s,phone number=%s\n", contact_id, display_name.c_str(), phone_number.c_str());
        printf("Received %d records.\n", i + 1);
        contacts_list.push_back(ContactInfo(contact_id, display_name, phone_number));
    }
    return true;
}

bool ADBTools::get_sms_list(vector<SMSInfo>& sms_list)
{
    if(!is_running || !connected_flag)
        return false;
    if(!SocketTools::send_msg(connect_socket, MESSAGE_READ_SMS))
        return false;
    string str_list_size;
    if(!SocketTools::receive_msg(connect_socket, str_list_size))
        return false;
    if(!(parse_key(str_list_size) == "SMSListSize"))
        return false;
    int list_size = atoi(parse_value(str_list_size).c_str());
    printf("Total %d records in the SMS list.\n", list_size);
    sms_list.clear();
    int i, j;
    for(i = 0; i < list_size; i++)
    {
        string sms_body = "";
        string phone_number = "";
        int64_t date = -1;
		string date_str = "";
        int sms_type = 1;

        for(j = 1; j <= 5; j++)
        {
            string buf, key;
            if(!SocketTools::receive_msg(connect_socket, buf))
                return false;
            key = parse_key(buf);
            if(key == "Date")
                date = atoll(parse_value(buf).c_str());
			else if(key == "DateStr")
				date_str = parse_value(buf);
            else if(key == "PhoneNumber")
                phone_number = parse_value(buf);
            else if(key == "SMSBody")
                sms_body = parse_value(buf);
            else if(key == "SMSType")
                sms_type = atoi(parse_value(buf).c_str());
        }
        if(date < 0)
            return false;
        //printf("date:%s\nphone number:%s\nSMS body:%s\n", date_str.c_str(), phone_number.c_str(), sms_body.c_str());
        printf("Received %d short messages.\n", i + 1);
        sms_list.push_back(SMSInfo(date, date_str, phone_number, sms_body, sms_type));
    }
    sort_sms_list(sms_list);
    return true;
}

int quicksort_sms_list_division(vector<SMSInfo>& sms_list, int left, int right)
{
	SMSInfo base = sms_list[left];
	while(left < right)
	{
		while(left < right && sms_list[right].get_date() >= base.get_date())
			right--;
		sms_list[left] = sms_list[right];
		while(left < right && sms_list[left].get_date() <= base.get_date())
			left++;
		sms_list[right] = sms_list[left];
	}
	sms_list[left] = base;
	return left;
}

void quicksort_sms_list(vector<SMSInfo>& sms_list, int left, int right)
{
	if(left < right)
	{
		int i = quicksort_sms_list_division(sms_list, left, right);
		quicksort_sms_list(sms_list, left, i - 1);
		quicksort_sms_list(sms_list, i + 1, right);
	}
}

void ADBTools::sort_sms_list(vector<SMSInfo>& sms_list)
{
	quicksort_sms_list(sms_list, 0, sms_list.size() - 1);
}

bool ADBTools::get_app_list(vector<AppInfo>& app_list)
{
    if(!is_running || !connected_flag)
        return false;
    if(!SocketTools::send_msg(connect_socket, MESSAGE_READ_APP_INFO))
        return false;
    string str_list_size;
    if(!SocketTools::receive_msg(connect_socket, str_list_size))
        return false;
    if(!(parse_key(str_list_size) == "AppListSize"))
        return false;
    int list_size = atoi(parse_value(str_list_size).c_str());
    printf("Total %d apps in the phone.\n", list_size);
    app_list.clear();
    int i, j;
    for(i = 0; i < list_size; i++)
    {
        string app_name = "";
        string app_version = "";
        string app_package = "";
        bool app_system_flag = false;
        GBytes* app_icon_bytes = NULL;
        int64_t app_size = 0;

        for(j = 1; j <= 6; j++)
        {
            string buf, key;
            if(!SocketTools::receive_msg(connect_socket, buf))
                return false;
            key = parse_key(buf);
            if(key == "AppName")
                app_name = parse_value(buf);
            else if(key == "AppPackage")
                app_package = parse_value(buf);
            else if(key == "AppVersion")
                app_version = parse_value(buf);
            else if(key == "AppSystemFlag") {
                string value = parse_value(buf);
                if(value == "true")
                    app_system_flag = true;
                else
                    app_system_flag = false;
            }
            else if(key == "AppIconBytesLength") {
                int bytes_length = atoi(parse_value(buf).c_str());;
                if(bytes_length <= 0)
                    continue;
                if(!SocketTools::receive_bytes(connect_socket, &app_icon_bytes, bytes_length))
                    return false;
            }
            else if(key == "AppSize") {
                app_size = atoll(parse_value(buf).c_str());
            }
        }
        if(app_package.length() <= 0)
            return false;
        printf("app name:%s\napp version:%s\napp package:%s\nsystem app=%d\napp size=%ld\n",
                app_name.c_str(), app_version.c_str(), app_package.c_str(), app_system_flag, app_size);
        printf("Received %d app info.\n", i + 1);
        /*if(app_icon_bytes != NULL) {
            int png_file = open((app_package + ".png").c_str(), O_WRONLY | O_CREAT, 0664);
            write(png_file, g_bytes_get_data(app_icon_bytes, NULL), g_bytes_get_size(app_icon_bytes));
            close(png_file);
        }*/
        app_list.push_back(AppInfo(app_name, app_version, app_package, app_system_flag, app_icon_bytes, app_size));
    }
    return true;
}

bool ADBTools::get_app_apk_file(const string& app_package_name, const string& file_path)
{
    if(!is_running || !connected_flag)
        return false;
    if(!SocketTools::send_msg(connect_socket, string(MESSAGE_HEADER_GET_APP_APK_FILE) + ':' + app_package_name))
        return false;
    string str_file_length;
    if(!SocketTools::receive_msg(connect_socket, str_file_length))
        return false;
    if(!(parse_key(str_file_length) == "FileLength"))
        return false;
    int64_t file_length = atoll(parse_value(str_file_length).c_str());
    if(file_length <= 0)
        return false;
    return SocketTools::receive_file(connect_socket, file_path, file_length);
}
