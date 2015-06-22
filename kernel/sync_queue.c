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

    int tmp = last_queue ++;
    sync_queue_t* queue = &(queue_tab[tmp]);
    queue->capacity = count;
    queue->length = 0;
    INIT_LIST_HEAD(&(queue->messages));
    INIT_LIST_HEAD(&(queue->waiting_proc));

    return tmp;
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

int pcount(int fid, int *count){
    if(fid < 0 || fid > NB_QUEUE - 1){
        return -1;
    }
    if(queue_tab[fid].length == 0){
        pidcell_t * ptr_elem;
        int numb_processes = 0;
        queue_for_each(ptr_elem, &(queue_tab[fid].waiting_proc), pidcell_t, chain) {
            numb_processes++;
        }
        *count = numb_processes;
    }
    else{
        //Number of processes
        pidcell_t * ptr_proc;
        int numb_processes = 0;
        queue_for_each(ptr_proc, &(queue_tab[fid].waiting_proc), pidcell_t, chain) {
            numb_processes++;
        }
        int tmp1 = numb_processes;

        //Number of messages
        message_t * ptr_msg;
        int numb_msgs = 0;
        queue_for_each(ptr_msg, &(queue_tab[fid].messages), message_t, chain) {
            numb_msgs++;
        }
        int tmp2 = numb_msgs;
    *count = tmp1 + tmp2;
    }
    return 0;
}

int preset(int fid){
    if(fid < 0 || fid > NB_QUEUE - 1){
        return -1;
    }

    //Deletes messages
        message_t * ptr_msg;
        queue_for_each(ptr_msg, &(queue_tab[fid].messages), message_t, chain) {
            queue_del(ptr_msg, chain);
        }

    //Activates waiting processes
    pidcell_t * ptr_proc;
        queue_for_each(ptr_proc, &(queue_tab[fid].waiting_proc), pidcell_t, chain) {
            process_tab[ptr_proc->pid].state = WAITING;
             queue_del(ptr_proc, chain);
        }
    return 0;
}