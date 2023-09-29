#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

void createEmptyListF(listFiles *L) {
    *L = NULL;
}

void createEmptyListH(listHist *L) {
    *L = NULL;
}

tPosF firstF(listFiles L){
    return L;
}

tPosH firstH(listHist L){
    return L;
}

tPosF lastF(listFiles L) {
    tPosF x = L;
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

tPosF previousF(tPosF p, listFiles L) {
    tPosF x = L;
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

tPosF nextF(tPosF p, listFiles L){
    return (p->next);
}

tPosH nextH(tPosF p, listHist L){
    return (p->next);
}


int countFiles(listFiles *L){
    tPosF x = *L;
    int a = 0;
    while (x->next != NULL) {
        x = x->next;
        a++;
    }
    return a;
}

char *getItemF(tPosF p,listFiles L) {
    return p->name;
}

char *getItemH(tPosH p,listHist L) {
    return p->comand;
}

tPosF findItemF(int df, listFiles L) {
    tPosF q;
        for (q = firstF(L); q != NULL; q = nextF(q, L)) {
            if (q->df == df) {
                return q;
            }
        }
        return NULL;
    }

bool createNodeF (tPosF *p){//cambiar tPosL -> tPosF
    *p=malloc(sizeof(**p));
    return(*p!=NULL);
}

bool createNodeH (tPosH *p){
    *p=malloc(sizeof(**p));
    return(*p!=NULL);
}

void insertItemF(int df, int mode, char name[MAXNAME], listFiles *L){
    tPosF aux,x;
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

bool insertItemH(char name[MAXNAME], listHist *L){
    tPosH aux,x;
    if(createNodeH(&aux)){
        stpcpy(aux->comand,name);
        aux->next = NULL;
        if (*L==NULL)
        *L = aux;
        else{
        x = lastH(*L);
        x->next = aux;
        }
    }else
        return false;   
}


void deleteItemF(listFiles *L, int df){
    tPosF x = *L;
    while (x != NULL && x->df != df) {
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

void deleteListH(listHist *L) {
    tPosH x = lastH(*L);
    while (x != NULL) {
        tPosH previous = previousH(x, *L);
        free(x);
        x = previous;
    }
    *L = NULL;
}
