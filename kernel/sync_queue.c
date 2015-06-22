#include "sync_queue.h"
#include "global.h"

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