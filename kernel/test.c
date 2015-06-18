#include "test.h"
#include "cpu.h"
#include "process.h"
#include "global.h"
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


/*void idle(void)
{
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        ordonnance();
    }
}

void prog1(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        ordonnance(); 
    }
}*/

void test_terminaison(void) {
    for (int i = 0; i < 20; i++) {
        printf("i = %d [%s] pid = %i pidp = %i\n",i, mon_nom(), mon_pid(), mon_papa());
        cree_processus("termm", 1, &(sous_fifre));
        cree_processus("termm", 1, &(sous_fifre));
        sti();
        for (int32_t i = 0; i < 50000000; i++) {}
        cli();
    }

}

void sous_fifre(void) {
    printf("coucou je suis %d mon papa c'est %d\n", mon_pid(), mon_papa());
}

void idle()
{
	unsigned long i;
		printf("%s je lance termm\n", mon_nom());
    int pidf = cree_processus("termm", 1, &(test_terminaison));
    int retvalp;
    waitpid(pidf, &retvalp);
	while (1){
		printf("%s\n", mon_nom());
		sti();

		for (i = 0; i < 50000000; i++);
		cli();
	}
}

void prog1()
{
	unsigned long i;
	while (1){
		printf("%s\n", mon_nom());
		sti();

		for (i = 0; i < 50000000; i++);
		cli();
	}

}

