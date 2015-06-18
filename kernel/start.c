#include "debugger.h"
#include "cpu.h"
#include "stdio.h"
#include "horloge.h"
#include "global.h"
#include "string.h"
#include "test.h"
#include "process.h"
#include "ecran.h"
#include "queue.h"

/*
 *   int16_t pid;
    char name[NAME_SIZE];
    uint8_t state;
    uint32_t reg[5];
    uint32_t stack[STACK_SIZE];
 */

void kernel_start(void)
{
	cmpt = 0;
	p = CLOCKFREQ/SCHEDFREQ;
    l = NULL;

    process_queue.prev = & process_queue;
    process_queue.next = & process_queue;

    last_pid = 0;
    last_index = 0;


    process_t idle_p;
    idle_p.pid = last_pid ++;
    idle_p.vivant = true;
    strcpy(idle_p.name, "idle_p");
    idle_p.state = RUNNING;
    idle_p.prio = 3;
    INIT_LINK(& idle_p.chain);

    process_tab[last_index ++] = idle_p;


    cree_processus("prog1", 2, &(prog1));
    cree_processus("prog2", 2, &(prog1));
    cree_processus("prog3", 1, &(prog1));
//    cree_processus("termm", &(test_terminaison));

    cur_proc = &process_tab[0];
    
    regler_frequence_horloge();
    demasque_IRQ();
    init_traitant_IT32(traitant_IT_32);

    
    idle();

     while(1)
	  hlt();

    return;
}
