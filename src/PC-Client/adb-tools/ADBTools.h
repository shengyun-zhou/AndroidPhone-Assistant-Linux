#ifndef ADBTOOLS_H
#define ADBTOOLS_H

#include <string>
#include <glib.h>
#include <vector>
#include "../info-provider/ContactInfo.h"
#include "../info-provider/SMSInfo.h"
#include "../info-provider/AppInfo.h"

using namespace std;

class ADBTools
{
    private:
        static const int ANDROID_SERVER_PORT = 42687;
        static const int STARTUP_TIME_OUT = 15;
        static const char* ADB_LOG_FILE;
        static const char* BUILD_PROP_PATH;
        static const char* MESSAGE_READ_CONTACTS;
        static const char* MESSAGE_READ_SMS;
        static const char* MESSAGE_READ_APP_INFO;
        static const char* MESSAGE_HEADER_GET_APP_APK_FILE;

        bool is_running;
        bool connected_flag;
        int connect_socket;
        GCond task_cond;
		bool is_root;
		string root_password;
		bool is_wrong_password;
		
        static void exec_adb_server_startup(ADBTools* data);
        static void exec_socket_daemon(ADBTools* data);
        static string parse_value(const string& key_val_pair);
        static string parse_key(const string& key_val_pair);
    public:
        static const char* ADB_PATH;
        static const char* ANDROID_SERVER_PACKAGE_NAME;
        static const char* ANDROID_SERVER_MAIN_ACTIVITY;
		static const char* ANDROID_SERVER_APK_PATH;
	
        enum ADBStartError{
            ADB_START_SUCCESSFULLY,				//ADB Server启动成功
            ADB_START_TIMEOUT,						//ADB Server启动超时
            ADB_START_PORT_UNAVAILABLE,			//ADB Server所需的本机5037端口被占用（需要手动关闭其它占用该端口的程序）
            ADB_START_UNKNOWN_ERROR,				//ADB Server启动遇到未知错误
            ADB_START_ROOT_WRONG_PASSWORD			//提权时密码错误
        };

        ADBTools();
		
		/**
		 * @brief 启动ADB Server
		 * @return ADB Server启动结果
		 */
        ADBStartError start_adb_server(bool is_root = false, const string& root_password = "");
		
		/**
		 * @brief 停止ADB Server
		 */
        void stop_adb_server();
		
		/**
		 * @brief 检测ADB Server是否在运行
		 */
		bool is_run()
        {
            return is_running;
        }
		
		/*以下功能无需连接手机端的守护App即可使用*/
		
		/**
		 * @brief 获取手机制造商名称
		 * @return 手机制造商名称，若获取过程中出错或无法获取到信息则返回空字符串
		 */
        string get_phone_manufacturer();
		
		/**
		 * @brief 获取手机型号
		 * @return 手机型号，若获取过程中出错或无法获取到信息则返回空字符串
		 */
        string get_phone_model();
		
		/**
		 * @brief 往手机中安装APK包
		 * @param apk_file_path 要安装的APK文件路径
		 * @return 安装成功返回true
		 * @attention 若手机制造商和型号信息都无法获取，请勿调用此函数！
		 */
		bool install_apk(const string& apk_file_path);
		
		/**
		 * @brief 卸载手机上的某个应用
		 * @param app_package_name 要卸载的应用的包名
		 * @return 卸载成功返回true
		 * @attention 若手机制造商和型号信息都无法获取，请勿调用此函数！
		 */
		bool uninstall_apk(const string& app_package_name);
		
		/**
		 * @brief 与手机端的守护App进行连接，若手机上没安装守护App则会自动安装，安装后再连接
		 * @return 连接成功返回true
		 * @attention 若手机制造商和型号信息都无法获取，请勿调用此函数！
		 */
		bool connect_to_phone();
		
		/**
		 * @brief 与手机端的守护App断开连接
		 */
		void disconnect_from_phone();
		
		/**
		 * @brief 检测是否已经连上手机端的守护App
		 */
        bool is_connected()
        {
            return connected_flag;
        }
        
        /*以下功能均需要连接到手机端的守护App后才能使用*/
        
		/**
		 * @brief 获取手机中的联系人列表
		 * @param [out]contacts_list 获取到的联系人信息存放于此
		 * @return 成功获取所有联系人信息返回true
		 * @attention 若手机端的安全软件对读取联系人信息进行拦截，请选择允许。
		 * @see ContactInfo（PC-Client/info-provider/ContactInfo.h）
		 */
        bool get_contacts_list(vector<ContactInfo>& contacts_list);
		
		/**
		 * @brief 获取手机中所有接收到的短信（有可能获取得到已发出的短信）
		 * @param [out]sms_list 获取到的所有短信存放于此
		 * @return 成功获取所有短信返回true
		 * @attention 若手机端的安全软件对读取短信进行拦截，请选择允许。
		 */
        bool get_sms_list(vector<SMSInfo>& sms_list);
		
		/**
		 * @brief 获取手机中所有已安装的应用（包括系统应用）
		 * @param app_list 获取到的应用信息存放于此
		 * @return 成功获取所有应用信息返回true
		 */
        bool get_app_list(vector<AppInfo>& app_list);
		
		/**
		 * @brief 提取手机中某个应用的APK文件
		 * @param app_package_name 目标应用的包名
		 * @param file_path 提取到的APK文件的输出路径 
		 */
        bool get_app_apk_file(const string& app_package_name, const string& file_path);
};

#endif // ADBTOOLS_H
