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

         void Cmd_dup (char * tr[])
         {
             int df, duplicado;
             char aux[MAXNAME],*p;

             if (tr[0]==NULL || (df=atoi(tr[0]))<0) { /*no hay parametro*/
            ListOpenFiles(-1);                 /*o el descriptor es menor que 0*/
            return;
        }


        p=.....NombreFicheroDescriptor(df).......;
        sprintf (aux,"dup %d (%s)",df, p);
        .......AnadirAFicherosAbiertos......duplicado......aux.....fcntl(duplicado,F_GETFL).....;
    }


void authors(char *input_trozos[]){
   if(strcmp(input_trozos[1], "-n") == 0)
      printf("");
   else if(strcmp(input_trozos[1], "-l") == 0)
}
   
