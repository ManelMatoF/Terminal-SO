#ifndef P0_COMANDOS_H
#define P0_COMANDOS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // Para la función open
#include <unistd.h> // Para la función close
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <errno.h>
#include <limits.h>
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
void repeat_command(char *input_trozos[], listHist H);
void hist(char *input_trozos[]);
void help(char *commands[], char *input_trozos[], int nComands);
void quit(bool terminado);
void exit_func(bool terminado);
void bye(bool terminado);


#endif //P0_COMANDOS_H
