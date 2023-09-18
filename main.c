#include <stdio.h>
#include <string.h>

#define MAX 100

void imprimirPromt(){
    printf("->");
}

void leerEntrada(char * input){
    fgets(input, 100, stdin);
}

int TrocearCadena(char *cadena, char *trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

void procesarEntrada(char *input_trozos[], int num_input, int terminado){
    switch (*input_trozos[0]){
        if(strcmp(*input_trozos[0], "authors\0"))
            printf("hola");
        else if(strcmp(*input_trozos[0], "date\0"))
            printf("adios");
        else
            printf("Opcion no valida\n");
    }
    
}


int main(){
    char * input; 
    char * input_trozos[MAX];
    int num_input, terminado;

    terminado = 0;

    while(!terminado){
        imprimirPromt();
        leerEntrada(input);
        procesarEntrada(input_trozos, num_input, terminado);
    }
}
