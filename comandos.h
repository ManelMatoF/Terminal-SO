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
void authors(char *input_trozos[]);
void date();
void tiempo();
void infosys();
void pid(char *input_trozos[]);
void chdir_func(char *input_trozos[]);
bool *repeat_command(char *input_trozos[], listHist H, char *cadena);
void hist(char *input_trozos[], listHist *H);
void help(char *commands[], char *input_trozos[], int nComands);
bool quit();
bool exit_func();
bool bye();


#endif //P0_COMANDOS_H
