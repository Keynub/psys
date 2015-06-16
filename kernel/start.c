#include "debugger.h"
#include "cpu.h"
#include "stdio.h"

#include "global.h"
#include "string.h"
#include "test.h"
#include "process.h"
#include "ecran.h"
#include "horloge.h"

/*
 *   int16_t pid;
    char name[NAME_SIZE];
    uint8_t state;
    uint32_t reg[5];
    uint32_t stack[STACK_SIZE];
 */

void kernel_start(void)
{

    last_pid = 0;
    last_index = 0;

    process_t idle_p;
    idle_p.pid = last_pid ++;
    strcpy(idle_p.name, "idle_p");
    idle_p.state = RUNNING;

    process_tab[last_index ++] = idle_p;

    cree_processus("prog1", prog1);
    
    index_run = 0;

    
    /*regler_frequence_horloge();
    demasque_IRQ();
    init_traitant_IT32(traitant_IT_32);*/

    
    idle();

     while(1)
	  hlt();

    return;
}
