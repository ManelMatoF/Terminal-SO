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
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>

#include "comandos.h"

tList H, C, F;

int TrocearCadena(char *cadena, char **input_trozos){
    int i=1;
    if ((input_trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((input_trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

tPosL encontrarComando(char **input_trozos) {
    if(C != NULL){
        tPosL p = first(C);
        Comands *c;
        c = (Comands*)(getItem(p));
        while(p!=NULL && strcmp(c->comand, input_trozos[0])!=0){
            p=next(p);
            if(p!=NULL)
                c = (Comands*)(getItem(p));   
        }
        return p;
    }
    return NULL;
}

void aux_procesarEntrada(char *input, bool *terminado){
    char copy_input[MAXNAME];
    char **input_trozos = (char **)malloc(MAX_TROZOS * sizeof(char*));
    strcpy(copy_input, input);
    int input_num = TrocearCadena(copy_input, input_trozos);
    if(input_num > 0){
        tPosL cmd = encontrarComando(input_trozos);
        if(cmd == NULL){
            printf("No ejecutado: No such file or directory\n");
        }
        else{
            Comands *c;
            c = (Comands*)(getItem(cmd));
            c->funcion(input, input_trozos, input_num, terminado);
        }      
    }  
    free(input_trozos);
}

tPosL findFiles(int df){
    if(F != NULL){
        tPosL p = first(F);
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

void ListOpenFiles(char *input, char *input_trozos[], int n, bool *terminado){
    if(F != NULL){
        tPosL p = first(F);
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
}

void Cmd_open (char *input, char *input_trozos[], int n, bool *terminado) {
    int i, df, mode = 0;

    if (input_trozos[1] == NULL) {
        ListOpenFiles(input, input_trozos, n, terminado);
        return;
    }
    for (i = 2; input_trozos[i] != NULL; i++)
    if (!strcmp(input_trozos[i], "cr")) mode |= O_CREAT;
    else if (!strcmp(input_trozos[i], "ex")) mode |= O_EXCL;
    else if (!strcmp(input_trozos[i], "ro")) mode |= O_RDONLY;
    else if (!strcmp(input_trozos[i], "wo")) mode |= O_WRONLY;
    else if (!strcmp(input_trozos[i], "rw")) mode |= O_RDWR;
    else if (!strcmp(input_trozos[i], "ap")) mode |= O_APPEND;
    else if (!strcmp(input_trozos[i], "tr")) mode |= O_TRUNC;
    else break;

    if ((df = open(input_trozos[1], mode, 0777)) == -1)
        perror("Imposible abrir fichero");
    else {
        FileInfo *f = malloc(sizeof(FileInfo));
        size_t fileSize = sizeof(FileInfo);
        f->df = df;
        f->modo = mode;
        strcpy(f->name, input_trozos[1]);
        if(!insertItem(f, fileSize, &F)){
            perror("Imposible abrir fichero");
            close(df);
            free(f);
        }
        printf("Anadida entrada %d a la tabla ficheros abiertos\n", df);
        free(f);
    }
}
void Cmd_close (char *input, char *input_trozos[], int n, bool *terminado){
    int df;
    
    if(input_trozos[1]==NULL){
        ListOpenFiles(input, input_trozos, n, terminado);
        return;
    }
    else{
        if ((df=atoi(input_trozos[1]))<0 || !isdigit(input_trozos[1][0])){
            ListOpenFiles(input, input_trozos, n, terminado);
            return;
        }        
    }
    
    if (close(df)==-1)
        perror("Imposible cerrar descriptor");
    else{
        tPosL p = findFiles(df);
        deleteItem(p, &F);
    }
        
}

void Cmd_dup (char *input, char *input_trozos[], int n, bool *terminado){
    int df, duplicado;
    char aux[MAXNAME+50];
    
    if(input_trozos[1]==NULL){
        ListOpenFiles(input, input_trozos, n, terminado);
        return;
    }
    else{
        if ((df=atoi(input_trozos[1]))<0 || !isdigit(input_trozos[1][0])){
            ListOpenFiles(input, input_trozos, n, terminado);
            return;
        }
    }
    duplicado = dup(df);
    if (duplicado == -1) {
        perror("Error al duplicar el archivo");
        return;
    }
    FileInfo *f = malloc(sizeof(FileInfo));
    size_t fileSize = sizeof(FileInfo);
    memcpy(f, (FileInfo*)getItem(findFiles(df)), fileSize);
    sprintf (aux,"dup %d (%s)",df, f->name);
    f->df = df + 1;
    f->modo = fcntl(duplicado,F_GETFL);
    strcpy(f->name,aux);
    if(!insertItem(f, fileSize, &F)){
        perror("Error al duplicar el archivo");
        close(df);
        free(f);
        }
    free(f);
}


void authors(char *input, char *input_trozos[], int n, bool *terminado){
    if(n == 1)
        printf("fernado.losada@udc.es: Fernando Losada Perez\nmanel.mfernandez@udc.es: Manel Mato Fernandez\n");
    else{
        if(strcmp(input_trozos[1], "-l") == 0)
        printf("fernado.losada@udc.es\nmanel.mfernandez@udc.es\n");

        if(strcmp(input_trozos[1], "-n") == 0)
            printf("Fernando Losada Perez\nManel Mato Fernandez\n");
    }
}

void date(char *input, char *input_trozos[], int n, bool *terminado){
    struct tm *fecha_aux;
    time_t fecha;
    time(&fecha);
    fecha_aux = localtime(&fecha);
    printf("%02d/%02d/%04d\n", fecha_aux->tm_mday, fecha_aux->tm_mon + 1, fecha_aux->tm_year + 1900);
}

void tiempo(char *input, char *input_trozos[], int n, bool *terminado){
    time_t hora;
    struct tm *time_aux;
    time(&hora);
    time_aux = localtime(&hora);

    printf("%02d:%02d:%02d\n",time_aux->tm_hour, time_aux->tm_min, time_aux->tm_sec);
}

void infosys(char *input, char *input_trozos[], int n, bool *terminado){
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

void pid(char *input, char *input_trozos[], int n, bool *terminado){
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

void chdir_func(char *input, char *input_trozos[], int n, bool *terminado){
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

void repeat_command(char *input, char *input_trozos[], int n, bool *terminado){
    static int bucle;
    tPosL p;
    char *comand;
    int i = 0;
    if(n == 1){
        for(p = first(H); p!= NULL; p = next(p)){
            comand=(char*)getItem(p);
            printf("%d->%s",i,comand);
            i++;
        }
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
            return;
        }
        else{
            comand = (char*)getItem(p);
            printf("Ejecutando hist (%d): %s", num, comand);
            if(strcmp(comand, input)==0)
                bucle++;
            else
                bucle = 0;
            if(bucle >= 10 && strcmp(comand, input)==0){
                printf("Demasiada percusion en hist\n");
                bucle=0;
                return;
            }
            strcpy(input, comand);
            aux_procesarEntrada(input, terminado);
            return;
        }
            
    }
    else
        return;
}



void hist(char *input, char *input_trozos[], int n, bool *terminado){
    tPosL p;
    char *comand;
    int i = 0;
    if(n == 1){
        for(p = first(H); p!= NULL; p = next(p)){
            comand = (char *)getItem(p);
            printf("%d->%s",i,comand);
            i++;
        }
    }
    else{
        if(input_trozos[1][0] == '-'){
            if(input_trozos[1][1] == 'c')
                deleteList(&H);
            else if(isdigit(input_trozos[1][1])){
                int n = atoi(input_trozos[1] + 1);
                p = first(H);
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

tPosL aux_help(char *input) {
    if(C != NULL){
        tPosL p = first(C);
        Comands *c;
        c = (Comands*)(getItem(p));
        while(p!=NULL && strcmp(c->comand, input)!=0){
           p=next(p);
           c = (Comands*)(getItem(p));   
        }
        return p;
    }
    return NULL;
}

void help(char *input, char *input_trozos[], int n, bool *terminado){
    tPosL cmd;
    Comands *c;
        
    if(n > 1) {
        cmd = aux_help(input_trozos[1]);
        if(cmd != NULL){
            c = (Comands*)(getItem(cmd));
            printf("%s", c->help_comand);
        }
        else
            printf("%s no encontrado\n", input_trozos[1]);
        
    }else{
        printf("\tComandos disponibles:\n");
        for( cmd = first(C); cmd != NULL; cmd = next(cmd)){
            c = (Comands*)(getItem(cmd));
            printf("%s ", c->comand);
        }
        printf("\n");
    }
}

void quit(char *input, char *input_trozos[], int n, bool *terminado){
    *terminado = true;
}

void exit_func(char *input, char *input_trozos[], int n, bool *terminado){
    *terminado = true;
}

void bye(char *input, char *input_trozos[], int n, bool *terminado){
    *terminado = true;
}

void create(char *input, char *input_trozos[], int n, bool *terminado){
    if(n==1){
        char cd[512];
        if (getcwd(cd, sizeof(cd)) != NULL)
            printf("%s\n", cd);
        else
            perror("No se ha podido obtener el directorio actual de trabajo");
    }
    else{
        if(strcmp(input_trozos[1],"-f")==0){
            int descriptor = open(input_trozos[2], O_WRONLY | O_CREAT | O_EXCL, 0777);
            if (descriptor == -1){
                 perror("Imposible crear");
                return;
            }
            close(descriptor);
        }
        else{
            if (mkdir(input_trozos[1], 0777) == -1) {
                perror("Imposible crear");
                }
        }
    }    
}




/*

char LetraTF (mode_t m)
{
     switch (m & S_IFMT) { 
        case S_IFSOCK: return 's';
        case S_IFLNK: return 'l'; 
        case S_IFREG: return '-'; 
        case S_IFBLK: return 'b'; 
        case S_IFDIR: return 'd'; 
        case S_IFCHR: return 'c';
        case S_IFIFO: return 'p';
        default: return '?';
     }
}

char *ConvierteModo2(mode_t m) {
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
}*/

void Stat_aux(struct stat file_stat, bool acc, bool link, bool Long, bool comands, char *input_trozos[], int i) {//FUNCION AUXILIAR
    if(!comands) {
        printf("\t%lld\t%s\n", (long long) file_stat.st_size, input_trozos[i]);
    }
    if (Long) {
        struct passwd *pwd = getpwuid(file_stat.st_uid);
        if (pwd) {
            if (acc) {
                printf("%s", ctime(&file_stat.st_atime));
            } else printf("%s", ctime(&file_stat.st_mtime));

            /* printf("\t%ld\t(%ld)\t%s\t%s\t%lld\t",
             file_stat.st_nlink, file_stat.st_ino, pwd->pw_name,
             ConvierteModo2(file_stat.st_mode), (long long) file_stat.st_size); */
            
            if (S_ISLNK(file_stat.st_mode) && link) {
                char pathLink[4096];
                ssize_t len = readlink(input_trozos[i], pathLink, sizeof(pathLink));
                if (len != -1) {
                    pathLink[len] = '\0';
                    printf("%s -> %s\n", pathLink, input_trozos[i]);
                }
            } else printf("%s\n", input_trozos[i]);
        }
    } else{
        if (acc) {
            printf("%s%-8lld%-4s\n", ctime(&file_stat.st_atime), (long long) file_stat.st_size, input_trozos[i]);
        }
        if (link) {
            if (S_ISLNK(file_stat.st_mode)) {
                char pathLink[4096];
                ssize_t len = readlink(input_trozos[i], pathLink, sizeof(pathLink));
                if (len != -1) {
                    pathLink[len] = '\0';
                    printf("%-8lld %s -> %s\n", (long long) file_stat.st_size, pathLink, input_trozos[i]);
                }
            } else printf("%-8lld %-4s\n", (long long) file_stat.st_size, input_trozos[i]);
        }
    }
}

void Stat(char *input, char *input_trozos[], int n, bool *terminado) {
    int i, j;
    struct stat file_stat;
    bool comands = false, Long = false, link = false, acc = false;

    for (j = 1; input_trozos[j] != NULL; j++) {
        if (strcmp(input_trozos[j], "-long") == 0) {
            Long = true;
            comands = true;
        } else if (strcmp(input_trozos[j], "-link") == 0) {//CAMBIAR LINK
            link = true;
            comands = true;
        } else if (strcmp(input_trozos[j], "-acc") == 0) {
            acc = true;
            comands = true;
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
        if (stat(input_trozos[i], &file_stat) == -1) {
            printf(" ****error al acceder a %s :No such file or directory\n", input_trozos[i]);
            continue;
        }
        Stat_aux(file_stat, acc, link, Long, comands, input_trozos, i);
    }
}

void listDirectory(const char *dirPath, char type){
    DIR *dir = opendir(dirPath);
    if (!dir) {
        perror("Error al abrir el directorio\n");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;

    if(type == 'a'){
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
        printf("************%s\n", dirPath);
        rewinddir(dir);
    }
    while ((entry = readdir(dir)) != NULL) {
        char path[4096]; 
        snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

        if (stat(path, &file_stat) == -1) {
            perror("Error al obtener información del archivo/directorio");
            continue;
        }
        if (entry->d_name[0] == '.')
            continue;

        if (S_ISDIR(file_stat.st_mode)) {
            listDirectory(path, type);
        }
    }
    if(type == 'b'){
        printf("************%s\n", dirPath);
        rewinddir(dir);
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
    }
    closedir(dir);
}

void List(char *input, char *input_trozos[], int n, bool *terminado) {
    struct stat file_stat, file_stat_aux;
    DIR *dir;
    int i, j;
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
        if (stat(input_trozos[i], &file_stat_aux) == -1) {
            printf(" ****error al acceder a %s :No such file or directory\n", input_trozos[i]);
            continue;
        }
        if (S_ISDIR(file_stat_aux.st_mode)) {
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
            }
            if (Long) {
                struct passwd *pwd;
                printf("***********%s\n", input_trozos[i]);
                while ((content = readdir(dir)) != NULL) {
                    snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                    if (stat(rutaAbsoluta, &file_stat) == -1) {
                        perror("Error al obtener información del archivo/directorio"); //PONER COMO EN EL SHELL DE REFERENCIA
                        return;
                    }
                    if (content->d_name[0] != '.') {
                        pwd = getpwuid(file_stat.st_uid);
                        if (pwd) {
                            if (acc) {
                                printf("%s", ctime(&file_stat.st_atime));
                            } else printf("%s", ctime(&file_stat.st_mtime));

                            /* printf("\t%ld\t(%ld)\t%s\t%s\t%lld\t",
                             file_stat.st_nlink, file_stat.st_ino, pwd->pw_name,
                             ConvierteModo2(file_stat.st_mode), (long long) file_stat.st_size); */

                            if (S_ISLNK(file_stat.st_mode) && link) {
                                char pathLink[4096];
                                ssize_t len = readlink(rutaAbsoluta, pathLink, sizeof(pathLink));
                                if (len != -1) {
                                    pathLink[len] = '\0';
                                    printf("%s -> %s\n", pathLink, content->d_name);
                                }
                            } else printf("%s\n", content->d_name);
                        } else {
                            perror("Error al obtener información del archivo/directorio");
                            return;
                        }
                    }
                }
                rewinddir(dir);
            } else{
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
                if (link) {
                    printf("***********%s\n", input_trozos[i]);
                    while ((content = readdir(dir)) != NULL) {
                        snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                        if (stat(rutaAbsoluta, &file_stat) == -1) {
                            perror("Error al obtener información del archivo/directorio\n"); //PONER COMO EN EL SHELL DE REFERENCIA
                            return;
                        }
                        if (content->d_name[0] != '.'){
                            if (S_ISLNK(file_stat.st_mode)) {
                                char pathLink[4096];
                                ssize_t len = readlink(rutaAbsoluta, pathLink, sizeof(pathLink));
                                if (len != -1) {
                                    pathLink[len] = '\0';
                                    printf("%-8lld %s -> %s\n", (long long) file_stat.st_size, pathLink, content->d_name);
                                }
                            } else printf("%-8lld %-4s\n", (long long) file_stat.st_size, content->d_name);
                        }
                    }
                    rewinddir(dir);
                }
            }

            if (recb) {
                listDirectory(input_trozos[i], 'b');
                rewinddir(dir);
            }
            if (reca) {
                listDirectory(input_trozos[i], 'a');
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
            closedir(dir);
        } else Stat_aux(file_stat_aux, acc, link, Long, commands, input_trozos, i);
    }
}

int directorioVacio(const char *ruta) {
    DIR *dir;
    struct dirent *entrada;

    dir = opendir(ruta);

    if (dir == NULL) {
        return -1;
    }

    while ((entrada = readdir(dir)) != NULL) {
        if (strcmp(entrada->d_name, ".") != 0 && strcmp(entrada->d_name, "..") != 0) {
            closedir(dir);
            return 0;       
        }
    }

    closedir(dir);

    return 1;
}

void aux_delete(char *name) {
    struct stat info;

    if (stat(name, &info) != 0) {
        printf("Imposible borrar %s: %s\n", name, strerror(errno));
        return;
    }

    if (S_ISDIR(info.st_mode)) {
        if (!directorioVacio(name)) {
            printf("Imposible borrar %s: Directory not empty\n", name);
            return;
        }
    }
    if (remove(name) != 0)
        printf("Imposible borrar %s: %s\n", name, strerror(errno));
}
void delete(char *input, char *input_trozos[], int n, bool *terminado){
    if(n==1){
        char cd[512];
        if (getcwd(cd, sizeof(cd)) != NULL)
            printf("%s\n", cd);
        else
            perror("No se ha podido obtener el directorio actual de trabajo");
    }
    else{
        for(int i = 1; i<n; i++)
            aux_delete(input_trozos[i]);
    }
}

void aux_deltree(char *name){
    struct stat info;

    if (stat(name, &info) != 0) {
        printf("Imposible borrar %s: %s\n", name, strerror(errno));
        return;
    }

    if (S_ISDIR(info.st_mode)) {
        if (directorioVacio(name)) {
            if (remove(name) != 0) 
                printf("Imposible borrar %s: %s\n", name, strerror(errno));
        }
        else{
            DIR *dir = opendir(name);
            struct dirent *entry;

            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    char path[MAXNAME];
                    snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
                    aux_deltree(path);
                }
            }
            closedir(dir);

            if (rmdir(name) != 0) {
                printf("Imposible borrar %s: %s\n", name, strerror(errno));
            }
        }
    }
    else{
        if (remove(name) != 0)
        printf("Imposible borrar %s: %s\n", name, strerror(errno));
    }    
}

void deltree(char *input, char *input_trozos[], int n, bool *terminado){
    if(n==1){
        char cd[512];
        if (getcwd(cd, sizeof(cd)) != NULL)
            printf("%s\n", cd);
        else
            perror("No se ha podido obtener el directorio actual de trabajo");
    }
    else{
        for(int i = 1; i<n; i++)
            aux_deltree(input_trozos[i]);
            
    }
}
