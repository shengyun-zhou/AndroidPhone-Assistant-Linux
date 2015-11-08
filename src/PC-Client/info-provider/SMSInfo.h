#ifndef SMSINFO_H
#define SMSINFO_H

#include <stdint.h>
#include <string>

using namespace std;

class SMSInfo
{
    private:
        int64_t date;
        string phone_number;
        string sms_body;
        int type;
    public:
        SMSInfo(int64_t date, const string& phone_number, const string& sms_body, int sms_type)
        {
            this->date = date;
            this->phone_number = phone_number;
            this->sms_body = sms_body;
            this->type = sms_type;
        }

        int64_t get_date()
        {
            return date;
        }

        string get_phone_number()
        {
            return phone_number;
        }

        string get_sms_body()
        {
            return sms_body;
        }

        int get_sms_type()
        {
            return type;
        }
};

#endif // SMSINFO_H
