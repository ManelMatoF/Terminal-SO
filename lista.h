#ifndef TERMINAL_SHELL_H
#define TERMINAL_SHELL_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // Para la función open
#include <unistd.h> // Para la función close
#define MAXNAME 100
typedef struct FileInfo* tPosL;
struct FileInfo{
    int modo;
    int df;
    char name[MAXNAME];
    tPosL next;
};
typedef tPosL listFiles;


#endif //TERMINAL_SHELL_H

