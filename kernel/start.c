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

    used_pid.prev = & used_pid;
    used_pid.next = & used_pid;

    last_pid = 0;
    last_index = 0;


    process_t idle_p;
    idle_p.pid = last_pid ++;
    idle_p.pid_pere = idle_p.pid;
    idle_p.vivant = true;
    strcpy(idle_p.name, "idle_p");
    idle_p.state = RUNNING;
    idle_p.prio = 1;
    INIT_LINK(& idle_p.chain);


    process_tab[last_index ++] = idle_p;


    cur_proc = &process_tab[0];

    //    cree_processus("termm", &(test_terminaison));



    
    regler_frequence_horloge();
    demasque_IRQ();
    init_traitant_IT32(traitant_IT_32);



    idle();

     while(1)
	  hlt();

    return;
}
