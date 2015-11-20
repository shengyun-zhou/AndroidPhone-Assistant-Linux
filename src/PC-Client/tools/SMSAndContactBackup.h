#ifndef SMSANDCONTACTBACKUP_H
#define SMSANDCONTACTBACKUP_H
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "sqlite3.h"
#include "../info-provider/SMSInfo.h"
#include "../info-provider/ContactInfo.h"
class SMSAndContactBackup
{
    public:
		static bool export_contact(const char* pdbName, vector<ContactInfo>& contact_Vector);
		static bool export_sms(const char* pdbName, vector<SMSInfo>& sms_Vector);		
};

#endif 
