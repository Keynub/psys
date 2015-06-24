#include "process.h"
#include "global.h"
#include "stdio.h"
#include <string.h>
#include "queue.h"
#include "mem.h"

void ordonnance(){

    // if queue is empty, keep executing same process
    if(queue_empty(&process_queue)) { return; }

    process_t * next_proc = queue_out(&process_queue, process_t, chain);
    // TODO check for process waiting because if not, gets out
    if(est_vivant()) {
        queue_add(cur_proc, &process_queue, process_t, chain, prio);
        cur_proc -> state = WAITING;
    }

    while (next_proc -> state != WAITING && next_proc -> state != BLOCKED_CHILD) {
        queue_add(next_proc, &process_queue, process_t, chain, prio);
        next_proc = queue_out(&process_queue, process_t, chain);
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

bool has_son(int pid) {
    pidcell_t * ptr_elem;
    queue_for_each(ptr_elem, &(cur_proc -> enfants), pidcell_t, chain) {
        if(ptr_elem -> pid == pid) {
            return true;
        }
    }
    return false;
}

int waitpid(int pid, int *retvalp) {
    // trouver le fils qui correspond au pid
    if(has_son(pid)) {
        while(process_tab[pid].vivant) {
            cur_proc -> state = BLOCKED_CHILD;
            ordonnance();
        }
        *retvalp = process_tab[pid].retval;
        return 0;
    } else {
        return -1;
    }
}

/* Gère la terminaison d'un processus, */
/* la valeur retval est passée à  */
/* la fonction exit */
void terminaison(){
    cur_proc -> vivant = false;
    pidcell_t freed;
    freed.pid = mon_pid();
    freed.prio = 1;
    INIT_LINK(&freed.chain);
    queue_add(&freed, &used_pid, pidcell_t, chain, prio);

  // La valeur de retour de la fonction (et donc du processus) qui retourne se trouve dans %eax après la fin de la fonction.
  // Il faut donc la récupérer grâce à une fonction en assembleur avant de lancer "terminaison".

    ordonnance();

}

void exit(int retval){
   terminaison();
   if(mon_papa() == NULL){
        kill(mon_pid);
   }
   while(1){}
}

int cree_processus(const char * name, int prio, int (*code)(void)) {

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


    pidcell_t * newson = mem_alloc(sizeof(pidcell_t));
    newson->pid = pid;
    newson->prio = 1;
    INIT_LINK(&newson->chain);
    queue_add(newson, &cur_proc -> enfants, pidcell_t, chain, prio);

    INIT_LIST_HEAD(&process_tab[pid].enfants);

    strcpy( process_tab[pid].name, name);

    process_tab[pid].state = WAITING;
    process_tab[pid].reg[1] = (uint32_t) &(process_tab[pid].stack[STACK_SIZE -2]);
    process_tab[pid].stack[STACK_SIZE - 2] = (uint32_t) (code);
    process_tab[pid].stack[STACK_SIZE - 1] = (uint32_t) &(exitlol);

    queue_add(&process_tab[pid], &process_queue, process_t, chain, prio);

    return process_tab[pid].pid;

}

