#ifndef P0_LISTA_H
#define P0_LISTA_H
#define MAXNAME 300
#include <stdio.h>
#include <stdbool.h>


typedef struct nodo *tPosL;
struct nodo {
    void *data;             
    struct nodo *next;
};
typedef tPosL tList;


void createEmptyList(tList *L);

tPosL first(tList L);

tPosL last(tList L);

tPosL previous(tPosL p, tList L);

tPosL next(tPosL p);

void *getItem(tPosL p);

bool insertItem(void *data, size_t dataSize, tList *L);

bool deleteItem(tPosL p, tList *L);

void deleteList(tList *L);

typedef struct{
    int modo;
    int df;
    char name[MAXNAME];
}FileInfo;

typedef struct{
    char comand[MAXNAME];
    char help_comand[MAXNAME];
    void (*funcion)(char *input, char *input_trozos[MAXNAME], int n, bool *terminate);
}Comands;

#endif
