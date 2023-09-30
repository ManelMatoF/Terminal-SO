
//Fernando Losada Pérez fernando.losada@udc.es
//Manel Mato Fernández manel.mfernandez@udc.es
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

int TrocearCadena(char * cadena, char * trozos[])
{ int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

int encontrarComands(char *input_trozos[], char *comands[]) {
    for (int i = 0; i < MAX_COMANDS; i++) {
        int j;
        for (j = 0; input_trozos[j] != NULL && comands[i] != NULL; j++) {
            if (strcmp(comands[i], input_trozos[j]) != 0) {
                break;  // Las cadenas son diferentes, salir del bucle interno
            }
        }
        if (input_trozos[j] == NULL && comands[i] == NULL) {
            // Todas las cadenas coincidieron, este es el comando
            return i;
        }
    }
    return -1;  // Comando no encontrado
}


void procesarEntrada(char *cadena, char *input_trozos[],char *comands[], bool *terminado, listHist *H, listFiles *F){
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
            if(repeat_command(input_trozos, *H, cadena))
                procesarEntrada(cadena, input_trozos, comands, terminado, H, F);//probar
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
            quit(terminado);
            break;
        case 14:
            exit_func(terminado);
            break;
        case 15:
            bye(terminado);
            break;
        default:
            printf("No ejecutado: No such file or directory\n");
            break;
    }
}

void insertComands(char *comands[]) {

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
    }

int main(){
    char input[MAX_CH];
    char *input_trozos[MAX_TROZOS], *comands[MAX_COMANDS];
    bool terminado = false;

    listHist H;
    listFiles F;
    createEmptyListF(&F);
    createEmptyListH(&H);

    for (int i = 0; i < MAX_TROZOS; i++)
        input_trozos[i] = malloc(MAX_CH * sizeof(char));

    for (int i = 0; i < MAX_COMANDS; i++)
        comands[i] = malloc(MAX_CH * sizeof(char));

    insertComands(comands);

    while(!terminado){
        imprimirPromt();
        leerEntrada(input);
        procesarEntrada(input, input_trozos,comands, &terminado, &H, &F);
    }

    for (int i = 0; i < MAX_TROZOS; i++) {
        free(input_trozos[i]);
    }
    for (int i = 0; i < MAX_COMANDS; i++) {
        free(comands[i]);
    }

    return 0;
}
