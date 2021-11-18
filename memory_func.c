//
// Created by t_hss3 on 17/11/21.
//

#include "memory_func.h"


void malloc_general(char *str, char *size, MemList *dynamic_register){
    long i;
    int in;
    char *rubbish,input_list[MAX_AUX_COMM];
    void *a;

    MemPos p,*aux;
    if(is_comm_void(size)){
        for(p=*dynamic_register;p!=NULL;p=p->next){
            printf("%p: size:%ld. malloc %d-%02d-%02d \n",p->memdir,p->size,p->date.tm_year+1900,p->date.tm_mon,p->date.tm_mday);
        }

    }
    if(is_comm_void(str)){
        i=str_to_int(size, rubbish);
        if(i>0 && rubbish==NULL){
            aux=createItemMemo(i);
            (*aux)->typeId=memo;
            insertItemMemo(aux,dynamic_register);
        }
        else if(strcmp(str,"free")==0){
            p=findSizeMemo(i,*dynamic_register);
            if(p!=NULL){
                deleteAtPositionMemo(p,dynamic_register);
                return;
            }
        }
        else{
            perror("Parámetros introducidos incorrectos");
        }
        return;
    }


}

void dealloc(char *str, tList* list_memo, MemList* L){

    tPosL p;
    if(is_comm_equals(str,"-malloc")){
        for(p=list_memo;p!=NULL;p=p->next){
            malloc_general("-free",p->data,L);
        }
    }
    else if(is_comm_equals(str,"-shared")){
        for(p=list_memo;p!=NULL;p=p->next){
            //malloc_general("-free",p->data,L);
        }
    }
    else if(is_comm_equals(str,"-mmap")){
        for(p=list_memo;p!=NULL;p=p->next){
         //   malloc_general("-free",p->data,L);
        }
    }
    else if(is_comm_void(str) && !is_comm_void((*list_memo)->data)){
        MemPos f;
        void* i;
        char* rubbish;
        for(p=*list_memo;p!=NULL;p=p->next){
            i= (void *) str_to_int_base(p->data,rubbish,16);
            f= findDirMemo(i,*L);
            if(f!=NULL){
                deleteAtPositionMemo(f,L);
            }
        }
    }
    else if(is_comm_void(str)){
        print_memory_list(*L);
    }
}






ssize_t LeerFichero (char *fich, void *p, ssize_t n)
{ /* le n bytes del fichero fich en p */

    ssize_t nleidos,tam=n; /*si n==-1 lee el fichero completo*/
    int df, aux;
    struct stat s;
    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
        return ((ssize_t)-1);
    if (n==LEERCOMPLETO)
        tam=(ssize_t) s.st_size;
    if ((nleidos=read(df,p, tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return ((ssize_t)-1);
    }
    close (df);
    return (nleidos);
}

void * MmapFichero (char * fichero, int protection)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    if (protection&PROT_WRITE) modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;
/*Guardar Direccion de Mmap (p, s.st_size,fichero,df......);*/
    return p;
}

void Mmap (char *str, char* str2, char *fich) {
    /*arg[0] is the file name
and arg[1] is the permissions*/

    char *perm;
    void *p;
    int protection=0;
    if (strcmp(str,"-free")!=0)
    {/*Listar Direcciones de Memoria mmap;*/ return;}
    if (strcmp(str2,"perm")!=0 && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MmapFichero(str,protection))==NULL){
        perror ("Imposible mapear fichero");
    }


    else{
        printf ("fichero %s mapeado en %p\n", str, p);
    }

}

void * ObtenerMemoriaShmget (key_t clave, size_t tam, MemList* L)
{ /*Obtienen un puntero a una zaona de memoria compartida*/
/*si tam >0 intenta crearla y si tam==0 asume que existe*/
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    struct tMemory* item;
    description description;
    if (tam) /*si tam no es 0 la crea en modo exclusivo
esta funcion vale para shared y shared -create*/
        flags=flags | IPC_CREAT | IPC_EXCL;
/*si tam es 0 intenta acceder a una ya creada*/
    if (clave==IPC_PRIVATE) /*no nos vale*/
    {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno; /*si se ha creado y no se puede mapear*/
        if (tam) /*se borra */
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    item=createItemMemo((long int)tam);
    description= createDescriptorShared(clave);
    modifyItem(item,"shared",description);
    insertItemMemo(item,L);
/* Guardar En Direcciones de Memoria Shared (p, s.shm_segsz, clave.....);*/
    return (p);
}

void SharedCreate (char *str, char *str2,MemList* L) /*arg[0] is the key
and arg[1] is the size*/
{
    key_t k;
    size_t tam=0;
    void *p;
    if (str==NULL || str2==NULL ) {
        print_memory_list(*L);
        return;
    }

    k=(key_t) atoi(str);
    if (str2!=NULL)tam=(size_t) atoll(str2);
    if ((p=ObtenerMemoriaShmget(k,tam))==NULL)
        perror ("Imposible obtener memoria shmget");
    else{
        printf ("Memoria de shmget de clave %d asignada en %p\n",k,p);
    }

}
void SharedDelkey (char *str) /*arg[0] points to a str containing the key*/{
    key_t clave;
    int id;
    char *key=str;

    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf (" shared -delkey clave_valida\n");
        return;
    }

    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }

    if (shmctl(id,IPC_RMID,NULL)==-1) perror ("shmctl: imposible eliminar memoria compartida\n");
}

void dopmap () /*no arguments necessary*/{ pid_t pid; /*ejecuta el comando externo pmap para */
    char elpid[32]; /*pasandole el pid del proceso actual */
    char *argv[3]={"pmap",elpid,NULL};
    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){
        if (execvp(argv[0],argv)==-1)
            perror("cannot execute pmap");
        exit(1);
    }
    waitpid (pid,NULL,0);
}


void main_shared(char* str,char* key, char* tam, MemList* L){
    if(is_comm_equals(str,"-create")){
        SharedCreate(str,key);

    }
    else if(is_comm_equals(str,"-delkey")){
        SharedDelkey(key);
    }
    else if(is_comm_equals(str,"-free")){

    }
    else if(is_comm_void(str)){

    }


}
long str_to_int_base(char* str, char* rubbish, int base){
    long i;
    i= strtol(str,&rubbish,base);
    return i;
}

