#include "test_bak.h"
#include "cpu.h"
#include "process.h"
#include "global.h"
#include "const.h"
#include "horloge.h"
#include "stddef.h"
#include <stdio.h>


/*    void idle(void)
{
      for (;;) {
        for (int i = 0; i < 500000; i++) {}
        printf("A");
        ctx_sw(process_tab[0].reg, process_tab[1].reg);
    }
    printf("[idle] je bloque le systeme\n");
    hlt();
}

void prog1(void)
{   for(;;) {
        for (int i = 0; i < 500000; i++) {}
        printf("B");
        ctx_sw(process_tab[1].reg, process_tab[0].reg);
    }
}
*/


void idle(void)
{
    start(&prog1,5,128,"prog1",NULL);
    for (int a=0 ;a<5;a++) {
        printf("idle %lu", current_clock());
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();

        for (int i = 0; i < 50000000; i++);
        cli();
    }
}

int prog1(void) {
    for (int a=0 ;a<5;a++) {
        printf("prog1 %lu", current_clock());
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();
        for (int i = 0; i < 50000000; i++);
        cli();
    }
    return 1;
}


int sous_fifre(void * truc) {
    printf("coucou je suis %d mon papa c'est %d\n", mon_pid(), mon_papa());
    printf("mon adresse en pile c'est %x et j'ai empilé mon paramètre en %x mais je le lis en %x", (unsigned int) &(cur_proc->stack[STACK_SIZE -4]), (unsigned int) &(cur_proc->stack[STACK_SIZE-2]), (unsigned int) &truc);
    uint32_t a = *((uint32_t *) (truc));
    printf("param : %d", a);
    return a;
}

/*void idle()
{
	unsigned long i;
    printf("%s je lance termm\n", mon_nom());
    int a = 42;
    void * b = (void *) &a;
    int pidf = start(&(sous_fifre), 24, 1, "termm", b);
    int retvalp;
    waitpid(pidf, &retvalp);
    printf("termm m'a renvoyé %d\n", retvalp);
	while (1){
		//printf("%s\n", mon_nom());
		sti();

		for (i = 0; i < 50000000; i++);
		cli();
	}
}*/
