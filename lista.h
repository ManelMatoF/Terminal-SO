#ifndef P0_LISTA_H
#define P0_LISTA_H
#define MAXNAME 100
#include <stdio.h>
#include <stdbool.h>

typedef struct FileInfo* tPosF;
struct FileInfo{
    int modo;
    int df;
    char name[MAXNAME];
    tPosF next;
};
typedef tPosF listFiles;

typedef struct ComandHist* tPosH;
struct ComandHist{
    char comand[MAXNAME];
    tPosH next;
};
typedef tPosH listHist;

void createEmptyListF(listFiles *L);
void createEmptyListH(listHist *L);
tPosF firstF(listFiles L);
tPosH firstH(listHist L);
tPosF lastF(listFiles L);
tPosH lastH(listHist L);
tPosF previousF(tPosF p, listFiles L);
tPosH previousH(tPosH p, listHist L);
tPosF nextF(tPosF p, listFiles L);
tPosH nextH(tPosH p, listHist L);
int countFiles(listFiles *L);
char *getItemF(tPosF p, listFiles L);
char *getItemH(tPosH p, listHist L);
tPosF findItemF(int df, listFiles L);
void insertItemF(int df, int mode, char name[MAXNAME], listFiles *L);
bool insertItemH(char name[MAXNAME], listHist *L);
void deleteItemF(listFiles *L, int df);
void deleteListH(listHist *L);

#endif
