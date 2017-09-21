#include "PipeTools.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int PipeTools::pipe_fd[2] = {-1, -1};

bool PipeTools::init_pipe() {
    if (pipe_fd[0] >= 0)
        close(pipe_fd[0]);
    if (pipe_fd[1] >= 0)
        close(pipe_fd[1]);
    if (pipe(pipe_fd) < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return false;
    }
    return true;
}
