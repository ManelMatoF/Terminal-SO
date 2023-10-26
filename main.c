//Fernando Losada Pérez fernando.losada@udc.es
//Manel Mato Fernández manel.mfernandez@udc.es
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "comandos.h"
#include "lista.h"

#define MAX_CH 200
#define MAX_TROZOS 10
#define MAX_COMANDS 21

void imprimirPromt(){
    printf("-> ");
}

void leerEntrada(char *input){
    fgets(input, MAX_CH, stdin);
}


void procesarEntrada(char *input, bool *terminado){
    if(input[0]!='\n'){
        size_t inputSize = strlen(input) + 1;
        if(!insertItem(input, inputSize, &H)){
            perror("No se ha podido insertar en Historial");
            return;
        }
        aux_procesarEntrada(input, terminado);
    }
}

void insert_Comands(){
    Comands *c = malloc(sizeof(Comands));
    size_t ComandSize = sizeof(Comands);
    for (int i = 0; i < MAX_COMANDS; i++){
        switch (i) {
            case 0 :
                strcpy(c->comand,"authors");
                strcpy(c->help_comand,"authors [-n|-l]\t Muestra los nombres y/o logins de los autores\n");
                c->funcion=authors;
                insertItem(c, ComandSize, &C);
                break;
            case 1:
                strcpy(c->comand, "pid");
                strcpy(c->help_comand, "pid [-p]\t Muestra el pid del shell o de su proceso padre\n");
                c->funcion=pid;
                insertItem(c, ComandSize, &C);
                break;
            case 2:
                strcpy(c->comand, "chdir");
                strcpy(c->help_comand,"chdir [dir]\t	Cambia (o muestra) el directorio actual del shell\n");
                c->funcion=chdir_func;
                insertItem(c, ComandSize, &C);
                break;
            case 3:
                strcpy(c->comand,"date");
                strcpy(c->help_comand, "date\tMuestra la fecha acual\n");
                c->funcion=date;
                insertItem(c, ComandSize, &C);
                break;
            case 4:
                strcpy(c->comand, "time");
                strcpy(c->help_comand, "time\tMuestra la hora actual\n");
                c->funcion=tiempo;
                insertItem(c, ComandSize, &C);
                break;
            case 5:
                strcpy(c->comand, "hist");
                strcpy(c->help_comand, "hist [-c|-N]\tMuestra (o borra)el historico de comandos\n"
                    "\t-N: muestra los N primeros\n"
                    "\t-c: borra el historico\n");
                c->funcion=hist;
                insertItem(c, ComandSize, &C);
                break;
            case 6:
                strcpy(c->comand, "comand");
                strcpy(c->help_comand, "comand [-N]\tRepite el comando N (del historico)\n");
                c->funcion=repeat_command;
                insertItem(c, ComandSize, &C);
                break;
            case 7:
                strcpy(c->comand, "open");
                strcpy(c->help_comand, "open fich m1 m2...\tAbre el fichero fich. y lo anade a la lista de ficheros abiertos del shell\n"
                    "\t m1, m2..es el modo de apertura (or bit a bit de los siguientes).\n"
                    "\tcr: O_CREAT\tap: O_APPEND\n"
                    "\tex: O_EXCL \tro: O_RDONLY\n"
                    "\trw: O_RDWR \two: O_WRONLY\n"
                    "\ttr: O_TRUNC\n");
                c->funcion=Cmd_open;
                insertItem(c, ComandSize, &C);
            break;
            case 8:
                strcpy(c->comand, "close");
                strcpy(c->help_comand, "close df\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");
                c->funcion= Cmd_close;
                insertItem(c, ComandSize, &C);
                break;
            case 9:
                strcpy(c->comand, "dup");
                strcpy(c->help_comand, "dup df\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n");
                c->funcion= Cmd_dup;
                insertItem(c, ComandSize, &C);
                break;
            case 10:
                strcpy(c->comand, "listopen");
                strcpy(c->help_comand, "listopen [n]\tLista los ficheros abiertos (al menos n) del shell\n");
                c->funcion= ListOpenFiles;
                insertItem(c, ComandSize, &C);
                break;
            case 11:
                strcpy(c->comand, "infosys");
                strcpy(c->help_comand, "infosys \tMuestra informacion de la maquina donde corre el shell\n");
                c->funcion = infosys;
                insertItem(c, ComandSize, &C);
                break;
            case 12:
                strcpy(c->comand, "help");
                strcpy(c->help_comand, "help [cmd|-lt|-T]\tMuestra ayuda sobre los comandos\n"
                    "\t-lt: lista topics de ayuda\n"
                    "\t-T topic: lista comandos sobre ese topic\n"
                    "\tcmd: info sobre el comando cmd\n");
                c->funcion=help;
                insertItem(c, ComandSize, &C);
                break;
            case 13:
                strcpy(c->comand, "quit");
                strcpy(c->help_comand, "quit \tTermina la ejecucion del shell\n");
                c->funcion=quit;
                insertItem(c, ComandSize, &C);
                break;
            case 14:
                strcpy(c->comand, "exit");
                strcpy(c->help_comand, "exit \tTermina la ejecucion del shell\n");
                c->funcion=exit_func;
                insertItem(c, ComandSize, &C);
                break;
            case 15:
                strcpy(c->comand, "bye");
                strcpy(c->help_comand, "bye \tTermina la ejecucion del shell\n");
                c->funcion=bye;
                insertItem(c, ComandSize, &C);
                break;
            case 16:
                strcpy(c->comand, "create");
                strcpy(c->help_comand, "create [-f] [name] \tCrea un directorio o un fichero (-f)\n");
                c->funcion=create;
                insertItem(c, ComandSize, &C);
                break;
            case 17:
                strcpy(c->comand, "stat");
                strcpy(c->help_comand, "stat [-long][-link][-acc] name1 name2 .. \tlista ficheros;\n"
                    "\t-long: listado largo\n"
                    "\t-acc: acesstime\n"
                    "\t-link: si es enlace simbolico, el path contenido\n");
                /*c->funcion=stat;*/
                insertItem(c, ComandSize, &C);
                break;
            case 18:
                strcpy(c->comand, "list");
                strcpy(c->help_comand, "list [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 .. \tlista contenidos de directorios"
                    "\t-hid: incluye los ficheros ocultos"
                    "\t-recb: recursivo (antes)"
                    "\t-reca: recursivo (despues)"
                    "\tresto parametros como stat\n");
                /*c->funcion=list;*/
                insertItem(c, ComandSize, &C);
            break;
            case 19:
                strcpy(c->comand, "delete");
                strcpy(c->help_comand, "delete [name1 name2 ..] \tBorra ficheros o directorios vacios\n");
                c->funcion=delete;
                insertItem(c, ComandSize, &C);
                break;
            case 20:
                strcpy(c->comand, "deltree");
                strcpy(c->help_comand, "deltree [name1 name2 ..] \tBorra ficheros o directorios no vacios recursivamente\n");
                c->funcion=deltree;
                insertItem(c, ComandSize, &C);
                break;
            default :
                break;
        }
    }
    free(c);
}

void prelist_files(int df, int modo, char name[MAX_CH], FileInfo *f, size_t fileSize){
    f->df=df;
    strcpy(f->name, name);
    f->modo=modo;
    if(!insertItem(f,fileSize, &F))
        perror("Imposible insertar");
}

int main(){
    char input[MAX_CH], name[MAX_CH];
    bool terminado = false;

    createEmptyList(&H);
    createEmptyList(&F);
    createEmptyList(&C);

    FileInfo *f0 = malloc(sizeof(FileInfo));
    size_t fileSize = sizeof(FileInfo);
    prelist_files(0, 02, strcpy(name, "entrada estandar"),  f0, fileSize);
    prelist_files(1, 02, strcpy(name, "salida estandar"), f0, fileSize);
    prelist_files(2, 02, strcpy(name, "error estandar"), f0, fileSize);

    insert_Comands(&C);

    while(!terminado){
        imprimirPromt();
        leerEntrada(input);
        procesarEntrada(input, &terminado);
    }
    
    deleteList(&H);
    deleteList(&F);
    deleteList(&C);
    free(f0);

    return 0;
}
