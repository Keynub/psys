#ifndef _HORLOGE_H_
#define _HORLOGE_H_

void tic_PIT(void);

void traitant_IT_32();

void init_traitant_IT32(void (*traitant)(void));

void regler_frequence_horloge();

void demasque_IRQ();

#endif
