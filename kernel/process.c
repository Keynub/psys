#include "process.h"
#include "global.h"
#include "stdio.h"
#include <string.h>

void ordonnance(){

    uint32_t next = (index_run + 1) % last_index ;
    while(!process_tab[next].vivant){
        next = (next + 1) % last_index ;
    }

    process_tab[index_run].state = WAITING;
    process_tab[next].state = RUNNING;
    uint32_t tmp = index_run; // need to store index_run or we can't change it before context switch

    index_run = next;
    ctx_sw(process_tab[tmp].reg, process_tab[next].reg);
}

char* mon_nom(){
    return process_tab[index_run].name;
}

int16_t mon_pid(){
    return process_tab[index_run].pid;
}

bool est_vivant(){
    return process_tab[index_run].vivant;
}

/* Gère la terminaison d'un processus, */
/* la valeur retval est passée au processus père */
/* quand il appelle waitpid. */
void terminaison(/*int retval*/){
    printf("APPPEEEEEEEEEEEEEEEEEEL A TERMINAISONNNNNNNNNNNNNNNNNNNNN\n");
    process_tab[index_run].vivant = false;
    
	push(&l, index_run);	
  
  // TODO valeur de retour pour waitpid
    ordonnance();

}

int cree_processus(const char * name, void (*code)(void)) {

 uint32_t index;

if (l!=NULL){
	index = *(pop(&l));
}
else{
	if(last_index == MAX_NB_PROCESS)
		return -1;
	else
	index = last_index ++;
}
  


    process_tab[index].pid = last_pid ++;
    process_tab[index].vivant = true;

    strcpy( process_tab[index].name, name);

    process_tab[index].state = WAITING;
    process_tab[index].reg[1] = (uint32_t) &(process_tab[index].stack[STACK_SIZE -2]);
    process_tab[index].stack[STACK_SIZE - 2] = (uint32_t) (code);
    process_tab[index].stack[STACK_SIZE - 1] = (uint32_t) &(terminaison);
       return process_tab[index].pid;

}

