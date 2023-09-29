#include "comandos.h"
#include "lista.h"
#include "lista.c"

void Cmd_open (char * tr[], listFiles *L) {
    int i, df, mode = 0, numFilesOpen;

    if (tr[1] == NULL) {
        ListOpenFiles(*L);
        return;
    }
    for (i = 1; tr[i] != NULL; i++)
        if (!strcmp(tr[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(tr[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(tr[i], "ro")) mode |= O_RDONLY;
        else if (!strcmp(tr[i], "wo")) mode |= O_WRONLY;
        else if (!strcmp(tr[i], "rw")) mode |= O_RDWR;
        else if (!strcmp(tr[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(tr[i], "tr")) mode |= O_TRUNC;
        else break;

    if ((df = open(tr[0], mode, 0777)) == -1)
        perror("Imposible abrir fichero");
    else {
        /*...........AnadirAFicherosAbiertos (descriptor...modo...nombre....)....*/
        insertFile(df, mode, tr[0], L);
        numFilesOpen = countFiles(L);
        printf("Anadida entrada %d a la tabla ficheros abiertos..................", numFilesOpen - 1);
    }
}
void Cmd_close (char *tr[], listFiles *L)
{
    int df;

    if (tr[0]==NULL || (df=atoi(tr[0]))<0) {
        ListOpenFiles(*L);
        return;
    }


    if (close(df)==-1)
        perror("Imposible cerrar descriptor");
    else
        CloseOpenFile(L,df);
}

void Cmd_dup (char * tr[], listFiles *L){
    int df, duplicado;
    char aux[MAXNAME],*p;

    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { /*no hay parametro*/ /*o el descriptor es menor que 0*/
        perror("No es posible duplicar el archivo");
        return;
    }
    p = getItemF(findItemF(df, *L), *L);
    sprintf (aux,"dup %d (%s)",df, p);
    insertItemF(df, fcntl(duplicado,F_GETFL), p, L); //CAMBIAR LA ESTRUCTURA FILEINFO
    dup(df);

    }


void authors(char *input_trozos[]){
    if(strcmp(input_trozos[1], "-n") == 0)
        printf("fernado.losada@udc.es\nmanel.mfernandez@udc.es\n");

    else if(strcmp(input_trozos[1], "-l") == 0)
        printf("Fernando Losada Perez\nManel Mato Fernandez\n");
    else printf("fernado.losada@udc.es: Fernando Losada Perez\nmanel.mfernandez@udc.es: Manel Mato Fernandez\n");
}

void date(){
    struct tm *fecha_aux;
    time_t fecha;
    time(&fecha);
    fecha_aux = localtime(&fecha);

    printf("%02d/%02d/%02d",fecha_aux->tm_mday, fecha_aux->tm_mon, fecha_aux->tm_year);
}

void tiempo(){
    time_t hora;
    struct tm *time_aux;
    time(&hora);
    time_aux = localtime(&hora);

    printf("%02d:%02d:%02d",time_aux->tm_hour, time_aux->tm_min, time_aux->tm_sec);
}

void infosys(){
    struct utsname info_sys;

    if (uname(&info_sys) != -1){
        printf("Informacion  de la maquina %s\n", info_sys.machine);
        printf("Host Name: %s\n", info_sys.nodename);
        printf("Version del Kernel: %s\n", info_sys.release);
        printf("Sistema Operativo: %s\n", info_sys.sysname);
        printf("Version del sistema: %s\n", info_sys.version);

    } else {
        perror("No se ha podido obtener la informacion del sistema");
    }
}

void pid(char *input_trozos[]){
    if(strcmp(input_trozos[1], "-p") == 0){
        pid_t ppid_aux = getppid();
        printf("Pid del proceso padre del shell: %d\n", ppid_aux);
    } else {
        pid_t pid_aux = getpid();
        printf("Pid del proceso del shell: %d\n", pid_aux);
    }
}

void chdir_func(char *input_trozos[]){
    if(strcmp(input_trozos[1], "") != 0){
        if (chdir(input_trozos[1]) == 0)
            printf("\n");
        else
            perror("No ha sido posible cambiar el directorio\n");

    } else{
        char cd[512];
        if (getcwd(cd, sizeof(cd)) != NULL)
            printf("%s\n", cd);
        else
            perror("No se ha podido obtener el directorio actual de trabajo");
    }
}

bool *repeat_command(char *input_trozos[], listHist H, char *cadena){
    tPosH p;
    char *comand;
    int i = 0;
    if(strcmp(input_trozos[1], "") == 0){
        for(p = firstH(H); p!= NULL; p = nextH(p, H)){
            comand=getItemH(p, H);
            printf("%d->%s",i,comand);
            i++;
        }
        return false;
    }
    else if(isdigit(input_trozos[1][0])){
        int n = atoi(input_trozos[1]);
        p = firstH(H);
        while( p!= NULL && i<n){
            i++;
            p = nextH(p, H);
        }
        if(p==NULL){
             printf("No hay elemento %d en el historico", n);
            return false;
        }
        else{
            comand = getItemH(p, H);
            printf("Ejecutando hist (%d): %s", n, comand);
            cadena = comand;
            return true;
        }
        
    }
    else
        return false;
}



void hist(char *input_trozos[], listHist *H){
    tPosH p;
    char *comand;
    int i = 0;
    if(strcmp(input_trozos[1], "") == 0){
        for(p = firstH(*H); p!= NULL; p = nextH(p, *H)){
            comand=getItemH(p, *H);
            printf("%d->%s",i,comand);
            i++;
        }
    }
    if(strcmp(input_trozos[1], "-") == 0){
        if(input_trozos[1][1] == 'c')
            deleteListH(H);
        if(isdigit(input_trozos[1][1])){
            int n = atoi(input_trozos[1] + 1);
            p = firstH(*H);
            while( p!= NULL && i<n){
                comand=getItemH(p, *H);
                printf("%d->%s",i,comand);
                i++;
                p = nextH(p, *H);
            }
        }
    }
}

void help(char *commands[], char *input_trozos[], int nComands){
    int i;

    if(strcmp(input_trozos[1], "") != 0) {
        for (i = 0; i < nComands && strcmp(commands[i], input_trozos[1]) != 0; i++);
        switch (i) {
            case 0 :
                printf("authors [-n|-l]\t Muestra los nombres y/o logins de los autores\n");
                break;
            case 1:
                printf("id [-p]\t Muestra el pid del shell o de su proceso padre\n");
                break;
            case 2:
                printf("chdir [dir]\t	Cambia (o muestra) el directorio actual del shell\n");
                break;
            case 3:
                printf("date\tMuestra la fecha acual");
                break;
            case 4:
                printf("time\tMuestra la hora actual");
                break;
            case 5:
                printf("hist [-c|-N]\tMuestra (o borra)el historico de comandos\n"
                       "\t-N: muestra los N primeros\n"
                       "\t-c: borra el historico");
                break;
            case 6:
                printf("command [-N]\tRepite el comando N (del historico)");
                break;
            case 7:
                printf("open fich m1 m2...\tAbre el fichero fich. y lo anade a la lista de ficheros abiertos del shell\n"
                       "\t m1, m2..es el modo de apertura (or bit a bit de los siguientes).\n"
                       "\tcr: O_CREAT\tap: O_APPEND\n"
                       "\tex: O_EXCL \tro: O_RDONLY\n"
                       "\trw: O_RDWR \two: O_WRONLY\n"
                       "\ttr: O_TRUNC");
                break;
            case 8:
                printf("lose df\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos");
                break;
            case 9:
                printf("dup df\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos");
                break;
            case 10:
                printf("listopen [n]\tLista los ficheros abiertos (al menos n) del shell");
                break;
            case 11:
                printf("infosys \tMuestra informacion de la maquina donde corre el shell");
                break;
            case 12:
                printf("help [cmd|-lt|-T]\tMuestra ayuda sobre los comandos\n"
                       "\t-lt: lista topics de ayuda\n"
                       "\t-T topic: lista comandos sobre ese topic\n"
                       "\tcmd: info sobre el comando cmd");
                break;
            case 13:
                printf("quit \tTermina la ejecucion del shell");
                break;
            case 14:
                printf("exit \tTermina la ejecucion del shell");
                break;
            case 15:
                printf("bye \tTermina la ejecucion del shell");
                break;
            default :
                printf("%s no encontrado", input_trozos[1]);
                break;
        }
    }else{
        for( i = 0; i < nComands; i++)
            printf("%s ", commands[i]);
    }
}


void quit(bool terminado){
    terminado = true;
}

void exit_func(bool terminado){
    terminado = true;
}

void bye(bool terminado){
    terminado = true;
}
