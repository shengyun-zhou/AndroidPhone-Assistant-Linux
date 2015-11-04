#include "PipeTools.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int PipeTools::pipe_fd[2] = {-1, -1};

bool PipeTools::init_pipe()
{
    if(pipe(pipe_fd) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return false;
    }
    return true;
}

void PipeTools::clear_pipe()
{
    if(get_read_pipe_fd() < 0)
        return;
    int origin_flag = fcntl(get_read_pipe_fd(), F_GETFL, NULL);
    int new_flag = origin_flag | O_NONBLOCK;
    if(fcntl(get_read_pipe_fd(), F_SETFL, new_flag) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return;
    }
    char buf[1024];
    while(read(get_read_pipe_fd(), buf, sizeof(buf)) == sizeof(buf));
    fcntl(get_read_pipe_fd(), F_SETFL, origin_flag);
    return;
}
