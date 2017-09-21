#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include <string>
#include <sys/types.h>

using namespace std;

class ContactInfo {
private:
    /**
     * @brief 联系人ID，经实际测试，该数据并没有什么实际意义，可忽略
     */
    int64_t contact_id;

    /**
     * @brief 联系人姓名
     */
    string display_name;

    /**
     * @brief 联系人电话
     */
    string phone_number;
public:
    ContactInfo(int64_t contact_id, const string &display_name, const string &phone_number) {
        this->contact_id = contact_id;
        this->display_name = display_name;
        this->phone_number = phone_number;
    }

    int64_t get_contact_id() {
        return contact_id;
    }

    string get_display_name() {
        return display_name;
    }

    string get_phone_number() {
        return phone_number;
    }
};

#endif // CONTACTINFO_H
