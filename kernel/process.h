#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <inttypes.h>

#include <stdbool.h>

#include "const.h"
#include "queue.h"

enum {RUNNING, WAITING, BLOCKED_SEM, BLOCKED_IO, BLOCKED_CHILD, SLEEP, ZOMBIE };

typedef struct {
    int retval; // /!\ CECI DOIT RESTER LE PREMIER ATTRIBUT SINON ON VA AVOIR DES PROBLÈMES EN ASSEMBLEUR !
    int16_t pid;
    char name[NAME_SIZE];
    uint8_t state;
    uint32_t reg[5];
    uint32_t stack[512];
    bool vivant;
    link chain;
    int prio;
    int16_t pid_pere;
    link enfants;
} process_t;

typedef struct {
   int16_t pid;
    link chain;
    int prio;
} pidcell_t;

void ctx_sw(uint32_t * reg1, uint32_t * reg2);

void ordonnance();

char* mon_nom();

int16_t mon_pid();

int16_t mon_papa();

process_t * ptr_mon_papa();

bool est_vivant();

/* Gère la terminaison d'un processus, */
/* la valeur retval est passée au processurs père */
/* quand il appelle waitpid. */
void terminaison();

void exitlol();

void exit(int retval);

int waitpid(int pid, int *retvalp);

void wait_clock(unsigned long clock);

int getprio(int pid);

int chprio(int pid, int newprio);

int start(int (*pt_func)(void*), unsigned long ssize, int prio, const char *name, void *arg);

int kill(int pid);


#endif
