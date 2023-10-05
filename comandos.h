#ifndef P0_COMANDOS_H
#define P0_COMANDOS_H

#include <stdio.h>
#include <stdbool.h>
#include "lista.h"

#define MAXNAME 100

void ListOpenFiles(listFiles L);
void Cmd_open (char * tr[], listFiles *L);
void Cmd_close (char *tr[], listFiles *L);
void Cmd_dup (char * tr[], listFiles *L);
void authors(char *input_trozos[], int n);
void date();
void tiempo();
void infosys();
void pid(char *input_trozos[], int n);
void chdir_func(char *input_trozos[], int n);
bool repeat_command(char *input_trozos[], int n, listHist H, char *cadena, int *bucle);
void hist(char *input_trozos[], int n, listHist *H);
void help(char *commands[], char *input_trozos[], int n, int nComands);
void quit(bool *terminado);
void exit_func(bool *terminado);
void bye(bool *terminado);


#endif //P0_COMANDOS_H
