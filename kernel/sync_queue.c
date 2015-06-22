#include "sync_queue.h"
#include "global.h"
#include "process.h"
#include "queue.h"
#include "mem.h"


//TODO : REUSE QUEUES
int pcreate(int count){

    if (count <= 0 || last_queue == (NB_QUEUE - 1)){
        return -1;
    }

    last_queue ++;
    sync_queue_t* queue = &(queue_tab[last_queue]);
    queue->capacity = count;
    queue->length = 0;
    INIT_LIST_HEAD(&(queue->messages));
    INIT_LIST_HEAD(&(queue->waiting_proc));

    return last_queue;
}

int pdelete(int fid) {
    sync_queue_t * to_delete = & queue_tab [fid];

    // TODO check fid for an actual used fid or return -1

    // step one : disalloc all pidcells

    while (! queue_empty(& to_delete -> waiting_proc)) {
        pidcell_t * cell = queue_out(& to_delete -> waiting_proc, pidcell_t, chain);
        // TODO return all waiting processes with a negative value
        mem_free(cell, sizeof(pidcell_t));
    }

    // step two : disalloc all messages

    while (! queue_empty(& to_delete -> messages)) {
        message_t * cell = queue_out(& to_delete -> messages, message_t, chain);
        mem_free(cell, sizeof(message_t));
    }

    // TODO : add index to list of available queue indices for reusability


    return 0;
}