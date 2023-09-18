#ifndef TERMINAL_SHELL_H
#define TERMINAL_SHELL_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // Para la función open
#include <unistd.h> // Para la función close
#define MAXNAME 100
struct FileInfo{
    int modo;
    int df;
    char name[MAXNAME];
    struct FileInfo* next;
};
struct FileInfo* listFiles = NULL;
#endif //TERMINAL_SHELL_H
