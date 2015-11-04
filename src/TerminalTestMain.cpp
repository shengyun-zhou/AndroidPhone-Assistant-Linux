#include "adb-tools/ADBTools.h"
#include "tools/PipeTools.h"
#include <stdio.h>
int main()
{
    PipeTools::init_pipe();
    ADBTools tools;
    if(tools.start_adb_service() == ADBTools::ADB_START_SUCCESSFULLY)
        printf("ADB server start successfully.\n");
    else
        printf("ADB server start failed.\n");
    return 0;
}
