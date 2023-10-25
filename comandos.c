#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>// Para la función open
#include <unistd.h>// Para la función close
#include <time.h>
#include <sys/utsname.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>

#include "comandos.h"

tPosL findFiles(int df, tList L){
    if(L != NULL){
        tPosL p = first(L);
        FileInfo *f;
        f = (FileInfo*)(getItem(p));
        while(p!=NULL && f->df != df){
            p=next(p);
            f = (FileInfo*)(getItem(p));   
        }
        return p;
    }
    return NULL;
    
}

void ListOpenFiles(tList L) {
    tPosL p = first(L);
    FileInfo *x;
    char aux[15];
    while (p != NULL) {
        x = (FileInfo*)(getItem(p));
        if (x->modo == 0100) strcpy(aux, "O_CREAT");
        else if (x->modo == 0200) strcpy(aux, "O_EXCL");
        else if (x->modo == 0) strcpy(aux, " O_RDONLY");
        else if (x->modo == 01) strcpy(aux, "O_WRONLY");
        else if (x->modo == 02) strcpy(aux, "O_RDWR");
        else if (x->modo == 02000) strcpy(aux, "O_APPEND");
        else if (x->modo == 01000) strcpy(aux, "O_TRUNC");
        printf("decriptor: %d -> %s %s\n", x->df, x->name, aux);
        p = next(p);
    }
}

void Cmd_open (char * tr[], tList *L) {
    int i, df, mode = 0;

    if (tr[0] == NULL) {
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
        FileInfo *f = malloc(sizeof(FileInfo));
        size_t fileSize = sizeof(FileInfo);
        f->df = df;
        f->modo = mode;
        strcpy(f->name, tr[0]);
        if(!insertItem(f, fileSize, L)){
            perror("Imposible abrir fichero");
            close(df);
            free(f);
        }
        free(f);
        printf("Anadida entrada %d a la tabla ficheros abiertos\n", df);
    }
}
void Cmd_close (char *tr[], tList *L){
    int df;
    
    if(tr[0]==NULL){
        ListOpenFiles(*L);
        return;
    }
    else{
        if ((df=atoi(tr[0]))<0 || !isdigit(tr[0][0])){
            ListOpenFiles(*L);
            return;
        }        
    }
    
    if (close(df)==-1)
        perror("Imposible cerrar descriptor");
    else{
        tPosL p = findFiles(df, *L);
        deleteItem(p, L);
    }
        
}

void Cmd_dup (char * tr[], tList *L){
    int df, duplicado;
    char aux[MAXNAME+50];
    FileInfo *f = malloc(sizeof(FileInfo));
    size_t fileSize = sizeof(FileInfo);

    if(tr[0]==NULL){
        ListOpenFiles(*L);
        return;
    }
    else{
        if ((df=atoi(tr[0]))<0 || !isdigit(tr[0][0])){
            ListOpenFiles(*L);
            return;
        }
    }
    memcpy(f, (FileInfo*)getItem(findFiles(df, *L)), fileSize);
    sprintf (aux,"dup %d (%s)",df, f->name);
    duplicado = dup(df);
    if (duplicado == -1) {
        perror("Error al duplicar el archivo");
        return;
    }
    f->df = df + 1;
    f->modo = fcntl(duplicado,F_GETFL);
    strcpy(f->name,aux);
    if(!insertItem(f, fileSize, L)){
        perror("Error al duplicar el archivo");
        close(df);
        free(f);
    }
    free(f);
}


void authors(char *input_trozos[], int n){
    if(n == 1)
        printf("fernado.losada@udc.es: Fernando Losada Perez\nmanel.mfernandez@udc.es: Manel Mato Fernandez\n");
    else{
        if(strcmp(input_trozos[1], "-l") == 0)
        printf("fernado.losada@udc.es\nmanel.mfernandez@udc.es\n");

        if(strcmp(input_trozos[1], "-n") == 0)
            printf("Fernando Losada Perez\nManel Mato Fernandez\n");
    }
}

void date(){
    struct tm *fecha_aux;
    time_t fecha;
    time(&fecha);
    fecha_aux = localtime(&fecha);
    printf("%02d/%02d/%04d\n", fecha_aux->tm_mday, fecha_aux->tm_mon + 1, fecha_aux->tm_year + 1900);
}

