#ifndef APPINFO_H
#define APPINFO_H

#include <string>
#include <glib.h>

using namespace std;

class AppInfo {
private:
    /**
     * @brief 应用名称
     */
    string app_name;

    /**
     * @brief 应用版本号
     */
    string app_version;

    /**
     * @brief 应用包名
     */
    string app_package_name;

    /**
     * @brief 该应用是否为系统应用
     */
    bool app_system_flag;

    /**
     * @brief 应用的图标，大部分应用的图标为png格式。在此以字节数组的形式存放于内存中
     *
     * 该字节数组的内容不可修改
     * 可以用g_bytes_get_size(GBytes* array)函数得到该数组的长度
     * 可以用g_bytes_get_data(GBytes* array, NULL)函数得到指向该数组首元素的指针
     */
    GBytes *app_icon_bytes;

    /**
     * @brief 应用大小，以byte为单位
     */
    int64_t app_size;
public:
    AppInfo(const string &app_name, const string &app_version, const string &app_package_name,
            bool is_system_app, GBytes *app_icon_bytes, int64_t app_size) {
        this->app_name = app_name;
        this->app_version = app_version;
        this->app_package_name = app_package_name;
        this->app_icon_bytes = app_icon_bytes;
        this->app_system_flag = is_system_app;
        this->app_size = app_size;
    }

    AppInfo(const AppInfo &app_info) {
        this->app_name = app_info.app_name;
        this->app_version = app_info.app_version;
        this->app_package_name = app_info.app_package_name;
        if (app_info.app_icon_bytes != NULL) {
            this->app_icon_bytes = g_bytes_new_from_bytes(app_info.app_icon_bytes, 0,
                                                          g_bytes_get_size(app_info.app_icon_bytes));
        } else {
            this->app_icon_bytes = NULL;
        }
        this->app_system_flag = app_info.app_system_flag;
        this->app_size = app_info.app_size;
    }

    virtual ~AppInfo() {
        g_bytes_unref(app_icon_bytes);
    }

    string get_app_name() {
        return app_name;
    }

    string get_app_version() {
        return app_version;
    }

    string get_app_package_name() {
        return app_package_name;
    }

    bool is_system_app() {
        return app_system_flag;
    }

    GBytes *get_app_icon_bytes() {
        return app_icon_bytes;
    }

    int64_t get_app_size() {
        return app_size;
    }
};

#endif // APPINFO_H
