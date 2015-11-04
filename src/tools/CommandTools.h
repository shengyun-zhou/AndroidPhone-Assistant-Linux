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
};

#endif // COMMANDTOOLS_H
