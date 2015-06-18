#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <inttypes.h>

#include <stdbool.h>

#include "const.h"
#include "queue.h"

enum {RUNNING, WAITING, BLOCKED_SEM, BLOCKED_IO, BLOCKED_CHILD, SLEEP, ZOMBIE };

typedef struct {
    int16_t pid;
    char name[NAME_SIZE];
    uint8_t state;
    uint32_t reg[5];
    uint32_t stack[STACK_SIZE];
    bool vivant;
    link chain;
    int prio;
} process_t;

void ctx_sw(uint32_t * reg1, uint32_t * reg2);

void ordonnance();

char* mon_nom();

int16_t mon_pid();

bool est_vivant();

/* Gère la terminaison d'un processus, */
/* la valeur retval est passée au processurs père */
/* quand il appelle waitpid. */
void terminaison(/*int retval*/);

int cree_processus(const char * name, void (*code)(void));


#endif