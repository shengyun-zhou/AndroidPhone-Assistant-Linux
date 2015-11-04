#include "CommandTools.h"
#include "PipeTools.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_SIZE 1024

using namespace std;

const char* CommandTools::EXEC_SUCCESS = "Exec success";
const char* CommandTools::EXEC_FAILED = "Exec failed";

bool CommandTools::exec_command(const char* command, vector<string>& output_result)
{
    PipeTools::clear_pipe();
    int cld_cid;
    if((cld_cid = fork()) == 0)                 //child process
    {
        close(PipeTools::get_read_pipe_fd());
        dup2(PipeTools::get_write_pipe_fd(), STDOUT_FILENO);
        if(system(command) == 0)
            printf("%s\n", EXEC_SUCCESS);
        else
            printf("%s\n", EXEC_FAILED);
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
                if(temp_str[i] == '\n')
                {
                    if(i - pos > 0)
                    {
                        line_str += temp_str.substr(pos, i - pos);
                        output_result.push_back()
                    }
                }
            }
        }
    }
    return true;
}
