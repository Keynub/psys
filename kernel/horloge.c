#include <inttypes.h>
#include "ecran.h"
#include "process.h"
#include "stdio.h"
#include "const.h"
#include "global.h"
#include "segment.h"
#include "mem.h"
#include <stdlib.h>
#include <cpu.h>
#include "queue.h"
#include "../shared/queue.h"


void tic_PIT()
{
    horloge ++;

    while ((!queue_empty(&sleeping)) && horloge >= (queue_top(&sleeping, sleeping_t, chain))->clock) {
        sleeping_t *sleep = queue_out(&sleeping, sleeping_t, chain);
        process_tab[sleep->pid].state = WAITING;
        queue_add(&process_tab[sleep->pid], &process_queue, process_t, chain, prio);
        mem_free(sleep, sizeof(sleeping_t));

        // ordonnance si prioritaire ? y penser.
    }

if(cmpt==p){
	cmpt=0;	
	ordonnance();
}
else
	cmpt++;


	// TODO table des processus en attente d'un t à parcourir

}

void init_traitant_IT32(void (*traitant)(void))
{
    /*
     *   le premier mot de l'entrée est composé de la constante sur 16 bits KERNEL_CS (bits 31 à 16) et des 
     *   16 bits de poids faibles de l'adresse du traitant (bits 15 à 0) ;
     *   le deuxième mot est composé des 16 bits de poids forts de l'adresse du traitant (bits 31 à 16) et de 
     *   la constante 0x8E00 (bits 15 à 0). 
     */
    
    uint32_t entry_high = (KERNEL_CS << 16) | ((uint32_t) traitant & 0x0000FFFF);
    uint32_t entry_low = ((uint32_t) traitant & 0xFFFF0000) | CONST_VECT_INT;
    // index table[32] : 2 cases sur 8 octets chacune
    uint32_t * address_entry = (uint32_t *) (TABLE_VECT_INT_32 + 8 * 32);
    * address_entry = entry_high;
    address_entry += 1;
    // address_entry = (uint32_t *) (TABLE_VECT_INT_32 + 8 * 32 + 4);
    * address_entry = entry_low;
}

/*
void regler_frequence_horloge()
{
    uint16_t freq = QUARTZ / CLOCKFREQ;
    uint8_t freq_low = freq % 256;
    uint8_t freq_hi = freq >> 8;
    
   outb(SET_FREQUENCY_CMD, CLOCK_CMD_PORT);
   outb(freq_low, CLOCK_DATA_PORT);
   outb(freq_hi, CLOCK_DATA_PORT);
}*/

void regler_frequence_horloge();

/*
void demasque_IRQ()
{
    uint8_t mask;
    mask = inb(MASK_DATA_PORT);
    mask = mask & 0xFE;
    outb(mask, MASK_DATA_PORT);
}*/

void demasque_IRQ();

unsigned long current_clock()
{
    return horloge;
}

/*Retourne dans *quartz la fréquence du quartz du système et dans *ticks le nombre d'oscillations du quartz entre chaque interruption. */
void clock_settings(unsigned long *quartz, unsigned long *ticks)
{
    * quartz = QUARTZ;
    *ticks = QUARTZ/CLOCKFREQ;
}
