#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXARG 32  // Maximum number of arguments
#define MAXLEN 512 // Maximum length of an input line

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [arg...]\n");
        exit(1);
    }

    char *new_argv[MAXARG];
    int new_argc = 0;

    // Check for -n argument
    if (argc > 2 && strcmp(argv[1], "-n") == 0) {
        for (int i = 3; i < argc; i++) {
            new_argv[new_argc++] = argv[i];
        }
    } else {
        for (int i = 1; i < argc; i++) {
            new_argv[new_argc++] = argv[i];
        }
    }

    char buf[MAXLEN];
    int len = 0;

    while (read(0, buf + len, 1) == 1) {
        if (buf[len] == '\n') { // When encountering a newline, execute the command
            buf[len] = 0; // Null-terminate the string
            new_argv[new_argc] = buf; // Add the line to the last argument
            new_argv[new_argc + 1] = 0; // Null-terminated argument list

            // Check if new_argv[0] is valid before execution
            if (new_argv[0] == 0) {
                fprintf(2, "xargs: no command provided\n");
                exit(1);
            }

            if (fork() == 0) { // Create a child process to execute the command
                exec(new_argv[0], new_argv);
                fprintf(2, "exec %s failed\n", new_argv[0]);
                exit(1);
            } else {
                wait(0); // Wait for the child process to finish
            }
            len = 0; // Reset buffer
        } else {
            len++;
            if (len >= MAXLEN) { // Limit input length
                fprintf(2, "xargs: input line too long\n");
                exit(1);
            }
        }
    }

    return 0;
}