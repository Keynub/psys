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
        process_tab[cell -> pid].state = WAITING;
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

int psend(int fid, int message) {
    sync_queue_t * to_send = & queue_tab [fid];

    // TODO check fid for an actual used fid or return -1

    to_send -> length ++;

    // first case : full

    if(to_send -> length > to_send -> capacity) {
        // fall asleep

        pidcell_t * me = mem_alloc(sizeof(pidcell_t));
        me->pid = mon_pid();
        me->prio = 1;
        INIT_LINK(&me->chain);
        queue_add(me, &to_send->waiting_proc, pidcell_t, chain, prio);

        cur_proc -> state = BLOCKED_IO;
        ordonnance();
        // after waking up next time
        // TODO check that file wasn't deleted / reseted
        message_t * msg = mem_alloc(sizeof(message_t));
        msg->message = message;
        msg->prio = 1;
        INIT_LINK(&msg->chain);
        queue_add(msg, &to_send->messages, message_t, chain, prio);

        return 0;
    } else if(to_send -> length <= 0) {
        // there are processes waiting to receive

        // drop a message anyway

        message_t * msg = mem_alloc(sizeof(message_t));
        msg->message = message;
        msg->prio = 1;
        INIT_LINK(&msg->chain);
        queue_add(msg, &to_send->messages, message_t, chain, prio);

        // wake one up so that it can consume the message
        pidcell_t * cell = queue_out(& to_send -> waiting_proc, pidcell_t, chain);
        process_tab[cell -> pid].state = WAITING;
        mem_free(cell, sizeof(pidcell_t));

        return 0;
    } else {
        // place left & no processes waiting : just drop a message

        message_t * msg = mem_alloc(sizeof(message_t));
        msg->message = message;
        msg->prio = 1;
        INIT_LINK(&msg->chain);
        queue_add(msg, &to_send->messages, message_t, chain, prio);

        return 0;
    }


}

int preceive(int fid, int * message) {
    sync_queue_t * to_receive = & queue_tab [fid];
    // TODO check fid etc.

    to_receive -> length --;

    // 3 cases : senders waiting, messages empty, or base case.

    if(to_receive -> length >= to_receive -> capacity) {
        // we pick up a message and wake up a single sender

        message_t * msg = queue_out(&to_receive -> messages, message_t, chain);
        * message = msg -> message;
        mem_free(msg, sizeof(message_t));

        // free space, wake up sleepyhead

        pidcell_t * cell = queue_out(& to_receive -> waiting_proc, pidcell_t, chain);
        process_tab[cell -> pid].state = WAITING;
        mem_free(cell, sizeof(pidcell_t));

        return 0;

    } else if(to_receive -> length < 0) {
        // we are trying to pick up a message where there are none
        // fall asleep

        pidcell_t * me = mem_alloc(sizeof(pidcell_t));
        me->pid = mon_pid();
        me->prio = 1;
        INIT_LINK(&me->chain);
        queue_add(me, &to_receive->waiting_proc, pidcell_t, chain, prio);

        cur_proc -> state = BLOCKED_IO;
        ordonnance();

        // after waking up next time
        // TODO check that queue wasn't deleted / reseted

        message_t * msg = queue_out(&to_receive -> messages, message_t, chain);
        * message = msg -> message;
        mem_free(msg, sizeof(message_t));

        return 0;
    } else {
        // just pick up a message

        message_t * msg = queue_out(&to_receive -> messages, message_t, chain);
        * message = msg -> message;
        mem_free(msg, sizeof(message_t));

        return 0;
    }
}