#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "comandos.h"
#include "lista.h"


#define MAX_CH 250
#define MAX_TROZOS 10
#define MAX_COMANDS 16

void imprimirPromt(){
    printf("-> ");
}

void leerEntrada(char * input){
    fgets(input, MAX_CH, stdin);//cuantos caracteres leer (preguntar)
}

int TrocearCadena(char *cadena, char *trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

int encontrarComands(char *input_trozos[],char *comands[]){
    int i;
    for(i=0; i<MAX_COMANDS && strcmp(comands[i], input_trozos[i]) != 0; i++);
    if(i==MAX_COMANDS && strcmp(comands[i], input_trozos[i]) != 0)
        return -1;
    else
        return i;
}

void procesarEntrada(char *cadena, char *input_trozos[],char *comands[], bool terminado, listHist *H, listFiles *F){
    insertItemH(cadena, H);
    TrocearCadena(cadena, input_trozos);
    int cmd = encontrarComands(input_trozos, comands);
    switch(cmd){
        case 0:
            authors(input_trozos);
            break;
        case 1:
            pid(input_trozos);
            break;
        case 2:
            chdir_func(input_trozos);
            break;
        case 3:
            date();
            break;
        case 4:
            tiempo();
            break;
        case 5:
            hist(input_trozos, H);
            break;
        case 6:
            if(repeat_command(input_trozos, *H, cadena));
                procesarEntrada(cadena, input_trozos, comands, terminado, H, *F);//probar
            break;
        case 7:
            Cmd_open(input_trozos, F);
            break;
        case 8:
            Cmd_close(input_trozos, F);
            break;
        case 9:
            Cmd_dup(input_trozos, F);
            break;
        case 10:
            ListOpenFiles(*F);
            break;
        case 11:
            infosys();
            break;
        case 12:
            help(comands, input_trozos, MAX_COMANDS);
            break;
        case 13:
            terminado = quit();
            break;
        case 14:
            terminado = exit_func();
            break;
        case 15:
            terminado = bye();
            break;
        default:
            printf("No ejecutado: No such file or directory\n");
            break; 
    }
}

void insertComands(char *comands[]){
    for (int i = 0; i < MAX_COMANDS; i++){
        comands[i] = malloc(MAX_CH * sizeof(char));
        switch(i){
            case 0 :
                comands[i]="authors";
                break;
            case 1 :
                comands[i]="pid";
                break;
            case 2 :
                comands[i]="chdir";
                break;
            case 3 :
                comands[i]="date";
            break;
            case 4 :
                comands[i]="time";
                break;
            case 5 :
                comands[i]="hist";
                break;
            case 6 :
                comands[i]="comand";
                break;
            case 7 :
                comands[i]="open";
                break;
            case 8 :
                comands[i]="close";
                break;
            case 9 :
                comands[i]="dup";
                break;
            case 10 :
                comands[i]="listopen";
                break;
            case 11 :
                comands[i]="infosys";
                break;
            case 12 :
                comands[i]="help";
                break;
            case 13 :
                comands[i]="quit";
                break;
            case 14 :
                comands[i]="exit";
                break;
            case 15 :
                comands[i]="bye";
                break;
            default:
                break;
        }
    }
}

int main(){
    char input[MAX_CH];
    char *input_trozos[MAX_TROZOS], *comands[MAX_COMANDS];
    int num_input;
    bool terminado;
    listHist H;
    listFiles F;

    createEmptyListF(&F);
    createEmptyListH(&H);

    for (int i = 0; i < MAX_TROZOS; i++)
        input_trozos[i] = malloc(MAX_CH * sizeof(char));
    insertComands(comands);

    terminado = false;

    while(!terminado){
        imprimirPromt();
        leerEntrada(input);
        procesarEntrada(input, input_trozos,comands, terminado, &H, &F);
    }

    for (int i = 0; i < MAX_TROZOS; i++) {
        free(input_trozos[i]);
    }
    for (int i = 0; i < MAX_COMANDS; i++) {
        free(comands[i]);
    }

    return 0;
}
