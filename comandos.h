#ifndef P0_COMANDOS_H
#define P0_COMANDOS_H

#include <stdio.h>
#include <stdbool.h>
#include "lista.h"

#define MAXNAME 350
#define MAX_TROZOS 100
#define TAMANO 2048


extern tList H, C, F, M, P;

void aux_procesarEntrada(char *input, bool *terminado, char **arg3);
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
void malloc_funct(char *input, char *input_trozos[], int n, bool *terminado);
void shared_funct(char *input, char *input_trozos[], int n, bool *terminado);
void mmap_funct(char *input, char *input_trozos[], int n, bool *terminado);
void CmdRead(char *input, char *input_trozos[], int n, bool *terminado);
void Write(char *input, char *input_trozos[], int n, bool *terminado);
void Memdump(char *input, char *input_trozos[], int n, bool *terminado);
void Memfill(char *input, char *input_trozos[], int n, bool *terminado);
void Mem(char *input, char *input_trozos[], int n, bool *terminado);
void Rec(char *input, char *input_trozos[], int n, bool *terminado);
void uid_funct(char *input, char *input_trozos[], int n, bool *terminado);
void showvar(char *input, char *input_trozos[], int n, bool *terminado);
void showenv(char *input, char *input_trozos[], int n, bool *terminado);
void changevar(char *input, char *input_trozos[], int n, bool *terminado);
void subsvar(char *input, char *input_trozos[], int n, bool *terminado);
void fork_funct(char *input, char *input_trozos[], int n, bool *terminado);
void Exec(char *input, char *input_trozos[], int n, bool *terminado);
void jobs(char *input, char *input_trozos[], int n, bool *terminado);
void DeleteJobs(char *input, char *input_trozos[], int n, bool *terminado);
void Job(char *input, char *input_trozos[], int n, bool *terminado);


#endif //P0_COMANDOS_H
