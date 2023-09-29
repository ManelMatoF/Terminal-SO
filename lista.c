#include "lista.h"

void createEmptyListF(listFiles *L) {
    *L = NULL;
}

void createEmptyListH(listHist *L) {
    *L = NULL;
}

tPosL firstF(listFiles F){
    return F;
}

tPosH firstH(listHist H){
    return H;
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

tPosL nextF(tPosL p, listFiles F){
    return (p->next);
}

tPosH nextH(tPosL p, listHist H){
    return (p->next);
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

char *getItemF(tPosL p,listFiles L) {
    return p->name;
}

char *getItemH(tPosH p,listHist L) {
    return p->comand;
}

tPosL findItemF(int df, listFiles L) {
    tPosL q;
        for (q = firstF(L); q != NULL; q = nextF(q, L)) {
            if (q->df == df) {
                return q;
            }
        }
        return NULL;
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

bool createNodeF (tPosL *p){//cambiar tPosL -> tPosF
    *p=malloc(sizeof(**p));
    return(*p!=NULL);
}

bool createNodeH (tPosH *p){
    *p=malloc(sizeof(**p));
    return(*p!=NULL);
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

void deleteListH(listHist *L) {
    tPosH x = lastH(*L);
    while (x != NULL) {
        tPosH previous = previousH(x, *L);
        free(x);
        x = previous;
    }
    *L = NULL;
}
