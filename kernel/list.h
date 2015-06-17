#ifndef _LIST_H_
#define _LIST_H_

#include <inttypes.h>


struct cell{
	uint32_t index;
	struct cell* next;

};

typedef struct cell cell;
typedef cell* list;

void push(list* l,uint32_t id);
int * pop(list* l);
//void print(list l);

#endif
