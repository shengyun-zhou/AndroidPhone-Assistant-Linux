#ifndef PIPETOOLS_H
#define PIPETOOLS_H


class PipeTools {
private:
    static int pipe_fd[2];
public:
    static bool init_pipe();

    static int get_read_pipe_fd() {
        return pipe_fd[0];
    }

    static int get_write_pipe_fd() {
        return pipe_fd[1];
    }
};

#endif // PIPETOOLS_H
