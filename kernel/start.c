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

    process_t idle_p, prog_p;
    idle_p.pid = 0;
    strcpy(idle_p.name, "idle_p");
    idle_p.state = RUNNING;

    prog_p.pid = 1;
    strcpy( prog_p.name, " prog_p");
    prog_p.state = WAITING;
     
    prog_p.reg[1] = (uint32_t) &prog_p.stack;
    prog_p.stack[0]= (uint32_t) &(prog1);
    
    process_tab[0] = idle_p;
    process_tab[1] = prog_p;
    
    index_run = 0;

    
    /*regler_frequence_horloge();
    demasque_IRQ();
    init_traitant_IT32(traitant_IT_32);*/

    
    idle();

     while(1)
	  hlt();

    return;
}