void tiempo(){
    time_t hora;
    struct tm *time_aux;
    time(&hora);
    time_aux = localtime(&hora);

    printf("%02d:%02d:%02d\n",time_aux->tm_hour, time_aux->tm_min, time_aux->tm_sec);
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

void pid(char *input_trozos[], int n){
    if(n == 1){
        pid_t pid_aux = getpid();
        printf("Pid del proceso del shell: %d\n", pid_aux);
    }
    else {
        if(strcmp(input_trozos[1], "-p") == 0){
            pid_t ppid_aux = getppid();
            printf("Pid del proceso padre del shell: %d\n", ppid_aux);
        }
    }
}

void chdir_func(char *input_trozos[], int n){
    if(n > 1){
        if (chdir(input_trozos[1]) != 0)
            perror("No ha sido posible cambiar el directorio");

    } else{
        char cd[512];
        if (getcwd(cd, sizeof(cd)) != NULL)
            printf("%s\n", cd);
        else
            perror("No se ha podido obtener el directorio actual de trabajo");
    }
}

bool repeat_command(char *input_trozos[], int n, tList H, char *cadena, int *bucle){
    tPosL p;
    char *comand;
    int i = 0;
    if(n == 1){
        for(p = first(H); p!= NULL; p = next(p)){
            comand=(char*)getItem(p);
            printf("%d->%s",i,comand);
            i++;
        }
        return false;
    }
    else if(isdigit(input_trozos[1][0])){
        int num = atoi(input_trozos[1]);
        p = first(H);
        while( p!= NULL && i<num){
            i++;
            p = next(p);
        }
        if(p == NULL){
            printf("No hay elemento %d en el historico\n", num);
            return false;
        }
        else{
            comand = (char*)getItem(p);
            printf("Ejecutando hist (%d): %s", num, comand);
            if(strcmp(comand, cadena)==0)
                (*bucle)++;
            else
                *bucle = 0;
            if(*bucle >= 10 && strcmp(comand, cadena)==0){
                printf("Demasiada percusion en hist\n");
                *bucle=0;
                return false;
            }
            strcpy(cadena, comand);
            return true;
        }
            
    }
    else
        return false;
}



void hist(char *input_trozos[], int n, tList *H){
    tPosL p;
    char *comand;
    int i = 0;
    if(n == 1){
        for(p = first(*H); p!= NULL; p = next(p)){
            comand = (char *)getItem(p);
            printf("%d->%s",i,comand);
            i++;
        }
    }
    else{
        if(input_trozos[1][0] == '-'){
            if(input_trozos[1][1] == 'c')
                deleteList(H);
            else if(isdigit(input_trozos[1][1])){
                int n = atoi(input_trozos[1] + 1);
                p = first(*H);
                while( p!= NULL && i<n){
                    comand=(char*)getItem(p);
                    printf("%d->%s",i,comand);
                    i++;
                    p = next(p);
                }
            }
        }
    }
    
}

void help(char *commands[], char *input_trozos[], int n, int nComands){
    int i;

    if(n > 1) {
        for (i = 0; i < nComands && strcmp(commands[i], input_trozos[1]) != 0; i++);
        switch (i) {
            case 0 :
                printf("authors [-n|-l]\t Muestra los nombres y/o logins de los autores\n");
                break;
            case 1:
                printf("pid [-p]\t Muestra el pid del shell o de su proceso padre\n");
                break;
            case 2:
                printf("chdir [dir]\t	Cambia (o muestra) el directorio actual del shell\n");
                break;
            case 3:
                printf("date\tMuestra la fecha acual\n");
                break;
            case 4:
                printf("time\tMuestra la hora actual\n");
                break;
            case 5:
                printf("hist [-c|-N]\tMuestra (o borra)el historico de comandos\n"
                       "\t-N: muestra los N primeros\n"
                       "\t-c: borra el historico\n");
                break;
            case 6:
                printf("command [-N]\tRepite el comando N (del historico)\n");
                break;
            case 7:
                printf("open fich m1 m2...\tAbre el fichero fich. y lo anade a la lista de ficheros abiertos del shell\n"
                       "\t m1, m2..es el modo de apertura (or bit a bit de los siguientes).\n"
                       "\tcr: O_CREAT\tap: O_APPEND\n"
                       "\tex: O_EXCL \tro: O_RDONLY\n"
                       "\trw: O_RDWR \two: O_WRONLY\n"
                       "\ttr: O_TRUNC\n");
                break;
            case 8:
                printf("lose df\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");
                break;
            case 9:
                printf("dup df\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n");
                break;
            case 10:
                printf("listopen [n]\tLista los ficheros abiertos (al menos n) del shell\n");
                break;
            case 11:
                printf("infosys \tMuestra informacion de la maquina donde corre el shell\n");
                break;
            case 12:
                printf("help [cmd|-lt|-T]\tMuestra ayuda sobre los comandos\n"
                       "\t-lt: lista topics de ayuda\n"
                       "\t-T topic: lista comandos sobre ese topic\n"
                       "\tcmd: info sobre el comando cmd\n");
                break;
            case 13:
                printf("quit \tTermina la ejecucion del shell\n");
                break;
            case 14:
                printf("exit \tTermina la ejecucion del shell\n");
                break;
            case 15:
                printf("bye \tTermina la ejecucion del shell\n");
                break;
            case 16:
                printf("create [-f] [name] \tCrea un directorio o un fichero (-f)\n");
                break;
            case 17:
                printf("stat [-long][-link][-acc] name1 name2 .. \tlista ficheros;\n"
                    "\t-long: listado largo\n"
                    "\t-acc: acesstime\n"
                    "\t-link: si es enlace simbolico, el path contenido\n");
                break;
            case 18:
                printf("\n");
                break;
            case 19:
                printf("\n");
                break;
            case 20:
                printf("\n");
                break;
            case 21:
                printf("bye \tTermina la ejecucion del shell\n");
                break;
            default :
                printf("%s no encontrado\n", input_trozos[1]);
                break;
        }
    }else{
        for( i = 0; i < nComands; i++)
            printf("%s ", commands[i]);
        printf("\n");
    }
}


/*char LetraTF(mode_t m) {//FUNCION AUXILIAR
    switch (m & S_IFMT) {
        case S_IFSOCK:
            return 's';
        case S_IFLNK:
            return 'l';
        case S_IFREG:
            return '-';
        case S_IFBLK:
            return 'b';
        case S_IFDIR:
            return 'd';
        case S_IFCHR:
            return 'c';
        case S_IFIFO:
            return 'p';
        default:
            return '?';
    }
}

char *ConvierteModo2(mode_t m) {//FUNCION AUXILIAR
    static char permisos[12];
    strcpy(permisos, "---------- ");

    permisos[0] = LetraTF(m);
    if (m & S_IRUSR) permisos[1] = 'r';
    if (m & S_IWUSR) permisos[2] = 'w';
    if (m & S_IXUSR) permisos[3] = 'x';
    if (m & S_IRGRP) permisos[4] = 'r';
    if (m & S_IWGRP) permisos[5] = 'w';
    if (m & S_IXGRP) permisos[6] = 'x';
    if (m & S_IROTH) permisos[7] = 'r';
    if (m & S_IWOTH) permisos[8] = 'w';
    if (m & S_IXOTH) permisos[9] = 'x';
    if (m & S_ISUID) permisos[3] = 's';
    if (m & S_ISGID) permisos[6] = 's';
    if (m & S_ISVTX) permisos[9] = 't';

    return permisos;
}
*/

void Stat(char *input_trozos[]) {
 int i, j;
    bool commands = false,  Long = false, link = false, acc = false;

    for (j = 1; input_trozos[j] != NULL; j++) {
        if (strcmp(input_trozos[j], "-long") == 0) {
            Long = true;
            commands = true;
        } else if (strcmp(input_trozos[j], "-link") == 0) {//CAMBIAR LINK
            link = true;
            commands = true;
        } else if (strcmp(input_trozos[j], "-acc") == 0) {
            acc = true;
            commands = true;
        }else {
            break;
        }
    }

    if (input_trozos[j] == NULL) {
        char ruta[1024];

        if (getcwd(ruta, sizeof(ruta)) != NULL)
            printf("%s\n", ruta);
        else
            perror("Error al obtener la ruta\n");
        return;
    }

    for (i = j; input_trozos[i] != NULL; i++) {
        struct stat file_stat;
        if (stat(input_trozos[i], &file_stat) == -1) {
            printf(" ****error al acceder a %s :No such file or directory\n", input_trozos[i]);
            continue;
        }

        if (!commands) {
            printf("\t%lld\t%s\n", (long long) file_stat.st_size, input_trozos[i]);
        }if (Long) {
            struct passwd *pwd = getpwuid(file_stat.st_uid);
            /* printf("%s\t%ld\t(%ld)\t%s\t%s\t%lld\t%s\n", ctime(&file_stat.st_atime),
                   file_stat.st_nlink, file_stat.st_ino, pwd->pw_name,
                   ConvierteModo2(file_stat.st_mode), (long long) file_stat.st_size, input_trozos[i]); */
        } if (acc) {
            printf("%s%-8lld%-4s\n", ctime(&file_stat.st_atime), (long long) file_stat.st_size, input_trozos[i]);
        } if (link) {
            printf("%ld (%ld) %-8lld %-4s\n", file_stat.st_nlink, file_stat.st_ino, (long long) file_stat.st_size,
                   input_trozos[i]);
        }
    }

}

void listDirectoryB(const char *dirPath) {//FUNCION AUXILIAR
    DIR *dir = opendir(dirPath);
    if (!dir) {
        perror("Error al abrir el directorio\n");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;

    while ((entry = readdir(dir)) != NULL) {
        char path[4096]; // Para contener la ruta completa de los elementos
        snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

        if (stat(path, &file_stat) == -1) {
            perror("Error al obtener información del archivo/directorio");
            continue; // Continuar con el siguiente elemento si hay un error
        }
        if (entry->d_name[0] == '.')
            continue;

        if (S_ISDIR(file_stat.st_mode)) {
            // Si es un directorio, imprímelo o procesa según lo necesites
            listDirectoryB(path);
        }
    }
    printf("************%s\n", dirPath);
    rewinddir(dir); // Volver al principio del directorio para procesar archivos


    while ((entry = readdir(dir)) != NULL) {
        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

        if (stat(path, &file_stat) == -1) {
            perror("Error al obtener información del archivo/directorio");
            continue;
        }
        if (entry->d_name[0] == '.')
            continue;

        printf("%ld\t%s\n", file_stat.st_size, entry->d_name);

    }

    closedir(dir);
}

void listDirectoryA(const char *dirPath) {//FUNCION AUXILIAR
    DIR *dir = opendir(dirPath);
    if (!dir) {
        perror("Error al abrir el directorio\n");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;
    printf("************%s\n", dirPath);
    while ((entry = readdir(dir)) != NULL) {
        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

        if (stat(path, &file_stat) == -1) {
            perror("Error al obtener información del archivo/directorio");
            continue;
        }
        if (entry->d_name[0] == '.')
            continue;

        printf("%ld\t%s\n", file_stat.st_size, entry->d_name);
    }

    rewinddir(dir); // Volver al principio del directorio para procesar directorios

    while ((entry = readdir(dir)) != NULL) {
        char path[4096]; // Para contener la ruta completa de los elementos
        snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

        if (stat(path, &file_stat) == -1) {
            perror("Error al obtener información del archivo/directorio");
            continue; // Continuar con el siguiente elemento si hay un error
        }
        if (entry->d_name[0] == '.')
            continue;

        if (S_ISDIR(file_stat.st_mode)) {
            // Si es un directorio, imprímelo o procesa según lo necesites
            listDirectoryA(path);
        }
    }
    closedir(dir);
}

void List(char *input_trozos[]) {
    struct stat file_stat;
    DIR *dir;
    int i;
    struct dirent *content;
    char rutaAbsoluta[4096];
    bool commands = false, Long = false, link = false, acc = false, recb = false, reca = false, hid = false;

    for (j = 1; input_trozos[j] != NULL; j++) {
        if (strcmp(input_trozos[j], "-long") == 0) {
            Long = true;    
            commands = true;
        } else if (strcmp(input_trozos[j], "-link") == 0) {//CAMBIAR LINK
            link = true;
            commands = true;
        } else if (strcmp(input_trozos[j], "-acc") == 0) {
            acc = true;
            commands = true;
        } else if (strcmp(input_trozos[j], "-recb") == 0) {
            recb = true;
            commands = true;
        } else if (strcmp(input_trozos[j], "-reca") == 0) {
            reca = true;
            commands = true;
        } else if (strcmp(input_trozos[j], "-hid") == 0) {
            hid = true;
            commands = true;
        } else {
            break;
        }
    }

    if (input_trozos[j] == NULL) {
        char ruta[1024];
        if (getcwd(ruta, sizeof(ruta)) != NULL)
            printf("%s\n", ruta);
        else
            perror("Error al obtener la ruta\n");
        return;
    }

    for (i = j; input_trozos[i] != NULL; i++) {

        dir = opendir(input_trozos[i]);
        if (!dir) {
            printf(" ****error al acceder a %s :No such file or directory\n", input_trozos[i]);
            continue;
        }

        if (!commands) {
            printf("***********%s\n", input_trozos[i]);
            while ((content = readdir(dir)) != NULL) {
                snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                if (stat(rutaAbsoluta, &file_stat) == -1) {
                    perror("Error al obtener información del archivo/directorio"); //PONER COMO EN EL SHELL DE REFERENCIA
                    return;
                }
                if (content->d_name[0] != '.') {
                    printf("\t%lld\t%s\n", (long long) file_stat.st_size, content->d_name);
                }
            }
            closedir(dir);
        }
        if (Long) {
            struct passwd *pwd;
            printf("***********%s\n", input_trozos[i]);
            while ((content = readdir(dir)) != NULL) {
                snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                if (stat(rutaAbsoluta, &file_stat) == -1) {
                    perror(""); //PONER COMO EN EL SHELL DE REFERENCIA
                    return;
                }
                if (content->d_name[0] != '.') {
                    pwd = getpwuid(file_stat.st_uid);
                    /* printf("%s\t%ld\t(%ld)\t%s\t%s\t%lld\t%s\n", ctime(&file_stat.st_atime), file_stat.st_nlink,
                            file_stat.st_ino, pwd->pw_name, ConvierteModo2(file_stat.st_mode), (long long) file_stat.st_size,
                            content->d_name);
                            */
                }
            }
            rewinddir(dir);
        }
        if (acc) {
            printf("***********%s\n", input_trozos[i]);
            while ((content = readdir(dir)) != NULL) {
                snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                if (stat(rutaAbsoluta, &file_stat) == -1) {
                    perror("Error al obtener información del archivo/directorio\n"); //PONER COMO EN EL SHELL DE REFERENCIA
                    return;
                }
                if (content->d_name[0] != '.') {
                    printf("%s\t%lld\t%s\n", ctime(&file_stat.st_atime), (long long) file_stat.st_size,
                           content->d_name);
                }
            }
            rewinddir(dir);
        }
        if (link) {//CAMBIAR LINK
            printf("***********%s\n", input_trozos[i]);
            while ((content = readdir(dir)) != NULL) {
                snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                if (stat(rutaAbsoluta, &file_stat) == -1) {
                    perror("Error al obtener información del archivo/directorio\n"); //PONER COMO EN EL SHELL DE REFERENCIA
                    return;
                }
                if (content->d_name[0] != '.') {
                    printf("%ld (%ld)\t%lld\t%s\n", file_stat.st_nlink, file_stat.st_ino, (long long) file_stat.st_size,
                           content->d_name);
                }
            }
            rewinddir(dir);
        }
        if (recb) {
            listDirectoryB(input_trozos[i]);
            rewinddir(dir);
        }
        if (reca) {
            listDirectoryA(input_trozos[i]);
            rewinddir(dir);
        }
        if (hid) {
            printf("***********%s\n", input_trozos[i]);
            while ((content = readdir(dir)) != NULL) {
                snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                if (stat(rutaAbsoluta, &file_stat) == -1) {
                    perror("Error al obtener información del archivo/directorio"); //PONER COMO EN EL SHELL DE REFERENCIA
                    return;
                }
                printf("\t%lld\t%s\n", (long long) file_stat.st_size, content->d_name);
            }
            rewinddir(dir);
        }
    }
    closedir(dir);
}



   
}


void quit(bool *terminado){
    *terminado = true;
}

void exit_func(bool *terminado){
    *terminado = true;
}

void bye(bool *terminado){
    *terminado = true;
}
