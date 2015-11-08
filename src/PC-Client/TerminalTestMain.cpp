#include "adb-tools/ADBTools.h"
#include "tools/PipeTools.h"
#include <stdio.h>
#include <unistd.h>
int main()
{
    PipeTools::init_pipe();
    ADBTools tools;
    if(tools.start_adb_server() == ADBTools::ADB_START_SUCCESSFULLY)
        printf("ADB server start successfully.\n");
    else
    {
        printf("ADB server start failed.\n");
        return 1;
    }
    string phone_manufacturer = tools.get_phone_manufacturer();
    string phone_model = tools.get_phone_model();
    printf("Phone Name:%s %s\n", phone_manufacturer.c_str(), phone_model.c_str());
    if(tools.connect_to_phone())
        printf("Connect to phone successfully.\n");
    vector<ContactInfo> contact_info_list;
    tools.get_contacts_list(contact_info_list);
    vector<SMSInfo> sms_info_list;
    tools.get_sms_list(sms_info_list);
    tools.stop_adb_server();
    return 0;
}
