#ifndef SMSINFO_H
#define SMSINFO_H

#include <stdint.h>
#include <string>

using namespace std;

class SMSInfo
{
    private:
		/**
		 * @brief 短信接收时间，以整数表示
		 */
        int64_t date;
		
		/**
		 * @brief 短信接收时间，以字符串表示（格式：yyyy-mm-dd HH:mm:ss）
		 */
		string date_str;
		
		/**
		 * @brief 发信人电话号码
		 */
        string phone_number;
		
		/**
		 * @brief 短信内容
		 */
        string sms_body;
		
		/**
		 * @brief 短信类型 1为收到，2为发出
		 */
        int type;
    public:
        SMSInfo(int64_t date, const string& date_str, const string& phone_number, const string& sms_body, int sms_type)
        {
            this->date = date;
			this->date_str = date_str;
            this->phone_number = phone_number;
            this->sms_body = sms_body;
            this->type = sms_type;
        }

        int64_t get_date()
        {
            return date;
        }

        string get_date_str()
		{
			return date_str;
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
