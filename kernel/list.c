#include "list.h"
#include "mem.h"
#include "string.h"

void push(list* l,uint32_t id){

cell* c = (cell*) mem_alloc(sizeof(cell));
c->index = id;
c->next = NULL;
c->next = *l;
*l = c;

}


int * pop(list* l){

int * res = NULL;

if(l!=NULL){
       *res = (*l)->index;
cell* ptr = (*l)->next;
mem_free(l, sizeof(cell));
*l = ptr;
}
return res;

}



