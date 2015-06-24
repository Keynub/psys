#include "process.h"
#include "global.h"
#include "stdio.h"
#include "const.h"
#include <string.h>
#include "queue.h"
#include "mem.h"
#include "cpu.h"
#include <stdio.h>

void ordonnance(){
    // if queue is empty, keep executing same process
    if(queue_empty(&process_queue)) { return; }

    // TODO check for process waiting because if not, gets out
    if(est_vivant()) {
        queue_add(cur_proc, &process_queue, process_t, chain, prio);
        cur_proc -> state = WAITING;
    }

    process_t * next_proc = queue_out(&process_queue, process_t, chain);

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

int getpid() {
    return mon_pid();
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
        if(retvalp != 0) {
            *retvalp = process_tab[pid].retval;
        }
        return pid;
    } else {
        return -1;
    }
}

void wait_clock(unsigned long clock){
    process_tab[mon_pid()].state = SLEEP;
    while(horloge < clock){}
    process_tab[mon_pid()].state = RUNNING;
}

void delete_queue( process_t * p){
    p -> vivant = false;
    pidcell_t * freed = mem_alloc(sizeof(pidcell_t));
    freed->pid = p->pid;
    freed->prio = 1;
    INIT_LINK(&freed->chain);
    queue_add(freed, &used_pid, pidcell_t, chain, prio);
}

/* Gère la terminaison d'un processus, */
/* la valeur retval est passée au processus père */
/* quand il appelle waitpid. */
void terminaison(){
    delete_queue(cur_proc);
  // La valeur de retour de la fonction (et donc du processus) qui retourne se trouve dans %eax après la fin de la fonction.
  // Il faut donc la récupérer grâce à une fonction en assembleur avant de lancer "terminaison".
    ordonnance();

}


void rienfaire(unsigned long ssize) {
    ssize = ssize;
}

int start(int (*pt_func)(void*), unsigned long ssize, int prio, const char *name, void *arg) {
    uint32_t pid;


    if (!queue_empty(&used_pid)){
        pidcell_t * pidcell = queue_out(&used_pid, pidcell_t, chain);
        pid = pidcell -> pid;
        // TODO free pidcell
    } else {
        if(last_pid == MAX_NB_PROCESS)
            return -1;
        else
        pid = last_pid ++;
    }

    // TODO utiliser ssize
    rienfaire(ssize);
    process_tab[pid].pid_pere = mon_pid();
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
    process_tab[pid].reg[1] = (uint32_t) &(process_tab[pid].stack[STACK_SIZE -3]);
    process_tab[pid].stack[STACK_SIZE - 3] = (uint32_t) (pt_func);
    process_tab[pid].stack[STACK_SIZE - 2] = (uint32_t) &(exitlol);
    process_tab[pid].stack[STACK_SIZE - 1] = (uint32_t) (arg);

    queue_add(&process_tab[pid], &process_queue, process_t, chain, prio);

    if(prio > getprio(getpid())) {
        ordonnance();
    }

    return process_tab[pid].pid;
}

int getprio(int pid) {
    return process_tab[pid].prio;
}

bool in_proc_queue(int pid) {
    return process_tab[pid].chain.next != NULL || process_tab[pid].chain.prev != NULL;
}

int chprio(int pid, int newprio) {
    // TODO check pid
    int oldprio = getprio(pid);
    if(oldprio != newprio) {
        process_tab[pid].prio = newprio;
        if(in_proc_queue(pid)){
            queue_del(& process_tab[pid], chain);
            queue_add(& process_tab[pid], & process_queue, process_t, chain, prio);
        }
        ordonnance();
    }

    return oldprio;
}

int kill(int pid){
  if (pid<0 || pid >= MAX_NB_PROCESS || ! process_tab[pid].vivant){
    return -1;
  } else{
    if(pid != mon_pid()) {
        queue_del(&process_tab[pid], chain);
    }
   
    if (process_tab[pid].state == BLOCKED_SEM){
      //TODO SEM
    }
    else 
      if(process_tab[pid].state == BLOCKED_IO){
	//TODO IO
      }
      else 
	if(process_tab[pid].state == BLOCKED_CHILD){
	  //TODO GestionFils
    }
    delete_queue(&process_tab[pid]);
    return 0;
  }

}

void exit(int retval){
   process_tab[mon_pid()].retval = retval;
   printf("NEW RETVAL : %d\n", process_tab[mon_pid()].retval);
   kill(mon_pid());
   while(1){
    sti(); hlt(); cli();
   }
}

