#ifndef P0_COMANDOS_H
#define P0_COMANDOS_H

#include <stdio.h>
#include <stdbool.h>
#include "lista.h"

#define MAXNAME 150

void ListOpenFiles(tList L);
void Cmd_open (char * tr[], tList *L);
void Cmd_close (char *tr[], tList *L);
void Cmd_dup (char * tr[], tList *L);
void authors(char *input_trozos[], int n);
void date();
void tiempo();
void infosys();
void pid(char *input_trozos[], int n);
void chdir_func(char *input_trozos[], int n);
bool repeat_command(char *input_trozos[], int n, tList H, char *cadena, int *bucle);
void hist(char *input_trozos[], int n, tList *H);
void help(char *commands[], char *input_trozos[], int n, int nComands);
void quit(bool *terminado);
void exit_func(bool *terminado);
void bye(bool *terminado);


#endif //P0_COMANDOS_H
