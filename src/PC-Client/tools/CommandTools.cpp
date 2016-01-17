#include "CommandTools.h"
#include "PipeTools.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <gksu.h>
#define MAX_SIZE 2048

extern char** environ;

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
		//dup2(PipeTools::get_write_pipe_fd(), STDERR_FILENO);
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
                        output_debug_info(string("commad output line:") + line_str.c_str());
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
	for(int i = output_result.size() - 1; i >= 0; i--)
	{
		if(output_result[i] == "TRUE")
		{
			if(i >= 1 && output_result[i - 1].substr(0, 5) == "Error")
				return false;
			return true;
		}
		else if(output_result[i] == "FALSE")
			return false;
	}
	return false;
}

gchar* ask_pass(GksuContext *context, gchar *prompt, gpointer user_data, GError** error)
{
	//printf("password=%s\n",(gchar*)user_data);
	return (gchar*)user_data;
}

void pass_not_needed(GksuContext *context, gpointer user_data)
{}

CommandTools::SUDO_COMMAND_EXEC_RESULT CommandTools::exec_sudo_command(const char* command, const char* password)
{
	GError* error = NULL;
	gint8 exit_status, error_code = -1;
	GksuContext* context = gksu_context_new();
	gksu_context_set_always_ask_password(context, TRUE);
	gksu_context_set_debug(context, TRUE);
	gksu_context_set_command(context, g_strdup(command));
	gksu_context_set_user(context, g_strdup("root"));
	gboolean ret = gksu_sudo_fuller(context, (GksuAskPassFunc)ask_pass, (gpointer)g_strdup(password),
				   (GksuPassNotNeededFunc)pass_not_needed, NULL,
					&exit_status, &error);
	gksu_context_free(context);
	if(error)
	{
		error_code = error->code;
		g_error_free(error);
	}
	if(error && (error_code == GKSU_ERROR_WRONGPASS || error_code == GKSU_ERROR_NOPASSWORD))
		return COMMAND_EXEC_WRONG_PASSWORD;
	else if(error)
		return COMMAND_EXEC_FAILED;
	if(exit_status != 0)
		return COMMAND_EXEC_FAILED;
	//gksu_su(const_cast<gchar*>(command), &error);
	return COMMAND_EXEC_SUCCESSFUL;
}

string CommandTools::get_current_proc_path()
{
	char buf[MAX_SIZE];
	int read_count = readlink("/proc/self/exe", buf, sizeof(buf));
	if(read_count < 0 || read_count >= sizeof(buf))
		return "";
	buf[read_count] = '\0';
	return buf;
}

void CommandTools::output_debug_info(const string& info)
{
	fprintf(stderr, "-DEBUG(AndroidPhone-Assistant):%s\n", info.c_str());
}
