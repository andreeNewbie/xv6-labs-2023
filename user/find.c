#include "kernel/types.h"
 #include "kernel/stat.h"
 #include "user/user.h"
 #include "kernel/fs.h"
 #include "kernel/fcntl.h"

void my_strcat(char *dest, const char *src)
{
    while (*dest)
        dest++;

    while (*src)
        *dest++ = *src++;

    *dest = '\0';
}

void joinPaths(char *buff, const char *dirPath, const char *fileName)
{
    strcpy(buff, dirPath);
    // check if the directory path ends with '/', if not then add it
    if (buff[strlen(buff) - 1] != '/')
        my_strcat(buff, "/");

    my_strcat(buff, fileName);
}

void find(const char *dirPath, const char *targetFile)
{
    char buff[512];
    int fd; // file descriptor
    struct dirent de;
    struct stat st; // status

    // open the directory
    fd = open(dirPath, 0);
    if (fd < 0)
    {
        fprintf(2, "Can't open %s\n", dirPath);
        return;
    }

    // get the status of the directory
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "Can't stat %s\n", dirPath);
        close(fd);
        return;
    }

    // check if the path is directory or not
    if (st.type != T_DIR)
    {
        fprintf(2, "find: %s is not a directory\n", dirPath);
        close(fd);
        return;
    }

    // reading directory entries
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // Skip entries that are empty or represent the current and parent directories
        if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // construct the full path of file/directory
        joinPaths(buff, dirPath, de.name);

        // get the status of the full path
        if (stat(buff, &st) < 0)
        {
            fprintf(2, "find: Can't stat %s\n", buff);
            continue;
        }

        // compare entry's name with target file
        if (strcmp(de.name, targetFile) == 0)
            printf("%s\n", buff);

        // check if the full path is directory or not
        if (st.type == T_DIR)
            find(buff, targetFile);
    }
    // close the file descriptor after reading all files/directories
    close(fd);
}

int main(int argc, char *argv[])
{
    // Check if the correct number of arguments is provided
    if (argc != 3)
    {
        fprintf(2, "Usage: <directory> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}