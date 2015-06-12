#ifndef PROCESS_H
#define PROCESS_H

#include <inttypes.h>
#include "const.h"

enum {RUNNING, WAITING, BLOCKED_SEM, BLOCKED_IO, BLOCKED_CHILD, SLEEP, ZOMBIE };

typedef struct {
    int16_t pid;
    char name[NAME_SIZE];
    uint8_t state;
    uint32_t reg[5];
    uint32_t stack[STACK_SIZE];
    
} process_t;

void ctx_sw(uint32_t * reg1, uint32_t * reg2);

#endif