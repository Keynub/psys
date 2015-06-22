#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <inttypes.h>
#include "const.h"
#include "process.h"
#include "list.h"
#include "queue.h"


 process_t process_tab [MAX_NB_PROCESS];
 sync_queue_t queue_tab [NB_QUEUE];
 process_t * cur_proc;
 link process_queue;
 link used_pid;
 uint32_t last_pid;
 uint32_t last_index;
 uint32_t cmpt;
 uint32_t p;
 list l;


#endif
