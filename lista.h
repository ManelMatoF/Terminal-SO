#ifndef P0_LISTA_H
#define P0_LISTA_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // Para la función open
#include <unistd.h> // Para la función close
#include <time.h>
#include <sys/utsname.h>
#include <stdbool.h>
#define MAXNAME 100
typedef struct FileInfo* tPosL;
struct FileInfo{
    int modo;
    int df;
    char name[MAXNAME];
    tPosL next;
};
typedef tPosL listFiles;

typedef struct ComandHist* tPosH;
struct ComandHist{
    char comand[MAXNAME];
    tPosH next;
};
typedef tPosH listHist;

#endif
