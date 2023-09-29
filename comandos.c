void createEmptyList(listFiles *L) {
    *L = NULL;
}
tPosL last(listFiles L) {
    tPosL x = L;
    while (x->next != NULL) {
        x = x->next;
    }
    return x;
}

tPosL previous(tPosL p, listFiles L) {
    tPosL x = L;
    while (x->next != NULL && x->next != p) {
        x = x->next;
    }
        return x;
}

int countFiles(listFiles *L){
    tPosL x = *L;
    int a = 0;
    while (x->next != NULL) {
        x = x->next;
        a++;
    }
    return a;
}
void insertFile(int df, int mode, char name[MAXNAME], listFiles *L){
    tPosL aux,x;
    aux->df=df;
    aux->modo=mode;
    stpcpy(aux->name,name);
    aux->next = NULL;

    if (*L==NULL){
        *L = aux;
    } else{
        x = last(*L);
        x->next = aux;
    }
    open(name, mode, 0777);
}

void ListOpenFiles(listFiles L){
    tPosL x = L;
    int i = 0;
    char aux[15];
    while (x->next != NULL) {
        i++;
        if (x->modo == 0100) strcpy(aux,"O_CREAT");
        else if (x->modo == 0200) strcpy(aux,"O_EXCL");
        else if (x->modo == 0) strcpy(aux," O_RDONLY");
        else if (x->modo == 01) strcpy(aux,"O_WRONLY");
        else if (x->modo == 02) strcpy(aux,"O_RDWR");
        else if (x->modo == 02000) strcpy(aux,"O_APPEND");
        else if (x->modo == 01000) strcpy(aux,"O_TRUNC");
        printf("decriptor: %d -> %s %s\n", i, x->name, aux);
       x = x->next;
    }
}

void CloseOpenFile(listFiles *L, int df){
    tPosL x = *L;
    while (x != NULL || x->df != df) {
        x = x->next;
    }
    if (x == NULL){
        printf("No se ha podido encontrar el descriptor");
    }else if (x->df == df){
        if (x == *L){
            *L = x->next;
        }else{
            previous(x, *L)->next = x->next;
        }
        free(x);
        close(df);
    }
}

void Cmd_open (char * tr[], listFiles *L) {
    int i, df, mode = 0, numFilesOpen;

    if (tr[0] == NULL) { /*no hay parametro*/
        /* ..............ListarFicherosAbiertos...............*/
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

        if (tr[0]==NULL || (df=atoi(tr[0]))<0) { /*no hay parametro*/
            /* ..............ListarFicherosAbiertos...............*/ /*o el descriptor es menor que 0*/
            ListOpenFiles(*L);
            return;
        }


        if (close(df)==-1)
            perror("Imposible cerrar descriptor");
        else
            /* ........EliminarDeFicherosAbiertos......*/
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

oid repeat_command(char *input_trozos[], /*lista*/){


}



void hist(char *input_trozos[]){




}

void help(char *commands[], char *input_trozos[], int nComands){
    int i;

    if(strcmp(input_trozos[1], "") != 0) {
        for (int i = 0; i < nComands && strcmp(commands[i], input_trozos[1]) != 0; i++);
        switch (i) {
            case 1 :
                printf("authors [-n|-l]\t Muestra los nombres y/o logins de los autores\n");
                break;
            case 2:
                printf("id [-p]\t Muestra el pid del shell o de su proceso padre\n");
                break;
            case 3:
                pritnf("chdir [dir]\t	Cambia (o muestra) el directorio actual del shell\n");
                break;
            case 4:
                prinf("date\tMuestra la fecha acual");
                break;
            case 5:
                printf("");
                break;
            case 6:
                printf("");
                break;
            case 7:
                printf("");
                break;
            case 8:
                pritnf("");
                break;
            case 9:
                printf("");
                break;
            case 10:
                printf("");
                break;
            case 11:
                printf("");
                break;
            case 12:
                printf("");
                break;
            case 13:
                printf("");
                break;
            case 14:
                printf("");
                break;
            case 15:
                printf("");
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
   
