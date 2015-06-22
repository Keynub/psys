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

    last_pid = 0;
    last_index = 0;
    last_queue = -1;

    INIT_LIST_HEAD(&process_queue);
    INIT_LIST_HEAD(&used_pid);

    uint32_t index = last_pid ++;

    int a = pcreate(3);

    int answer = 0;

    psend(a, 23);

    preceive(a, &answer);

    printf("ANSWER %d", answer);

    pdelete(a);

    process_tab[index].pid = index;
    process_tab[index].pid_pere = index;
    process_tab[index].vivant = true;
    strcpy(process_tab[index].name, "idle_p");
    process_tab[index].state = RUNNING;
    process_tab[index].prio = 1;
    INIT_LINK(& process_tab[index].chain);
    INIT_LIST_HEAD(&process_tab[index].enfants); // CHECK bien vide

    pidcell_t * ptr_elem;

    cur_proc = &process_tab[index];

    queue_for_each(ptr_elem, &(cur_proc -> enfants), pidcell_t, chain) {
        printf("foreach in start\n");
        printf("pid_son : %d\n", ptr_elem -> pid);
    }

    //    cree_processus("termm", &(test_terminaison));

    
    regler_frequence_horloge();
    demasque_IRQ();
    init_traitant_IT32(traitant_IT_32);

    queue_for_each(ptr_elem, &(cur_proc -> enfants), pidcell_t, chain) {
        printf("foreach before idle\n");
        printf("pid_son : %d\n", ptr_elem -> pid);
    }


    idle();

     while(1)
	  hlt();

    return;
}
