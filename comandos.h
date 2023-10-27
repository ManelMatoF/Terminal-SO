#ifndef P0_COMANDOS_H
#define P0_COMANDOS_H

#include <stdio.h>
#include <stdbool.h>
#include "lista.h"

#define MAXNAME 300
#define MAX_TROZOS 10

extern tList H, C, F;

void aux_procesarEntrada(char *input, bool *terminado);
void ListOpenFiles(char *input, char *input_trozos[], int n, bool *terminado);
void Cmd_open (char *input, char *input_trozos[], int n, bool *terminado);
void Cmd_close (char *input, char *input_trozos[], int n, bool *terminado);
void Cmd_dup (char *input, char *input_trozos[], int n, bool *terminado);
void authors(char *input, char *input_trozos[], int n, bool *terminado);
void date(char *input, char *input_trozos[], int n, bool *terminado);
void tiempo(char *input, char *input_trozos[], int n, bool *terminado);
void infosys(char *input, char *input_trozos[], int n, bool *terminado);
void pid(char *input, char *input_trozos[], int n, bool *terminado);
void chdir_func(char *input, char *input_trozos[], int n, bool *terminado);
void repeat_command(char *input, char *input_trozos[], int n, bool *terminado);
void hist(char *input, char *input_trozos[], int n, bool *terminado);
void help(char *input, char *input_trozos[], int n, bool *terminado);
void quit(char *input, char *input_trozos[], int n, bool *terminado);
void exit_func(char *input, char *input_trozos[], int n, bool *terminado);
void bye(char *input, char *input_trozos[], int n, bool *terminado);
void create(char *input, char *input_trozos[], int n, bool *terminado);
void Stat(char *input, char *input_trozos[], int n, bool *terminado);
void list(char *input, char *input_trozos[], int n, bool *terminado);
void delete(char *input, char *input_trozos[], int n, bool *terminado);
void deltree(char *input, char *input_trozos[], int n, bool *terminado);


#endif //P0_COMANDOS_H
