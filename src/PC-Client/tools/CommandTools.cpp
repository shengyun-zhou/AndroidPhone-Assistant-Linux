#include "CommandTools.h"
#include "PipeTools.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_SIZE 2048

using namespace std;

const char* CommandTools::EXEC_SUCCESS = "Exec success";
const char* CommandTools::EXEC_FAILED = "Exec failed";

bool CommandTools::exec_command(const char* command, vector<string>& output_result)
{
    if(!PipeTools::init_pipe())
        return false;
    int cld_cid;
    if((cld_cid = fork()) == 0)                 //child process
    {
        close(PipeTools::get_read_pipe_fd());
        dup2(PipeTools::get_write_pipe_fd(), STDOUT_FILENO);
        if(system(command) == 0)
            printf("\n%s\n", EXEC_SUCCESS);
        else
            printf("\n%s\n", EXEC_FAILED);
        exit(0);
    }
    else
    {
        close(PipeTools::get_write_pipe_fd());
        char buf[MAX_SIZE];
        string temp_str;
        string line_str = "";
        while(true)
        {
            int len = read(PipeTools::get_read_pipe_fd(), buf, sizeof(buf) - 1);
            buf[len] = '\0';
            temp_str = buf;
            int i, pos = 0;
            for(i = 0; i < temp_str.length(); i++)
            {
                if(temp_str[i] == '\n' || temp_str[i] == '\r')
                {
                    if(i - pos > 0)
                    {
                        line_str += temp_str.substr(pos, i - pos);
                        //printf("output line:%s\n", line_str.c_str());
                        if(line_str == EXEC_SUCCESS)
                            return true;
                        else if(line_str == EXEC_FAILED)
                            return false;
                        output_result.push_back(line_str);
                        line_str.clear();
                    }
                    else if(i == 0 && line_str.length() > 0){
                        output_result.push_back(line_str);
                        line_str.clear();
                    }
                    pos = i + 1;
                }
            }
            if(pos < temp_str.length())
                line_str += temp_str.substr(pos, i - pos);
        }
    }
    return false;
}

bool CommandTools::exec_adb_shell_command(const char* adb_path, const char* shell_command)
{
	char adb_shell_command[MAX_SIZE];
	sprintf(adb_shell_command, "%s shell \"%s && echo TRUE || echo FALSE\"", adb_path, shell_command);
	
	vector<string> output_result;
	if(!CommandTools::exec_command(adb_shell_command, output_result))
		return false;
	if(output_result.empty())
		return false;
	if(output_result[output_result.size() - 1] == "TRUE")
	{
		if(output_result.size() > 1 && output_result[output_result.size() - 2].substr(0, 5) == "Error")
			return false;
		return true;
	}
	return false;
}

