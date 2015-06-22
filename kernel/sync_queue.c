#include "sync_queue.h"
#include "global.h"

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

    return 0;
}