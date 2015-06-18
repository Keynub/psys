#include "process.h"
#include "global.h"
#include "stdio.h"
#include <string.h>
#include "queue.h"

void ordonnance(){

    if(est_vivant()) {
        queue_add(cur_proc, &process_queue, process_t, chain, prio);
        cur_proc -> state = WAITING;
    }
    process_t * next_proc = queue_out(&process_queue, process_t, chain);

    next_proc -> state = RUNNING;
    process_t * tmp = cur_proc; // need to store cur_proc or we can't change it before context switch

    cur_proc = next_proc;
    ctx_sw(tmp -> reg, next_proc -> reg);
}

char* mon_nom(){
    return cur_proc -> name;
}

int16_t mon_pid(){
    return cur_proc -> pid;
}

bool est_vivant(){
    return cur_proc -> vivant;
}

/* Gère la terminaison d'un processus, */
/* la valeur retval est passée au processus père */
/* quand il appelle waitpid. */
void terminaison(/*int retval*/){
    cur_proc -> vivant = false;
	push(&l, cur_proc -> pid);
  // TODO valeur de retour pour waitpid
    ordonnance();
}

int cree_processus(const char * name, void (*code)(void)) {

    uint32_t pid;

    if (l!=NULL){
        pid = *(pop(&l));
    }
    else{
        if(last_pid == MAX_NB_PROCESS)
            return -1;
        else
        pid = last_pid ++;
    }
  



    process_tab[pid].pid = pid;
    process_tab[pid].vivant = true;
    process_tab[pid].prio = 1;
    INIT_LINK(& (process_tab[pid].chain));

    strcpy( process_tab[pid].name, name);

    process_tab[pid].state = WAITING;
    process_tab[pid].reg[1] = (uint32_t) &(process_tab[pid].stack[STACK_SIZE -2]);
    process_tab[pid].stack[STACK_SIZE - 2] = (uint32_t) (code);
    process_tab[pid].stack[STACK_SIZE - 1] = (uint32_t) &(terminaison);

    queue_add(&process_tab[pid], &process_queue, process_t, chain, prio);

    return process_tab[pid].pid;

}

