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
		enum SUDO_COMMAND_EXEC_RESULT
		{
			COMMAND_EXEC_SUCCESSFUL,
			COMMAND_EXEC_WRONG_PASSWORD,
			COMMAND_EXEC_FAILED
		};
		
        static bool exec_command(const char* command, vector<string>& output_result);
		static bool exec_adb_shell_command(const char* adb_path, const char* shell_command);
		static SUDO_COMMAND_EXEC_RESULT exec_sudo_command(const char* command, const char* password);
		static string get_current_proc_path();
		static void output_debug_info(const string& info);
};

#endif // COMMANDTOOLS_H
