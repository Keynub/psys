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

int test_exit(void * arg) {
arg++;
for (int i = 0; i < 20; i++) {
        printf("MON retour : %d\n", process_tab[mon_pid()].retval);
        int retval = 5;
        exit(retval);
        printf("VALEUR DE RETOUR : %d\n", retval);
   }
    return 3;

}

/*int test_terminaison(void) {
    for (int i = 0; i < 20; i++) {
        printf("i = %d [%s] pid = %i pidp = %i\n",i, mon_nom(), mon_pid(), mon_papa());
        int pidf = cree_processus("termm", 1, &(sous_fifre));
        cree_processus("termm", 1, &(sous_fifre));
        sti();
        for (int32_t i = 0; i < 50000000; i++) {}
        cli();
        printf("retour premier fils : %d\n", process_tab[pidf].retval);
    }
    return 3;

}*/


/*int sous_fifre(void * truc) {
    printf("coucou je suis %d mon papa c'est %d\n", mon_pid(), mon_papa());
    printf("mon adresse en pile c'est %x et j'ai empilé mon paramètre en %x mais je le lis en %x", (unsigned int) &(cur_proc->stack[STACK_SIZE -4]), (unsigned int) &(cur_proc->stack[STACK_SIZE-2]), (unsigned int) &truc);
    uint32_t a = *((uint32_t *) (truc));
    printf("param : %d", a);
    return a;
}*/


int idle(void * arg)
{
    (void)arg;
	//unsigned long i;
	while(1) {
	    sti(); hlt(); cli();
	}

}
