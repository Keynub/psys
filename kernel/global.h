#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <inttypes.h>
#include "const.h"
#include "process.h"


 process_t process_tab [MAX_NB_PROCESS];
 uint32_t index_run;
 uint32_t last_pid;
 uint32_t last_index;
 uint32_t cmpt = 0;
 uint32_t p = CLOCKFREQ/SCHEDFREQ;

#endif
