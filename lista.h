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
    char name[MAXNAME];
    tPosH next;
};
typedef tPosH listHist;

void createEmptyListF(listFiles *L);
void createEmptyListH(listHist *L);
tPosL lastF(listFiles L);
tPosH lastH(listHist L);
tPosL previousF(tPosL p, listFiles L);
tPosH previousH(tPosH p, listHist L);
int countFiles(listFiles *L);
char *getItemF(tPosL p,listFiles L);
tPosL findItemF(int df, listFiles L);
void insertItemF(int df, int mode, char name[MAXNAME], listFiles *L);
void insertItemH(char name[MAXNAME], listHist *L);
void deleteItemF(listFiles *L, int df);
void deleteListH(listHist *L);
#endif
