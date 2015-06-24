#include "process.h"
#include "global.h"
#include "stdio.h"
#include "const.h"
#include <string.h>
#include "queue.h"
#include "mem.h"
#include "cpu.h"
#include "../shared/queue.h"
#include <stdio.h>

void ordonnance(){
    // if queue is empty, keep executing same process
    if(queue_empty(&process_queue)) { return; }

    // TODO check for process waiting because if not, gets out
    if(est_vivant() && cur_proc->state == RUNNING) {
        // on ne se remet pas dans la file d'ordonnancement si on a prévu d'attendre
        queue_add(cur_proc, &process_queue, process_t, chain, prio);
        cur_proc -> state = WAITING;
    }

    process_t * next_proc = queue_out(&process_queue, process_t, chain);

    while (next_proc -> state != WAITING) {
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

bool has_any_son() {
    return !queue_empty(&cur_proc -> enfants);
}

int16_t has_zombie_son() {
    pidcell_t * ptr_elem;
    queue_for_each(ptr_elem, &(cur_proc -> enfants), pidcell_t, chain) {
        if(process_tab[ptr_elem -> pid].state == ZOMBIE) {
            return ptr_elem -> pid;
        }
    }
    return -1;
}

/*
 * Mort définitive d'un processus : il s'indique non vivant et donne son PID
 * au garbage collector.
 */
void delete_queue( process_t * p){
    // passage au garbage collector.
    p -> vivant = false;
    pidcell_t * freed = mem_alloc(sizeof(pidcell_t));
    freed->pid = p->pid;
    freed->prio = 1;
    INIT_LINK(&freed->chain);
    queue_add(freed, &used_pid, pidcell_t, chain, prio);
}

int waitpid(int pid, int *retvalp) {
    if(!has_any_son()) {
        return -1;
    }
    // deux cas
    // cas 1 : n'importe quel fils doit mourir
    if(pid < 0) {
        int16_t pid_zombie_son = has_zombie_son();
        while(pid_zombie_son < 0) {
            cur_proc -> state = BLOCKED_CHILD;
            ordonnance();
            pid_zombie_son = has_zombie_son();
        }
        // le fils est zombie
        if(retvalp != 0) {
            *retvalp = process_tab[pid_zombie_son].retval;
        }
        // maintenant, on termine définitivement le fils
        // passage de son pid au garbage collector
        delete_queue(&process_tab[pid_zombie_son]);
        return pid_zombie_son;
    } // deuxième cas : trouver le fils qui correspond au pid
    else if(has_son(pid)) {
        while(process_tab[pid].state != ZOMBIE) {
            // le premier passage est obligatoire
            // les passages suivants sont quand un fils me réveille
            // je me rendors.
            cur_proc->state = BLOCKED_CHILD;
            ordonnance();
        }
        // le fils est zombie
        if(retvalp != 0) {
            *retvalp = process_tab[pid].retval;
        }
        // maintenant, on termine définitivement le fils
        // passage de son pid au garbage collector
        delete_queue(&process_tab[pid]);
        return pid;
    } else {
        return -1;
    }
}

/*
 * Indique aux fils d'un processus que ce processus meurt.
 * Désalloue la structure de stockage de fils en même temps.
 */
void delete_children( process_t * p){
    while (!queue_empty(&p->enfants)){
        pidcell_t * pidcell = queue_out(&p -> enfants, pidcell_t, chain);
        int16_t pid = pidcell -> pid;
        // on indique à chaque fils que son père est mort
        process_tab[pid].pid_pere = -1;
        // on libère toutes les cellules qui contenaient les enfants
        mem_free(pidcell, sizeof(pidcell_t));
    }
}

/*
 * Gère les aspects communs de la terminaison.
 * Ces aspects comprennent :
 * - gérer la filiation (prévenir les fils du décédé pour qu'ils ne passent pas zombie mais meurent direct)
 * - désallouer la liste des fils
 * - si mon père existe :
 *      passer zombie, le prévenir s'il m'attend
 *   sinon:
 *      libérer mon pid et m'indiquer mort
 * - finalement, si j'étais en cours d'exécution, je devrais laisser la main.
 */
void terminaison(){
    // je préviens mes fils de mon décès
    delete_children(cur_proc);
    // je regarde si mon père est encore vivant et m'attend
    if(mon_papa() >= 0) {
        // mon père vit : je ne peux pas encore mourir
        // je mourrai dans waitpid
        cur_proc->state = ZOMBIE;
        if(process_tab[mon_papa()].state == BLOCKED_CHILD) {
            // il attend moi ou un de mes frères
            process_tab[mon_papa()].state = WAITING;
            queue_add(& process_tab[mon_papa()], &process_queue, process_t, chain, prio);
            // je préviens mon père qu'il pourra se réveiller
            // si je n'étais pas le bon fils, il se rendormira
            // sinon, il sera libéré
        }
    } else {
        // si mon papa est mort, je peux mourir
        // je m'indique comme mort et je permets aux futurs processus de voler mon adresse
        delete_queue(cur_proc);
    }
    // place aux jeunes
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
    }
   switch(process_tab[pid].state) {
       case WAITING:
           queue_del(&process_tab[pid], chain);
           break;
       case RUNNING:
           break;
       case BLOCKED_SEM:
       case BLOCKED_IO:
           //todo
           break;
       case BLOCKED_CHILD:
           // todo child
           break;
       case SLEEP:
           // todo
           break;
       case ZOMBIE:
           return -1;
       default:
           // todo panic
           break;

   }
      delete_children(cur_proc);
      if(cur_proc->pid_pere > 0) {
          // mon père vit : je ne peux pas encore mourir
          // je mourrai dans waitpid
          cur_proc->state = ZOMBIE;
          if(process_tab[cur_proc->pid_pere].state == BLOCKED_CHILD) {
              // il attend moi ou un de mes frères
              process_tab[cur_proc->pid_pere].state = WAITING;
              queue_add(& process_tab[mon_papa()], &process_queue, process_t, chain, prio);
              // je préviens mon père qu'il pourra se réveiller
              // si je n'étais pas le bon fils, il se rendormira
              // sinon, il sera libéré
          }
      } else {
          delete_queue(&process_tab[pid]);
      }
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

