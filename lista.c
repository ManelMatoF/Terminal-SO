#include "shell.h"

void createEmptyList(listFiles *L) {
    *L = NULL;
}
tPosL last(listFiles L) {
    tPosL x = L;
    while (x->next != NULL) {
        x = x->next;
    }
    return x;
}

tPosL previous(tPosL p, listFiles L) {
    tPosL x = L;
    while (x->next != NULL && x->next != p) {
        x = x->next;
    }
    return x;
}

int countFiles(listFiles *L){
    tPosL x = *L;
    int a = 0;
    while (x->next != NULL) {
        x = x->next;
        a++;
    }
    return a;
}

void insertFile(int df, int mode, char name[MAXNAME], listFiles *L){
    tPosL aux,x;
    aux->df=df;
    aux->modo=mode;
    stpcpy(aux->name,name);
    aux->next = NULL;

    if (*L==NULL){
        *L = aux;
    } else{
        x = last(*L);
        x->next = aux;
    }
    open(name, mode, 0777);
}

void ListOpenFiles(listFiles L) {
    tPosL x = L;
    int i = 0;
    char aux[15];
    while (x->next != NULL) {
        i++;
        if (x->modo == 0100) strcpy(aux, "O_CREAT");
        else if (x->modo == 0200) strcpy(aux, "O_EXCL");
        else if (x->modo == 0) strcpy(aux, " O_RDONLY");
        else if (x->modo == 01) strcpy(aux, "O_WRONLY");
        else if (x->modo == 02) strcpy(aux, "O_RDWR");
        else if (x->modo == 02000) strcpy(aux, "O_APPEND");
        else if (x->modo == 01000) strcpy(aux, "O_TRUNC");
        printf("decriptor: %d -> %s %s\n", i, x->name, aux);
        x = x->next;

    }
}

void CloseOpenFile(listFiles *L, int df){
    tPosL x = *L;
    while (x != NULL || x->df != df) {
        x = x->next;
    }
    if (x == NULL){
        printf("No se ha podido encontrar el descriptor");
    }else if (x->df == df){
        if (x == *L){
            *L = x->next;
        }else{
            previous(x, *L)->next = x->next;
        }
        free(x);
        close(df);
    }
}
