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
    fgets(input, MAX_CH, stdin);//cuantos caracteres leer (preguntar)
}

int TrocearCadena(char *cadena, char **input_trozos){
    int i=1;
    if ((input_trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((input_trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

int encontrarComands(char **input_trozos, char **comands, int n) {
    int i = 0;
    if(n>0){
        while (i < MAX_COMANDS - 1 && (strcmp(comands[i], input_trozos[0]) != 0))
        i++;                
        if(strcmp(comands[i], input_trozos[0]) == 0)
            return i;
        else
            return -1;
    }
    else
        return -1;
}


void procesarEntrada(char *input, char **input_trozos,char **comands, bool *terminado, tList *H, tList *F, int *bucle){
    char copy_input[MAX_CH];
    if(*bucle < 1){
        size_t inputSize = strlen(input) + 1;
        if(!insertItem(input, inputSize, H)){
            perror("No se ha podido insertar en Historial");
            return;
        }
    }
       
    strcpy(copy_input, input);
    int input_num = TrocearCadena(copy_input, input_trozos);
    int cmd = encontrarComands(input_trozos, comands, input_num);
    switch(cmd){
        case 0:
            authors(input_trozos, input_num);
            break;
        case 1:
            pid(input_trozos, input_num);
            break;
        case 2:
            chdir_func(input_trozos, input_num);
            break;
        case 3:
            date();
            break;
        case 4:
            tiempo();
            break;
        case 5:
            hist(input_trozos, input_num, H);
            break;
        case 6:
            if(repeat_command(input_trozos, input_num, *H, input, bucle))
                procesarEntrada(input, input_trozos, comands, terminado, H, F, bucle);
            break;
        case 7:
            Cmd_open(input_trozos + 1, F);
            break;
        case 8:
            Cmd_close(input_trozos + 1, F);
            break;
        case 9:
            Cmd_dup(input_trozos + 1, F);
            break;
        case 10:
            ListOpenFiles(*F);
            break;
        case 11:
            infosys();
            break;
        case 12:
            help(comands, input_trozos, input_num, MAX_COMANDS);
            break;
        case 13:
            quit(terminado);
            break;
        case 14:
            exit_func(terminado);
            break;
        case 15:
            bye(terminado);
            break;
        default:
            if(input_num == 0)
                printf("\n");
            else
                printf("No ejecutado: No such file or directory\n");
            break;
    }
}

void insertComands(char **comands) {

    comands[0] = "authors";
    comands[1] = "pid";
    comands[2] = "chdir";
    comands[3] =  "date";
    comands[4] = "time";
    comands[5] = "hist";
    comands[6] = "comand";
    comands[7] = "open";
    comands[8] = "close";
    comands[9] = "dup";
    comands[10] = "listopen";
    comands[11] = "infosys";
    comands[12] = "help";
    comands[13] = "quit";
    comands[14] = "exit";
    comands[15] = "bye";
    comands[16] = "create";
    comands[17] = "stat";
    comands[18] = "list";
    comands[19] = "delete";
    comands[20] = "deltree";

    }

void prelist_files(int df, int modo, char name[MAX_CH], tList *F, FileInfo *f, size_t fileSize){
    f->df=df;
    strcpy(f->name, name);
    f->modo=modo;
    insertItem(f,fileSize, F);

}

int main(){
    char input[MAX_CH], name[MAX_CH];
    bool terminado = false;
    int bucle = 0;
    char **input_trozos = (char **)malloc(MAX_TROZOS * sizeof(char*));
    char **comands = (char **)malloc(MAX_COMANDS * sizeof(char*));

    tList H;
    tList F;
    createEmptyList(&F);
    createEmptyList(&H);

    FileInfo *f0 = malloc(sizeof(FileInfo));
    size_t fileSize = sizeof(FileInfo);
    prelist_files(0, 02, strcpy(name, "entrada estandar"), &F, f0, fileSize);
    prelist_files(1, 02, strcpy(name, "salida estandar"), &F, f0, fileSize);
    prelist_files(2, 02, strcpy(name, "error estandar"), &F, f0, fileSize);

    insertComands(comands);

    while(!terminado){
        imprimirPromt();
        leerEntrada(input);
        procesarEntrada(input, input_trozos,comands, &terminado, &H, &F, &bucle);
    }    

    free(input_trozos);
    free(comands);
    deleteList(&H);
    deleteList(&F);
    free(f0);

    return 0;
}
