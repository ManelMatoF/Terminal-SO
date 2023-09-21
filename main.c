#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100

void imprimirPromt(){
    printf("->");
}

void leerEntrada(char * input){
    fgets(input, MAX, stdin);//cuantos caracteres leer (preguntar)
}

int TrocearCadena(char *cadena, char *trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

void procesarEntrada(char *cadena, char *input_trozos[], int num_input, int terminado){//Lista de comandos con switch o if else (preguntar)
    TrocearCadena(cadena, input_trozos);
    if(strcmp(input_trozos[0], "authors") == 0)
        printf("hola\n");
    else if(strcmp(input_trozos[0], "date") == 0)
        printf("adios\n");
    else
        printf("Opcion no valida\n");
}


int main(){
    char input[MAX]; //preguntar si asi o con el malloc (punteros)
    char *input_trozos[10];
    int num_input, terminado;

    for (int i = 0; i < 10; i++) {
        input_trozos[i] = malloc(MAX * sizeof(char));  // Allocate memory for each string in input_trozos
    }
    terminado = 0;

    while(!terminado){
        imprimirPromt();
        leerEntrada(input);
        procesarEntrada(input, input_trozos, num_input, terminado);
    }

    for (int i = 0; i < 10; i++) {
        free(input_trozos[i]);
    }
