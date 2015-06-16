#include "debugger.h"
#include "cpu.h"
#include "stdio.h"

#include "global.h"
#include "string.h"
#include "test.h"
#include "process.h"
#include "ecran.h"

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
    
    idle();
    
  /*  for(;;) {
        for(int i = 0; i < 500000000 ; i++) {}
        printf("LICOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORNE");
    }*/
     while(1)
	  hlt();

    return;
}
