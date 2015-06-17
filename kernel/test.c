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
        printf("i = %d [%s] pid = %i\n",i, mon_nom(), mon_pid());
        sti();
        for (int32_t i = 0; i < 10000000; i++) {}
        cli();
    }

}

void idle()
{
	unsigned long i;
	while (1){
		printf("A\n");
		sti();

		for (i = 0; i < 5000000; i++);
		cli();
	}
}

void prog1()
{
	unsigned long i;
	while (1){
		printf("B\n");
		sti();

		for (i = 0; i < 5000000; i++);
		cli();
	}
}

