#include "debugger.h"
#include "cpu.h"
#include "stdio.h"
#include "horloge.h"
#include "global.h"
#include "string.h"
#include "process.h"
#include "ecran.h"
#include "queue.h"
#include "test_token_ring.h"
#include "test_bak.h"

/*
 *   int16_t pid;
    char name[NAME_SIZE];
    uint8_t state;
    uint32_t reg[5];
    uint32_t stack[STACK_SIZE];
 */

int test_run(int n);

void kernel_start(void)
{
	cmpt = 0;
	p = CLOCKFREQ/SCHEDFREQ;
    l = NULL;

    last_pid = 0;
    last_index = 0;
    last_queue = -1;

    INIT_LIST_HEAD(&process_queue);
    INIT_LIST_HEAD(&used_pid);

    uint32_t index = last_pid ++;

    process_tab[index].pid = index;
    process_tab[index].pid_pere = index;
    process_tab[index].vivant = true;
    strcpy(process_tab[index].name, "idle_p");
    process_tab[index].state = RUNNING;
    process_tab[index].prio = 128;
    INIT_LINK(& process_tab[index].chain);
    INIT_LIST_HEAD(&process_tab[index].enfants); // CHECK bien vide

    cur_proc = &process_tab[index];

    //regler_frequence_horloge();
    demasque_IRQ();
    init_traitant_IT32(traitant_IT_32);



    test_run(1);
/*    unsigned long size = 3;
    const char* nom = "name";
    printf("TEST EXIT #####################\n");
    start(&(test_exit), size, 5, nom, NULL);
    idle((void*)1);
    printf("FIN EXIT ########################\n");*/

     while(1)
	  hlt();

    return;
}
