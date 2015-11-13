#ifndef COMMANDTOOLS_H
#define COMMANDTOOLS_H

#include <vector>
#include <string>

using namespace std;
class CommandTools
{
    private:
        static const char* EXEC_SUCCESS;
        static const char* EXEC_FAILED;
    public:
        static bool exec_command(const char* command, vector<string>& output_result);
		static bool exec_adb_shell_command(const char* adb_path, const char* shell_command);
};

#endif // COMMANDTOOLS_H
