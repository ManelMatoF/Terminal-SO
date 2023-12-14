#define _GNU_SOURCE

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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <sys/wait.h>
#include <sys/resource.h>



#include "comandos.h"

tList H, C, F, M, P;
int num_back_process = 0;

int entero_global_i = 45;
float flotante_global_i = 3.12;
char caracter_global_i = 'M';

int entero_global_sin;
float flotante_global_sin;
char caracter_global_sin;

extern char **environ;
char **valorGuardado;


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

void execute_external_command(char *input_trozos[], int n);

char **obtenerValorGuardado() {
    return valorGuardado;
}

void guardarValor(char *valor[]) {
    valorGuardado = valor;
}


void aux_procesarEntrada(char *input, bool *terminado, char **arg3){
    guardarValor(arg3);
    char copy_input[MAXNAME];
    char **input_trozos = (char **)malloc(MAX_TROZOS * sizeof(char*));
    strcpy(copy_input, input);
    int input_num = TrocearCadena(copy_input, input_trozos);
    if(input_num > 0){
        tPosL cmd = encontrarComando(input_trozos);
        if(cmd == NULL){
            execute_external_command(input_trozos, input_num);
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
            aux_procesarEntrada(input, terminado, valorGuardado);
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
            if(p!=NULL){
                c = (Comands*)(getItem(p));  
            } 
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

char LetraTF (mode_t m){
    
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
}

void Stat_aux(struct stat file_stat, bool acc, bool link, bool Long, bool comands, char *input_trozos[], int i) {
    char buffer_time[20];
    struct tm *tm_date;

    if(!comands) {
        printf("  %lld\t%s\n", (long long) file_stat.st_size, input_trozos[i]);
    }
    if (Long) {
        struct passwd *pwd = getpwuid(file_stat.st_uid);
        if (pwd) {
            if (acc)
                tm_date = localtime(&file_stat.st_atime);
            else 
                tm_date = localtime(&file_stat.st_mtime);

            strftime(buffer_time, sizeof(buffer_time), "%Y/%m/%d-%H:%M", tm_date);
            
            printf("%s\t%ld\t(%ld)\t%s\t%s\t%lld\t", buffer_time,
                    file_stat.st_nlink, file_stat.st_ino, pwd->pw_name, ConvierteModo2(file_stat.st_mode), (long long) file_stat.st_size);
            
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
            tm_date = localtime(&file_stat.st_atime);
            strftime(buffer_time, sizeof(buffer_time), "%Y/%m/%d-%H:%M", tm_date);
            printf("%s\t%-8lld%-4s\n", buffer_time, (long long) file_stat.st_size, input_trozos[i]);
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
        } else if (strcmp(input_trozos[j], "-link") == 0) {
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
            printf("****error al acceder a %s:%s\n", input_trozos[i], strerror(errno));
            continue;
        }
        Stat_aux(file_stat, acc, link, Long, comands, input_trozos, i);
    }
}

void acc_funct(char *rutaAbsoluta, bool link, struct dirent *content, struct stat file_stat){
    char buffer_time[20];
    struct tm *tm_date;

    tm_date = localtime(&file_stat.st_atime);
    strftime(buffer_time, sizeof(buffer_time), "%Y/%m/%d-%H:%M", tm_date);
    printf("%s\t%lld\t", buffer_time, (long long) file_stat.st_size);
    if (S_ISLNK(file_stat.st_mode) && link) {
            char pathLink[4096];
            ssize_t len = readlink(rutaAbsoluta, pathLink, sizeof(pathLink));
            if (len != -1) {
                pathLink[len] = '\0';
                printf("%s -> %s\n", pathLink, content->d_name);
            }
        } else printf("%s\n", content->d_name);
}



void long_funct(char *rutaAbsoluta, bool acc, bool link, struct dirent *content, struct stat file_stat){
    char buffer_time[20];
    struct tm *tm_date;
    struct passwd *pwd;

    pwd = getpwuid(file_stat.st_uid);
    if (pwd) {
        if (acc)
            tm_date = localtime(&file_stat.st_atime);
        else 
            tm_date = localtime(&file_stat.st_mtime);

        strftime(buffer_time, sizeof(buffer_time), "%Y/%m/%d-%H:%M", tm_date);
        printf("%s\t%ld\t(%ld)\t%s\t%s\t%lld\t", buffer_time, file_stat.st_nlink, file_stat.st_ino, pwd->pw_name,
        ConvierteModo2(file_stat.st_mode), (long long) file_stat.st_size);

        if (S_ISLNK(file_stat.st_mode) && link) {
            char pathLink[4096];
            ssize_t len = readlink(rutaAbsoluta, pathLink, sizeof(pathLink));
            if (len != -1) {
                pathLink[len] = '\0';
                printf("%s -> %s\n", pathLink, content->d_name);
            }
        } else printf("%s\n", content->d_name);
    } else
        perror("Error al obtener información del archivo/directorio");

}

void link_funct(char *rutaAbsoluta, struct dirent *content, struct stat file_stat){
    if (S_ISLNK(file_stat.st_mode)) {
        char pathLink[4096];
        ssize_t len = readlink(rutaAbsoluta, pathLink, sizeof(pathLink));
        if (len != -1) {
            pathLink[len] = '\0';
            printf("%-8lld %s -> %s\n", (long long) file_stat.st_size, pathLink, content->d_name);
        }
    } else 
        printf("%-8lld %-4s\n", (long long) file_stat.st_size, content->d_name);
}

void hid_funct(char *rutaAbsoluta, bool acc, bool Long, bool link, struct dirent *content, struct stat file_stat){
    if(Long)
        long_funct(rutaAbsoluta, acc, link, content, file_stat);
    else if(acc)
        acc_funct(rutaAbsoluta, link, content, file_stat);
    else if(link)
        link_funct(rutaAbsoluta, content, file_stat);
    else
        printf("\t%lld\t%s\n", (long long) file_stat.st_size, content->d_name);
}


void listDirectory(const char *dirPath, char type, bool hid, bool Long, bool acc, bool link){
    DIR *dir = opendir(dirPath);
    if (!dir) {
        perror("Error al abrir el directorio\n");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;

    if(type == 'a'){
        printf("************%s\n", dirPath);
        while ((entry = readdir(dir)) != NULL) {
            
            char path[4096];
            snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

            if (stat(path, &file_stat) == -1) {
                perror("Error al obtener información del archivo/directorio");
                continue;
            }
            if (entry->d_name[0] != '.' && !hid){
                if(Long)
                    long_funct(path, acc, link, entry, file_stat);      
                else if(acc)
                    acc_funct(path, link, entry, file_stat);
                else if(link)
                    link_funct(path, entry, file_stat);
                else
                    printf("\t%ld\t%s\n", file_stat.st_size, entry->d_name);
            }
            if(hid)
                    hid_funct(path, acc, Long, link, entry, file_stat);    
            
        }
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
            listDirectory(path, type, hid, Long, acc, link);
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
            if (entry->d_name[0] != '.' && !hid){
                if(Long)
                    long_funct(path, acc, link, entry, file_stat);      
                else if(acc)
                    acc_funct(path, link, entry, file_stat);
                else if(link)
                    link_funct(path, entry, file_stat);
                else
                    printf("\t%ld\t%s\n", file_stat.st_size, entry->d_name);
            }
            if(hid)
                    hid_funct(path, acc, Long, link, entry, file_stat);    
        }
    }
    closedir(dir);
}

void list(char *input, char *input_trozos[], int n, bool *terminado) {
    struct stat file_stat;
    DIR *dir;
    int i, j;
    struct dirent *content;
    char rutaAbsoluta[4096];
    bool commands = false, Long = false, link = false, acc = false, recb = false, reca = false, hid = false;

    for (j = 1; input_trozos[j] != NULL; j++) {
        if (strcmp(input_trozos[j], "-long") == 0) {
            Long = true;
            commands = true;
        } else if (strcmp(input_trozos[j], "-link") == 0) {
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
        if (stat(input_trozos[i], &file_stat) == -1) {
            printf("****error al acceder a %s:%s\n", input_trozos[i], strerror(errno));
            continue;
        }
        if (S_ISDIR(file_stat.st_mode)) {
            dir = opendir(input_trozos[i]);
            if (!dir) {
                printf("****error al acceder a %s:%s\n", input_trozos[i], strerror(errno));
                continue;
            }

            if (!commands) {
                printf("************%s\n", input_trozos[i]);
                while ((content = readdir(dir)) != NULL) {
                    snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                    if (stat(rutaAbsoluta, &file_stat) == -1) {
                        perror("Error al obtener información del archivo/directorio"); 
                        return;
                    }
                    if (content->d_name[0] != '.') {
                        printf("  %lld\t%s\n", (long long) file_stat.st_size, content->d_name);
                    }
                }
                continue;
            }
            if(!recb && !reca && !hid){
                if (Long) {
                    printf("************%s\n", input_trozos[i]);
                    while ((content = readdir(dir)) != NULL) {
                        snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                        if (stat(rutaAbsoluta, &file_stat) == -1) {
                            perror("Error al obtener información del archivo/directorio"); 
                            continue;;
                        }
                        if (content->d_name[0] != '.') 
                                long_funct(input_trozos[i], acc, link, content, file_stat);
                            
                    }
                    rewinddir(dir);
                }
                else{
                    if (acc) {
                        printf("************%s\n", input_trozos[i]);
                        while ((content = readdir(dir)) != NULL) {
                            snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                            if (stat(rutaAbsoluta, &file_stat) == -1) {
                                perror("Error al obtener información del archivo/directorio\n");
                                continue;;
                            }
                            if (content->d_name[0] != '.') 
                                acc_funct(input_trozos[i], link, content, file_stat);
                        }
                        rewinddir(dir);
                    }
                    if (link && !acc) {
                        printf("************%s\n", input_trozos[i]);
                        while ((content = readdir(dir)) != NULL) {
                            snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                            if (stat(rutaAbsoluta, &file_stat) == -1) {
                                perror("Error al obtener información del archivo/directorio\n");
                                continue;
                            }
                            if (content->d_name[0] != '.'){
                                link_funct(input_trozos[i], content, file_stat);
                            }
                        }
                        rewinddir(dir);
                    }
                }
            }
            if (hid && !reca && !recb) {
                printf("************%s\n", input_trozos[i]);
                while ((content = readdir(dir)) != NULL) {
                    snprintf(rutaAbsoluta, sizeof(rutaAbsoluta), "%s/%s", input_trozos[i], content->d_name);
                    if (stat(rutaAbsoluta, &file_stat) == -1) {
                        perror("Error al obtener información del archivo/directorio");
                        return;
                    }
                    hid_funct(input_trozos[i], acc, Long, link, content, file_stat);
                }
                rewinddir(dir);
            }
            if (recb) {
                listDirectory(input_trozos[i], 'b', hid, Long, acc, link);
                rewinddir(dir);
            }
            if (reca) {
                listDirectory(input_trozos[i], 'a', hid, Long, acc, link);
                rewinddir(dir);
            }
            closedir(dir);
          
        } else 
            Stat_aux(file_stat, acc, link, Long, commands, input_trozos, i);
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

tPosL findMemTam(size_t size, AllocationType type){
    if(M != NULL){
        tPosL p = first(M);
        MemInfo *m;
        m = (MemInfo*)(getItem(p));
        while(p!=NULL && (m->size != size || m->allocationType != type)){
            p = next(p);
            if(p!=NULL)
                m = (MemInfo*)(getItem(p));
        }
        return p;
    }
    return NULL;
}

void printMemList(AllocationType type, bool mem){
    pid_t pid_aux = getpid();
    if(!mem)
        printf("******Lista de bloques asignados%spara el proceso %d\n", type==MALLOC_MEMORY ? " malloc " : type==SHARED_MEMORY ? " shared " : type == MAPPED_FILE ? " mmap " : " ", pid_aux);
    if(M != NULL){
        tPosL p;
        MemInfo *m = malloc(sizeof(MemInfo));
        for(p = first(M); p != NULL; p=next(p)){
            m = (MemInfo*)getItem(p);
            if(m->allocationType == type){  
                char typename[10];
                char buffer_time[20];
                struct tm *tm_date;

                tm_date = localtime(&m->t);
                strftime(buffer_time, sizeof(buffer_time), "%b %d %H:%M", tm_date);

                if(m->allocationType==MALLOC_MEMORY)
                    strcpy(typename, "malloc ");
                else if(m->allocationType==SHARED_MEMORY)
                    strcpy(typename, "shared ");
                else
                    strcpy(typename, " ");

                printf("\t%p\t\t%lu %s %s", m->address, (unsigned long) m->size, buffer_time, typename);
                if(m->allocationType==SHARED_MEMORY)
                    printf("(key %lu)", (unsigned long) m->OtherInfo.key);
                if(m->allocationType==MAPPED_FILE)
                    printf("%s  (descriptor %d)", m->OtherInfo.mapped.filename, m->OtherInfo.mapped.df);
                printf("\n");
            }
        }
    }
}

void malloc_funct(char *input, char *input_trozos[], int n, bool *terminado){
    if(n == 1){
        printMemList(MALLOC_MEMORY, false);
    }
    else {
        size_t size = atoi(input_trozos[1]);
        if(size > 0){
            MemInfo *block = malloc(sizeof(MemInfo));
            size_t MemInfosize = sizeof(MemInfo);
            void *address = malloc(size);

            block->address = address;
            block->size = size; 
            block->t = time(NULL);
            block->allocationType = MALLOC_MEMORY;
            
            insertItem(block, MemInfosize, &M);
            printf("Asignados %lu bytes en %p\n", (unsigned long) size, block->address);

            free(block);
        }
        else if(strcmp("-free", input_trozos[1])==0){
            if(n == 2)
                printMemList(MALLOC_MEMORY, false);
            else {
                size_t size = atoi(input_trozos[2]);
                if(size > 0){
                    tPosL m = findMemTam(size, MALLOC_MEMORY);
                    if(m != NULL){
                        MemInfo *block;
                        block = (MemInfo*)getItem(m);
                        free(block->address);
                        deleteItem(m, &M);
                    }
                    else
                        printf("No hay bloque de ese tamano asignado con malloc\n");
                }
                else
                    printf("No se asignan bloques de 0 bytes\n");
            }
        }
        else
            printf("No se asignan bloques de 0 bytes\n");
    }
}

void * ObtenerMemoriaShmget (key_t clave, size_t tam){
    void *p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    MemInfo *block = malloc(sizeof(MemInfo));
    size_t MemInfosize = sizeof(MemInfo);

    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
             shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    block->address=p;
    block->allocationType=SHARED_MEMORY;
    block->size=s.shm_segsz;
    block->t=time(NULL);
    block->OtherInfo.key=clave;

    insertItem(block, MemInfosize, &M);
    free(block);

    return (p);

}
void do_AllocateCreateshared (char *tr[]){
   key_t cl;
   size_t tam;
   void *p;

   if (tr[0]==NULL || tr[1]==NULL) {
		printMemList(SHARED_MEMORY, false);
		return;
   }
  
   cl=(key_t)  strtoul(tr[0],NULL,10);
   tam=(size_t) strtoul(tr[1],NULL,10);
   if (tam==0) {
	printf ("No se asignan bloques de 0 bytes\n");
	return;
   }
   if ((p=ObtenerMemoriaShmget(cl,tam))!=NULL)
		printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
   else
		printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void do_DeallocateDelkey (char *args[]){//donde se buscan las keys? (lista o general)
   key_t clave;
   int id;
   char *key=args[0];

   if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey necesita clave_valida\n");
        return;
   }                
   if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
   }
   if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
}

void * findMemKey(key_t cl, AllocationType type){
    if(M != NULL){
        tPosL p = first(M);
        MemInfo *m;
        m = (MemInfo*)(getItem(p));
        while(p!=NULL && (m->allocationType != type || m->OtherInfo.key!= cl)){
            p = next(p);
            if(p!=NULL)
                m = (MemInfo*)(getItem(p));
        }
        if(p!=NULL)
            return m->address;
    }
    return NULL;
}

void do_FreeDelKey(char *tr[]) {
    key_t cl;
    void *p;
    char *key = tr[0];

    if (tr[0] == NULL) {
        printMemList(SHARED_MEMORY, false);
        return;
    }
    
    cl = (key_t)strtoul(key, NULL, 10);

    
    if (cl == IPC_PRIVATE) {
        printf(" free necesita clave valida\n");
        return;
    }
    p = findMemKey(cl, SHARED_MEMORY);

    if (p == NULL) {
        printf("No hay bloque de esa clave mapeado en el proceso\n");
        return;
    }

    // Desmapear la memoria compartida
    if (shmdt(p) == -1) {
        perror("No se pudo desmapear la memoria compartida");
        return;
    }
    else
        deleteItem(first(M), &M);
}


void shared_funct(char *input, char *input_trozos[], int n, bool *terminado){
    if(n == 1){
        printMemList(SHARED_MEMORY, false);
    }
    else{
        if(strcmp(input_trozos[1], "-create") == 0){
            do_AllocateCreateshared(input_trozos + 2);
        }
        else if(strcmp(input_trozos[1], "-free")== 0){
            do_FreeDelKey(input_trozos + 2);
        }
        else if(strcmp(input_trozos[1], "-delkey") == 0){
            do_DeallocateDelkey(input_trozos + 2);
        }
        else{
            key_t cl = (key_t)  strtoul(input_trozos[1],NULL,10);
            void *p;
            if ((p=ObtenerMemoriaShmget(cl, 0))!=NULL)
                printf ("Memoria compartida de clave %lu en %p\n",(unsigned long) cl, p);
            else
                printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
                    
        }       
    }
}

void * MapearFichero (char * fichero, int protection)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    MemInfo *block = malloc(sizeof(MemInfo));
    size_t MemInfosize = sizeof(MemInfo);

    if (protection&PROT_WRITE)
          modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
          return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
           return NULL;

    block->address = p;
    block->allocationType = MAPPED_FILE;
    block->size = s.st_size;
    block->t = time(NULL);
    block->OtherInfo.mapped.df = df;
    strcpy(block->OtherInfo.mapped.filename, fichero);

    insertItem(block, MemInfosize, &M);
    free(block);
    
    return p;
}

void do_AllocateMmap(char *arg[])
{ 
    char *perm;
    void *p;
    int protection=0;
     
    if ((perm=arg[1])!=NULL && strlen(perm)<4) {
            if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
            if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
            if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MapearFichero(arg[0],protection))==NULL)
             perror ("Imposible mapear fichero");
    else
             printf ("fichero %s mapeado en %p\n", arg[0], p);
}

void * findMemName(char *fichero, AllocationType type){
    if(M != NULL){
        tPosL p = first(M);
        MemInfo *m;
        m = (MemInfo*)(getItem(p));
        while(p!=NULL && (m->allocationType != type || strcmp(m->OtherInfo.mapped.filename, fichero) != 0)){
            p = next(p);
            if(p!=NULL)
                m = (MemInfo*)(getItem(p));
        }
    return p;
    }
    return NULL;
}

void do_freeMmap(char * fichero) {

    MemInfo *block;
    tPosL p;

    p=findMemName(fichero, MAPPED_FILE);

    if (p == NULL) {
        perror("Imposible mapear fichero:");
        return;
    }

    block = (MemInfo*)(getItem(p));

    if (munmap(block->address, block->size) == -1) {
        perror("Error unmapping memory");
        return;
    }
    close(block->OtherInfo.mapped.df);
    deleteItem(p, &M);
}


void mmap_funct(char *input, char *input_trozos[], int n, bool *terminado){
    if(n == 1)
        printMemList(MAPPED_FILE, false);
    else{
        if(strcmp(input_trozos[1], "-free")==0)
            do_freeMmap(input_trozos[2]);
        else
            do_AllocateMmap(input_trozos + 1);
    }
        
}

ssize_t LeerFichero(char *f, void *p, size_t cont) {
    struct stat s;
    ssize_t n;
    int df, aux;

    if (stat(f, &s) == -1 || (df = open(f, O_RDONLY)) == -1)
        return -1;
    if (cont == -1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont = s.st_size;
    if ((n = read(df, p, cont)) == -1) {
        aux = errno;
        close(df);
        errno = aux;
        return -1;
    }
    close(df);
    return n;
}

tPosL findMemAddr(unsigned long long addr) {
    if (M != NULL) {
        tPosL p = first(M);
        MemInfo *m;
        m = (MemInfo *) (getItem(p));
        while (p != NULL && (m->address != (void *) addr)) {
            p = next(p);
            if (p != NULL)
                m = (MemInfo *) (getItem(p));
        }
        return p;
    }
    return NULL;
}

void *cadtop(char *cadena) {
    unsigned long long addr = strtoull(cadena, NULL, 16);
    tPosL p = findMemAddr(addr);
    if (p != NULL) {
        MemInfo *m = (MemInfo *) getItem(p);
        return m->address;
    }
    return NULL;
}

void CmdRead(char *input, char *input_trozos[], int n, bool *terminado) {
    void *p;
    size_t cont = -1;  /* -1 indica leer todo el fichero*/
    ssize_t size;
    if (input_trozos[1] == NULL || input_trozos[2] == NULL) {
        printf("faltan parametros\n");
        return;
    }
    p = cadtop(input_trozos[2]);  /*convertimos de cadena a puntero*/
    if (input_trozos[3] != NULL)
        cont = (size_t) atoll(input_trozos[3]);

    if ((size = LeerFichero(input_trozos[1], p, cont)) == -1)
        perror("Imposible leer fichero");
    else
        printf("leidos %lld bytes de %s en %p\n", (long long) size, input_trozos[1], p);
}

ssize_t EscribirFichero(char *f, void *p, size_t cont, int overwrite) {
    ssize_t n;
    int df, aux, flags = O_CREAT | O_EXCL | O_WRONLY;

    if (overwrite)
        flags = O_CREAT | O_WRONLY | O_TRUNC;

    if ((df = open(f, flags, 0777)) == -1)
        return -1;

    if ((n = write(df, p, cont)) == -1) {
        aux = errno;
        close(df);
        errno = aux;
        return -1;
    }
    close(df);
    return n;
}

void Write(char *input, char *input_trozos[], int n, bool *terminado) {
    void *p;
    size_t cont;
    ssize_t size;
    int overwrite = 0;

    if (input_trozos[1] != NULL && strcmp(input_trozos[1], "-o") == 0)
        overwrite = 1;

    if (input_trozos[overwrite + 1] == NULL || input_trozos[overwrite + 2] == NULL ||
        input_trozos[overwrite + 3] == NULL) {
        printf("faltan parametros\n");
        return;
    }

    p = cadtop(input_trozos[overwrite + 2]);  /*convertimos de cadena a puntero*/
    cont = (size_t) atoll(input_trozos[overwrite + 3]);

    if ((size = EscribirFichero(input_trozos[overwrite + 1], p, cont, overwrite) == -1))
        perror("Imposible escribir fichero");
    else
        printf("Escritos %lld bytes en %s desde %p\n", (long long) cont, input_trozos[overwrite + 1], p);
}

void Memdump(char *input, char *input_trozos[], int n, bool *terminado) {
    size_t cont, cont_aux;
    tPosL p;
    MemInfo *m;

    if (input_trozos[1] == NULL) {
        printf("faltan parametros\n");
        return;
    } else {
        p = findMemAddr(strtoull(input_trozos[1], NULL, 16));
        m = (MemInfo *) getItem(p);
    }


    if (input_trozos[2] != NULL) {
        cont = (size_t) atoll(input_trozos[2]);
        if (cont <= m->size)
            cont_aux = cont;
        else {
            cont = m->size;
            cont_aux = cont;
        }
    } else {
        if(m->size <= 25)
            cont = m->size;
        else
            cont = 25;
        cont_aux = cont;
    }

    printf("Volcando %zu bytes desde la direccion %s\n", cont_aux, input_trozos[1]);
    int j, k;
    j = 0;
    while(cont > 0){
        int charsToPrint = (cont > 25) ? 25 : cont;
        k = j;
        for (int i = 0; i < charsToPrint; i++, j++) {
            if (j >= cont_aux) {
                // Verifica si has alcanzado el final de la memoria asignada
                break;
            }

            if (*(unsigned char *)(m->address + j) == 0x0a) {
                printf("\\n  ");
            } else {
                printf("%c  ", *((unsigned char *)(m->address + j)));
            }
        }
        printf("\n");

        
        for (int i = 0; i < charsToPrint; i++, k++) {
            if (k >= cont_aux) {
                // Verifica si has alcanzado el final de la memoria asignada
                break;
            }
            printf("%02x ", *((unsigned char *)(m->address + k)));
        }
        printf("\n");

        cont -= charsToPrint;
    }
    
}


void LlenarMemoria(void *p, size_t cont, unsigned char byte) {
    unsigned char *arr = (unsigned char *) p;
    size_t i;

    for (i = 0; i < cont; i++)
        arr[i] = byte;
}


void Memfill(char *input, char *input_trozos[], int n, bool *terminado) {
    size_t cont;
    MemInfo *m;
    int byte = 65;
    tPosL p;

    if (input_trozos[1] == NULL) {
        return;
    } else {
        p = findMemAddr(strtoull(input_trozos[1], NULL, 16));
        m = (MemInfo *) getItem(p);
    }
    if (input_trozos[2] == NULL) {
        cont = m->size;
    } else {
        cont = strtoul(input_trozos[2], NULL, 10);
        if (cont > m->size) {
            printf("No es posible asignar tantos bytes\n");
            return;
        }

    }
    if (input_trozos[3] != NULL) {
        byte = (int) strtoul(input_trozos[3], NULL, 10);
    }
    printf("LLenando %zu bytes de memoria con el byte %c(%02x) a partir de la direccion %s\n", cont, (char) byte, byte,
           input_trozos[1]);
    LlenarMemoria(m->address, cont, (char) byte);
}


void Do_MemPmap(void) /*sin argumentos*/
{
    pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4] = {"pmap", elpid, NULL};

    sprintf(elpid, "%d", (int) getpid());
    if ((pid = fork()) == -1) {
        perror("Imposible crear proceso");
        return;
    }
    if (pid == 0) { /*proceso hijo*/
        if (execvp(argv[0], argv) == -1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0] = "vmmap";
        argv[1] = "-interleave";
        argv[2] = elpid;
        argv[3] = NULL;
        if (execvp(argv[0], argv) == -1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");

        argv[0] = "procstat";
        argv[1] = "vm";
        argv[2] = elpid;
        argv[3] = NULL;
        if (execvp(argv[0], argv) == -1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0] = "procmap", argv[1] = elpid;
        argv[2] = NULL;
        if (execvp(argv[0], argv) == -1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        exit(1);
    }
    waitpid(pid, NULL, 0);
}

void funcsPro() {
    void *array[5];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 5);
    strings = backtrace_symbols(array, size);

    if (strings == NULL) {
        perror("backtrace_symbols");
        return;
    }

    printf("Funciones programa\t");

    for (i = 0; i < 3; i++) {
        void *addr = array[i];
        Dl_info info;
        dladdr(addr, &info);
        if (i+1 != 3)
            printf("%p, \t", addr);
        else
            printf("%p\n", addr);
    }
    free(strings);
}

void funcsLib(){
       void *printfAddress[5];

        printfAddress[0] = dlsym(RTLD_DEFAULT, "printf");
        printfAddress[1] = dlsym(RTLD_DEFAULT, "open");
        printfAddress[2] = dlsym(RTLD_DEFAULT, "malloc");
        
        printf("Funciones libreria\t");
        for (int i = 0; i < 3; i++){
            if(i+1 != 3)
                printf("%p, \t", printfAddress[i]);
            else
                printf("%p\n", printfAddress[i]);
        }
            

}

void funcsVar(){
  int entero_local = 10;
        float flotante_local = 2.718;
        char caracter_local = 'B';
        printf("Variables locales\t%p, \t%p, \t%p\n", (void*)&entero_local, (void*)&flotante_local, (void*)&caracter_local);

        printf("Variables globales\t%p, \t%p, \t%p\n", (void*)&entero_global_i, (void*)&flotante_global_i, (void*)&caracter_global_i);

        printf("Var (N.I.)globales\t%p, \t%p, \t%p\n", (void*)&entero_global_sin, (void*)&flotante_global_sin, (void*)&caracter_global_sin);

        
        static int entero_estatico_i = 33;
        static float flotante_estatico_i = 1.24;
        static char caracter_estatico_i = 'N';

        // Direcciones de variables estáticas inicializadas
        printf("Variables estáticas\t%p, \t%p, \t%p\n", (void*)&entero_estatico_i, (void*)&flotante_estatico_i, (void*)&caracter_estatico_i);

        // Variables estáticas sin inicializar
        static int entero_estatico_sin;
        static float flotante_estatico_sin;
        static char caracter_estatico_sin;

        // Direcciones de variables estáticas sin inicializar
        printf("Var (N.I.)staticas\t%p, \t%p, \t%p\n", (void*)&entero_estatico_sin, (void*)&flotante_estatico_sin, (void*)&caracter_estatico_sin);
}


void Mem(char *input, char *input_trozos[], int n, bool *terminado) {

    if (input_trozos[1] == NULL || strcmp(input_trozos[1], "-all") == 0) {
        char *vars_trozos[] = {input_trozos[0], "-vars", NULL};
        Mem(input, vars_trozos, n, terminado);

        char *funcs_trozos[] = {input_trozos[0], "-funcs", NULL};
        Mem(input, funcs_trozos, n, terminado);

        char *blocks_trozos[] = {input_trozos[0], "-blocks", NULL};
        Mem(input, blocks_trozos, n, terminado);
    } else if (strcmp(input_trozos[1], "-blocks") == 0) {
        pid_t pid_aux = getpid();
        printf("******Lista de bloques asignados para el proceso %d\n", pid_aux);
        printMemList(MALLOC_MEMORY, true);
        printMemList(SHARED_MEMORY, true);
        printMemList(MAPPED_FILE, true);
    } else if (strcmp(input_trozos[1], "-funcs") == 0) {
        funcsPro(); 
        funcsLib();
    } else if (strcmp(input_trozos[1], "-vars") == 0) {
        funcsVar();
    } else if (strcmp(input_trozos[1], "-pmap") == 0) {
        Do_MemPmap();
    } else printf("Opcion %s no contenplada", input_trozos[1]);
}

void Recursiva (int n)
{
  char automatico[TAMANO];
  static char estatico[TAMANO];

  printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

  if (n>0)
    Recursiva(n-1);
}

void Rec(char *input, char *input_trozos[], int n, bool *terminado){
    if(n == 1)
        return;
    else
        Recursiva(atoi(input_trozos[1]));
}


void showCredentials() {
    printf("Credencial real: %d, (%s)\n", getuid(), getlogin());
    printf("Credencial efectiva: %d, (%s)\n", geteuid(), getlogin());
}

void setCredentials(int uid) {
    if (setuid(uid) == -1) {
        perror("Imposible cambiar de credencial");
        return;
    }
}

void setLoginCredentials(const char *username) {
    struct passwd *pw = getpwnam(username);
    if (pw == NULL) {
        perror("Imposible cambiar de credencial");
        return;
    }
    setCredentials(pw->pw_uid);
}

void uid_funct(char *input, char *input_trozos[], int n, bool *terminado){//comprobar que funciona bien
    if(n>1){
        if(strcmp(input_trozos[1], "-get")==0)
            showCredentials();
        else if(strcmp(input_trozos[1], "-set")==0){
            if(n>2){
                if(strcmp(input_trozos[2], "-l")==0){
                    if(n==3){
                        showCredentials();
                        return;
                    }
                    setLoginCredentials(input_trozos[3]);
                }
                    
                else
                    setCredentials(atoi(input_trozos[2]));
            }
            else
                showCredentials();
        }
        else
            showCredentials();
    }
    else
        showCredentials();
}


void showvar_aux(const char *var_name) {
    char **arg3 = obtenerValorGuardado();
    int i;
    if (var_name == NULL) {
        for (i = 0; arg3[i] != NULL; ++i)
            printf("%p->main arg3[%d]=(%p) %s\n", (void*)arg3[i], i, (void*)&arg3[i], arg3[i]);
    }

    else{
        char *value = NULL;
        
        if (getenv(var_name)!=NULL) {
            for (int i = 0; arg3[i] != NULL; ++i) {
                if((value = strstr(arg3[i], var_name)) != NULL){
                    printf("Con arg3 main %s(%p) @%p\n", arg3[i], (void*)value, (void*)&arg3[i]);
                    break;
                }
                
            }
            for (int i = 0; environ[i] != NULL; ++i) {
                if ((value = strstr(environ[i], var_name)) == environ[i]) {
                    printf("  Con environ %s(%p) @%p\n", environ[i], (void*)value, (void*)&environ[i]);
                    break;
                }
            }
            value = getenv(var_name);
            printf("    Con getenv %s(%p)\n", value, (void*)value);
            return;
        }
        return;
    }
}

void showvar(char *input, char *input_trozos[], int n, bool *terminado){
    showvar_aux(input_trozos[1]);
}

void actuValor(char **arg3, char *var1, char *var2, char *valor){
    int i;
    for (i = 0; arg3[i] != NULL && strstr(arg3[i], var1) == NULL; ++i);
    if (arg3[i] != NULL) {
        size_t new_length = strlen(var2) + strlen(valor) + 2;
        char *new_entry = malloc(new_length);
        snprintf(new_entry, new_length, "%s=%s", var2, valor);

        // Liberar la memoria de la variable antigua y asignar la nueva
        free(arg3[i]);
        arg3[i] = new_entry;
    }
}

void changevar(char *input, char *input_trozos[], int n, bool *terminado){
    if(n != 4){
        printf("Uso: changevar [-a|-e|-p] var valor\n");
        return;
    }
    char **arg3 = obtenerValorGuardado();

    if(strcmp("-a", input_trozos[1])==0){
        int i;
        for (i = 0; arg3[i] != NULL && strstr(arg3[i], input_trozos[2]) == NULL; ++i);
        if(arg3[i] != NULL){
            const char *variable = input_trozos[2];
            const char *value = input_trozos[3];
            setenv(variable, value, 1);
            actuValor(arg3, input_trozos[2], input_trozos[2], input_trozos[3]);
        }
        return;
    }
    else if(strcmp("-e", input_trozos[1])==0){
        char **env_var = environ;

        while (*env_var != NULL) {
            if (strncmp(*env_var, input_trozos[2], strlen(input_trozos[2])) == 0) {
                strcpy(*env_var, input_trozos[2]);
                strcat(*env_var, "=");
                strcat(*env_var, input_trozos[3]);
                actuValor(arg3, input_trozos[2], input_trozos[2], input_trozos[3]);
                break;
            }
            env_var++;
        }
        return;
    }
    else if(strcmp("-p", input_trozos[1])==0){
        char *env_entry = malloc(strlen(input_trozos[2]) + strlen(input_trozos[3]) + 2);
        strcpy(env_entry, input_trozos[2]);
        strcat(env_entry, "=");
        strcat(env_entry, input_trozos[3]);
        putenv(env_entry);
        actuValor(arg3, input_trozos[2], input_trozos[2], input_trozos[3]);
        return;
    }
    else
        printf("Uso: changevar [-a|-e|-p] var valor\n");
}

void subsvar(char *input, char *input_trozos[], int n, bool *terminado){
    if (n != 5) {
        printf("Uso: subsvar [-a|-e] var1 var2 valor\n");
        return;
    }

    const char *var1 = input_trozos[2];
    const char *var2 = input_trozos[3];
    const char *value = input_trozos[4];

    if (strcmp(input_trozos[1], "-a") == 0) {
        char **arg3 = obtenerValorGuardado();
        int i, j;
        for (i = 0; arg3[i] != NULL && strstr(arg3[i], input_trozos[2]) == NULL; ++i);
        if(arg3[i] != NULL){
            for(j = 0;  arg3[j] != NULL && strstr(arg3[j], input_trozos[2]) == NULL; ++j);
            if(arg3[j] == NULL){
                if(unsetenv(input_trozos[2]) != 0){
                    fprintf(stderr, "Imposible sustituir variable %s por %s:", input_trozos[2], input_trozos[3]);
                }
                setenv(input_trozos[3], value, 0);
                actuValor(arg3, input_trozos[2], input_trozos[3], input_trozos[4]);
            }
            
        }
        else
            fprintf(stderr, "Imposible sustituir variable %s por %s:", input_trozos[2], input_trozos[3]);
            
    } else if (strcmp(input_trozos[1], "-e") == 0) {
        char **env_var = environ;
        while (*env_var != NULL) {
            if (strncmp(*env_var, var1, strlen(var1)) == 0) {
                char *new_entry = malloc(strlen(var2) + strlen(value) + 2);
                sprintf(new_entry, "%s=%s", var2, value);
                putenv(new_entry);
                break;
            }
            env_var++;
        }

        if (*env_var == NULL) {
            fprintf(stderr, "Variable de entorno no encontrada: %s\n", var1);
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Uso: subsvar [-a|-e] var1 var2 valor\n");
    }
}


void showenv(char *input, char *input_trozos[], int n, bool *terminado){
    if(n == 1)
        showvar_aux(NULL);
    else if(strcmp("-environ", input_trozos[1]) == 0){
        for (int i = 0; environ[i] != NULL; ++i)
        printf("%p->environ[%d]=(%p) %s\n",  (void*)environ[i], i, (void*)&environ[i], environ[i]);
    }
    else if(strcmp("-addr", input_trozos[1]) == 0){
        char **arg3 = obtenerValorGuardado();
        printf("environ:    %p (almacenado en %p)\n", (void*)environ, (void*)&environ);
        printf("main arg3:  %p (almacenado en %p)\n", (void*)arg3, (void*)&arg3);
    }
    else
        printf("Uso: showenv [-environ|-addr]\n");

}

void Cmd_fork (){
	pid_t pid;
	
	if ((pid=fork())==0){
		printf ("ejecutando proceso %d\n", getpid());
	}
	else if (pid!=-1)
		waitpid (pid,NULL,0);
}

void fork_funct(char *input, char *input_trozos[], int n, bool *terminado){
    Cmd_fork();
}

char *aux_estado(int status){
    char *estado = (char *)malloc(30 * sizeof(char));
    
    if (WIFEXITED(status)) 
        strcpy(estado, "FINISHED");
    else if (WIFSTOPPED(status))
        strcpy(estado, "STOPPED");
    else if(WIFSIGNALED(status))
        strcpy(estado, "SIGNALED");
    else
        strcpy(estado, "ACTIVE");

    return estado;
}

void display_backprocess(tPosL p){
    Backprocess *process;
    process = (Backprocess *)getItem(p);

    if(getppid() == process->pid_padre){
        char buffer_time[20];
        struct tm *tm_date;

        tm_date = localtime(&process->launch_time);
        strftime(buffer_time, sizeof(buffer_time), "%Y/%m/%d %H:%M:%S", tm_date);

        printf("  %d        fer p=%d %s %s (%03d) %s\n", process->pid, getpriority(PRIO_PROCESS, process->pid), buffer_time, aux_estado(process->status), process->return_value, process->command_line);
    }
    else 
        return;
   
}
void Exec(char *input, char *input_trozos[], int n, bool *terminado) {
    char *args[n + 1];
    for (int i = 1; i < n; i++) {
      args[i - 1] = input_trozos[i];
    }
    args[n - 1] = NULL;

    execvp(input_trozos[1], args);
    perror("Imposible ejecutar");
}

void check_background_processes() {
    // Verifica el estado de los porcesos en segundo plano
    if(P != NULL){
        Backprocess *process;
        
        for (tPosL p = first(P); p != NULL; p = next(p)) {
            process = (Backprocess *)getItem(p);
            pid_t pid = waitpid(process->pid, &(process->status), WNOHANG);

            if (pid > 0) {
                if (WIFEXITED(process->status)) 
                    process->return_value = WEXITSTATUS(process->status);
                if (WIFSTOPPED(process->status))
                    process->return_value = WSTOPSIG(process->status);
                if(WIFSIGNALED(process->status))
                    process->return_value = WTERMSIG(process->status);
            }
        }
    }
    return;
}

void jobs(char *input, char *input_trozos[], int n, bool *terminado){
    if(P != NULL){
        check_background_processes();
        for(tPosL p = first(P); p != NULL; p = next(p)){
            display_backprocess(p);
        }
    }
    return;
}

void deleteProcesses(int target_status) {
    Backprocess *process;
    tPosL p;
    if(P != NULL){
        check_background_processes();
        p = first(P);
        process = (Backprocess *)getItem(p);
        while (p != NULL) {
            if (WIFEXITED(process->status) && getppid() == process->pid_padre){
                deleteItem(p, &P);
            }else if(WIFSIGNALED(process->status) && getppid() == process->pid_padre){
                deleteItem(p, &P);
            }
            else
                p=next(p);
        }
    }
}

void DeleteJobs(char *input, char *input_trozos[], int n, bool *terminado) {
    if (n == 1) {
        jobs(input, input_trozos, n, terminado);
        return;
    } else if (n == 2) {
        if (strcmp(input_trozos[1], "-term") == 0)
            deleteProcesses(1);
        if (strcmp(input_trozos[1], "-sig") == 0)
            deleteProcesses(3);
    }
}

void ProcessToForeground(pid_t pid, tPosL p) {
    if (tcsetpgrp(STDIN_FILENO, getpgid(pid)) == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }
    waitpid(pid, NULL, 0);
    deleteItem(p, &P);
}

void Job(char *input, char *input_trozos[], int n, bool *terminado) {
    if(P != NULL)
        check_background_processes();
    if (n > 1) {
        int fg = 0;
        pid_t pid;
        if (n == 3) {
            if (strcmp(input_trozos[1], "-fg") == 0){
                fg = 1;
                pid = (pid_t) atoi(input_trozos[2]);
            }else {
                jobs(input, input_trozos, n, terminado);
                return;
            }
    } else if (n == 2)
        pid = (pid_t) atoi(input_trozos[1]);

    Backprocess *current;
    tPosL p = P;

        while (p != NULL) {
            current = (Backprocess *)getItem(p);

            if (current->pid == pid && getppid() == current->pid_padre){
                if (fg){
                    if (WIFEXITED(current->status) || WIFSIGNALED(current->status)){
                    printf("Proceso %d no se puede llevar a primer plano\n", current->pid);
                    return;
                    }
                    ProcessToForeground(current->pid, p);
                    return;
                }
                display_backprocess(p);
                return; 
            }
            p = next(p);
        }
        jobs(input, input_trozos, n, terminado);
    }
    else 
        jobs(input, input_trozos, n, terminado);
}

void eec_aux(char *commands[], int n, int in_background){
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        execvp(commands[0], commands);
        perror("No ejecutado");
        exit(EXIT_FAILURE);   
    }
    else {
        if (!in_background)
            waitpid(pid, NULL, 0);
        else {
            Backprocess *p = malloc(sizeof(Backprocess));
            size_t BackprocessSize = sizeof(Backprocess);
            
            p->pid_padre = getppid();
            p->pid = pid;
            p->launch_time = time(NULL);
            p->status = -1;//activo
            

            strcpy(p->command_line, commands[0]);
            for (int i = 1; commands[i] != NULL; ++i) {
                strcat(p->command_line, " ");
                strcat(p->command_line, commands[i]);
            }
            insertItem(p, BackprocessSize, &P);
            num_back_process++;
            free(p);
        }
    }
}

void execute_external_command(char *input_trozos[], int n){
    int in_background = 0;
    for(int i = 0; i < n; i++){
        if(strcmp(input_trozos[i], "&") == 0){
            input_trozos[i]=NULL;
            in_background = 1;
            continue;
        }
    }
    eec_aux(input_trozos, n, in_background);

}
