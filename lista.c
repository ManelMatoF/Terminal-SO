#include "shell.h"

void createEmptyListF(listFiles *L) {
    *L = NULL;
}

void createEmptyListH(listHist *L) {
    *L = NULL;
}

tPosL lastF(listFiles L) {
    tPosL x = L;
    while (x->next != NULL) {
        x = x->next;
    }
    return x;
}

tPosH lastH(listHist L) {
    tPosH x = L;
    while (x->next != NULL) {
        x = x->next;
    }
    return x;
}

tPosL previousF(tPosL p, listFiles L) {
    tPosL x = L;
    while (x->next != NULL && x->next != p) {
        x = x->next;
    }
    return x;
}

tPosH previousH(tPosH p, listHist L) {
    tPosH x = L;
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

void insertItemF(int df, int mode, char name[MAXNAME], listFiles *L){
    tPosL aux,x;
    aux->df=df;
    aux->modo=mode;
    stpcpy(aux->name,name);
    aux->next = NULL;

    if (*L==NULL){
        *L = aux;
    } else{
        x = lastF(*L);
        x->next = aux;
    }
}

void insertItemH(char name[MAXNAME], listHist *L){
    tPosH aux,x;
    stpcpy(aux->name,name);
    aux->next = NULL;

    if (*L==NULL){
        *L = aux;
    } else{
        x = lastH(*L);
        x->next = aux;
    }
}


void deleteItemF(listFiles *L, int df){
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
            previousF(x, *L)->next = x->next;
        }
        free(x);
    }
}

void deleteListH(listHist *L){
    tPosH x = previousH(lastH(*L), *L);
    while (x != NULL) {
        free(x->next);
        x = previousH(x, *L);
    }
}
