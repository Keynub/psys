#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <inttypes.h>
#include "const.h"
#include "process.h"
#include "list.h"


 process_t process_tab [MAX_NB_PROCESS];
 uint32_t index_run;
 uint32_t last_pid;
 uint32_t last_index;
 uint32_t cmpt;
 uint32_t p;
 list l;


#endif
