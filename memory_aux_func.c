//
// Created by t_hss3 on 18/11/21.
//

#include "memory_aux_func.h"
void deleteFromMemoryGeneral(int search, MemList *L){
    MemPos p;
    p=findSizeMemo(search,*L);
    if(p!=NULL){
        deleteAtPositionMemo(p,L);
        return;
    }
}

void deleteFromMemoryShared(long key, MemList *L){
    MemPos p;
    for(p=*L;p!=MNULL;p=p->next){
        if(p->typeId==shared && p->info.key==key){
            deleteAtPositionMemo(p,L);
            p=MNULL;
        }
    }
}

void deleteFromMemoryMmap(char* filename, MemList *L){
    MemPos p;
    for(p=*L;p!=MNULL;p=p->next){
        if(p->typeId==shared && strcmp(p->info.file.filename,filename)==0){
            deleteAtPositionMemo(p,L);
            p=MNULL;
        }
    }
}