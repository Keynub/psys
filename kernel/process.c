#include "process.h"
#include "global.h"
#include "stdio.h"
#include <string.h>
#include "queue.h"

void ordonnance(){

    // if queue is empty, keep executing same process
    if(queue_empty(&process_queue)) { return; }

    process_t * next_proc = queue_out(&process_queue, process_t, chain);

    if(est_vivant()) {
        queue_add(cur_proc, &process_queue, process_t, chain, prio);
        cur_proc -> state = WAITING;
    }

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

int16_t mon_papa(){
    return cur_proc -> pid_pere;
}

bool est_vivant(){
    return cur_proc -> vivant;
}

/* Gère la terminaison d'un processus, */
/* la valeur retval est passée au processus père */
/* quand il appelle waitpid. */
void terminaison(/*int retval*/){
    cur_proc -> vivant = false;
    pidcell_t freed;
    freed.pid = mon_pid();
    freed.prio = 1;
    INIT_LINK(&freed.chain);
    queue_add(&freed, &used_pid, pidcell_t, chain, prio);
  // TODO valeur de retour pour waitpid
  // La valeur de retour de la fonction (et donc du processus) qui retourne se trouve dans %eax après la fin de la fonction.
  // Il faut donc la récupérer grâce à une fonction en assembleur avant de lancer "terminaison".

    ordonnance();
}

int waitpid(int pid, int *retvalp) {
    return *retvalp + pid;
}

int cree_processus(const char * name, int prio, void (*code)(void)) {

    uint32_t pid;

    if (!queue_empty(&used_pid)){
        pidcell_t * pidcell = queue_out(&used_pid, pidcell_t, chain);
        pid = pidcell -> pid;
    } else {
        if(last_pid == MAX_NB_PROCESS)
            return -1;
        else
        pid = last_pid ++;
    }


    process_tab[pid].pid_pere = mon_pid(); // fail
    process_tab[pid].pid = pid;
    process_tab[pid].vivant = true;
    process_tab[pid].prio = prio <= MAX_PRIO ? prio : MAX_PRIO; // min(prio, MAX_PRIO)

    INIT_LINK(& (process_tab[pid].chain));


    strcpy( process_tab[pid].name, name);

    process_tab[pid].state = WAITING;
    process_tab[pid].reg[1] = (uint32_t) &(process_tab[pid].stack[STACK_SIZE -2]);
    process_tab[pid].stack[STACK_SIZE - 2] = (uint32_t) (code);
    process_tab[pid].stack[STACK_SIZE - 1] = (uint32_t) &(exitlol);

    queue_add(&process_tab[pid], &process_queue, process_t, chain, prio);

    return process_tab[pid].pid;

}

