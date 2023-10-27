#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

void createEmptyList(tList *L) {
    *L = NULL;
}

tPosL first(tList L){
    return L;
}

tPosL last(tList L) {
    if (L == NULL) {
        return NULL;
    }
    
    tPosL x = L;
    while (x->next != NULL)
        x = x->next;

    return x;
}

tPosL previous(tPosL p, tList L) {
    if(p == L)
        return NULL;
    else{
        tPosL q;
        for(q=L;q->next!=p;q=q->next);
        return q;
    }
}


tPosL next(tPosL p){
    return (p->next);
}

void *getItem(tPosL p) {
    return p->data;
}

static bool createNode (tPosL *p){
    *p=malloc(sizeof(**p));
    return(*p!=NULL);
}


bool insertItem(void *data, size_t dataSize, tList *L) {
    tPosL aux, x;
    
    if (createNode(&aux)) {
        aux->data = malloc(dataSize);
        
        if (aux->data != NULL) {
            memcpy(aux->data, data, dataSize);

            if (*L == NULL) {
                *L = aux;
            } else {
                x = last(*L);
                x->next = aux;
                aux->next = NULL;
            }
            return true;
        }
    }
    return false;
}



bool deleteItem(tPosL p, tList *L){
    tPosL x = *L;
    while (x != NULL && x != p) {
        x = x->next;
    }
    if (x == NULL)
        return false;
    else{
        if (x == first(*L)){
            *L = x->next;
        }else{
            previous(x, *L)->next = x->next;
        }
        free(x->data);
        free(x);
        return true;
    }    
}

void deleteList(tList *L) {
    if (*L != NULL) {
        tPosL x = last(*L);
        while (x != NULL) {
            tPosL prev = previous(x, *L);
            free(x->data);
            free(x);
            x = prev;
        }
        *L = NULL;
    }
}
