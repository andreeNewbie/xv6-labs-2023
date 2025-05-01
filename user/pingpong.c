#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    // create 2 pipe (pipe1_parent to child, pipe2_child to parent)
    int pipe1[2], pipe2[2];

    // check if create pipe fail or not (-1: fail, 0: success)
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
    {
        fprintf(2, "Failed to create 2 pipes!\n");
        exit(1);
    }

    // tạo tiến trình con
    int pid = fork();

    if (pid < 0)
    {
        fprintf(2, "Failed to create pid!\n");
        exit(1);
    }

    // tiến trình con
    if (pid == 0)
    {
        // close write head of pipe1, read head of pipe2
        close(pipe1[1]);
        close(pipe2[0]);

        char byte;

        if (read(pipe1[0], &byte, 1) != 1)
        {
            fprintf(2, "Child: Failed to read from pipe\n");
            exit(1);
        }

        printf("%d: received ping\n", getpid());

        if (write(pipe2[1], &byte, 1) != 1)
        {
            fprintf(2, "Child: Failed to write to pipe\n");
            exit(1);
        }

        // close pipes
        close(pipe1[0]);
        close(pipe2[1]);

        exit(0);
    }
    else // tiến trình cha
    {
        // close read head of pipe1, write head of pipe2
        close(pipe1[0]);
        close(pipe2[1]);

        char byte = 'A';
        if (write(pipe1[1], &byte, 1) != 1)
        {
            fprintf(2, "Parent: Failed to write to pipe\n");
            exit(1);
        }

        if (read(pipe2[0], &byte, 1) != 1)
        {
            fprintf(2, "Parent: Failed to read from pipe\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        // close pipes
        close(pipe1[1]);
        close(pipe2[0]);

        // wait until the subcycle ends
        wait(0);
        exit(0);
    }
}