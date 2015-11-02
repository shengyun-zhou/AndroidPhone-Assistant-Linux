#include "adb-tools/ADBTools.h"
#include <stdio.h>
int main()
{
    ADBTools tools;
    if(tools.start_adb_service() == ADBTools::ADB_START_SUCCESSFULLY)
        printf("ADB service start successfully.\n");
    return 0;
}
