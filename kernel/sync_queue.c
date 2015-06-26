#include <stdio.h>
#include "sync_queue.h"
#include "global.h"
#include "process.h"
#include "queue.h"
#include "mem.h"
#include "../shared/queue.h"


//TODO : REUSE QUEUES
int pcreate(int count){
    if (count <= 0 || last_queue == (NB_QUEUE - 1)){
        return -1;
    }

    int tmp = last_queue ++;
    sync_queue_t* queue = &(queue_tab[tmp]);
    queue->capacity = count;
    queue->length = 0;
    queue->alive = true;
    INIT_LIST_HEAD(&(queue->messages));
    INIT_LIST_HEAD(&(queue->waiting_proc));

    return tmp;
}

int pdelete(int fid) {
    sync_queue_t * to_delete = & queue_tab [fid];

    // TODO check fid for an actual used fid or return -1
    /*if (queue_tab[fid].length != 0 || queue_tab[fid].waiting_proc != NULL){
        code ci dessous ?
    }
    else{
        return -1;
    }*/

    // step one : disalloc all pidcells

    while (! queue_empty(& to_delete -> waiting_proc)) {
        wproc_t * cell = queue_out(& to_delete -> waiting_proc, wproc_t, chain);
        // TODO return all waiting processes with a negative value
        //exit(-1);

        process_tab[cell -> pid].state = WAITING;
        mem_free(cell, sizeof(wproc_t));
    }

    // step two : disalloc all messages

    while (! queue_empty(& to_delete -> messages)) {
        message_t * cell = queue_out(& to_delete -> messages, message_t, chain);
        mem_free(cell, sizeof(message_t));
    }
    to_delete->alive = false;

    // TODO : add index to list of available queue indices for reusability
    // creer tableau à part regroupant les files réutilisables ?

    return 0;
}


int pcount(int fid, int *count){
    if(fid < 0 || fid > NB_QUEUE - 1 || ! queue_tab[fid].alive){
        return -1;
    }
    if(queue_tab[fid].length <= 0){
        wproc_t * ptr_elem;
        int numb_processes = 0;
        queue_for_each(ptr_elem, &(queue_tab[fid].waiting_proc), wproc_t, chain) {
            numb_processes++;
        }
        *count = -numb_processes;
    }
    else{
        //Number of processes
        wproc_t * ptr_proc;
        int numb_processes = 0;
        queue_for_each(ptr_proc, &(queue_tab[fid].waiting_proc), wproc_t, chain) {
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
    if(fid < 0 || fid > NB_QUEUE - 1 || ! queue_tab[fid].alive){
        return -1;
    }

    //Deletes messages
        message_t * ptr_msg;
        queue_for_each(ptr_msg, &(queue_tab[fid].messages), message_t, chain) {
            queue_del(ptr_msg, chain);
        }

    //Activates waiting processes
    wproc_t * ptr_proc;
        queue_for_each(ptr_proc, &(queue_tab[fid].waiting_proc), wproc_t, chain) {
            process_tab[ptr_proc->pid].state = WAITING;
             queue_del(ptr_proc, chain);
        }
    return 0;
}

int psend(int fid, int message) {
    if(fid < 0 || fid > NB_QUEUE - 1 || ! queue_tab[fid].alive){
        return -1;
    }

    sync_queue_t * to_send = & queue_tab [fid];

    to_send -> length ++;

    // first case : full

    if(to_send -> msgs < to_send -> capacity) {
        // slot left : just drop a message

        message_t * msg = mem_alloc(sizeof(message_t));
        msg->message = message;
        msg->prio = 1;
        INIT_LINK(&msg->chain);
        queue_add(msg, &to_send->messages, message_t, chain, prio);
        to_send -> msgs ++;
        if(to_send -> length < to_send -> msgs) {
            // receivers waiting
            wproc_t *cell = queue_out(&to_send->waiting_proc, wproc_t, chain);

            message_t *msg = queue_out(&to_send->messages, message_t, chain);
            *(cell -> message) = msg->message;
            mem_free(msg, sizeof(message_t));
            to_send->msgs--;

            process_tab[cell->pid].state = WAITING;
            queue_add(&process_tab[cell->pid], &process_queue, process_t, chain, prio);
            bool ordo = cell->prio > getprio(getpid());
            mem_free(cell, sizeof(wproc_t));
            if (ordo) {
                ordonnance();
            }
        }
        return 0;
    } else if(to_send -> length < to_send -> capacity) {
        // no messages but receivers !
        wproc_t *cell = queue_out(&to_send->waiting_proc, wproc_t, chain);
        *(cell->message) = message;
        process_tab[cell->pid].state = WAITING;
        queue_add(&process_tab[cell->pid], &process_queue, process_t, chain, prio);
        bool ordo = cell->prio > getprio(getpid());
        mem_free(cell, sizeof(wproc_t));
        if (ordo) {
            ordonnance();
        }
        return 0;
    } else {
        // fall asleep
        wproc_t * me = mem_alloc(sizeof(wproc_t));
        me->pid = mon_pid();
        me->prio = getprio(getpid());
        me->message = &message;
        INIT_LINK(&me->chain);
        queue_add(me, &to_send->waiting_proc, wproc_t, chain, prio);

        cur_proc -> state = BLOCKED_IO;
        ordonnance();
        return 0;
    }


}

int preceive(int fid, int * message) {

    if(fid < 0 || fid > NB_QUEUE - 1 || ! queue_tab[fid].alive){
        return -1;
    }



    sync_queue_t * to_receive = & queue_tab [fid];
    to_receive -> length --;

    // 3 cases : messages available, senders available, nothing available

    if(to_receive -> msgs > 0) {
        // messages in queue
        message_t *msg = queue_out(&to_receive->messages, message_t, chain);
        if (message != 0) {
            *message = msg->message;
        }
        mem_free(msg, sizeof(message_t));
        to_receive->msgs--;
        if(to_receive -> length > to_receive -> msgs) {
            message_t * msg = mem_alloc(sizeof(message_t));
            msg->prio = 1;
            INIT_LINK(&msg->chain);

            wproc_t *cell = queue_out(&to_receive->waiting_proc, wproc_t, chain);
            msg->message = *(cell->message);
            process_tab[cell->pid].state = WAITING;
            queue_add(&process_tab[cell->pid], &process_queue, process_t, chain, prio);
            bool ordo = cell->prio > getprio(getpid());
            mem_free(cell, sizeof(wproc_t));

            queue_add(msg, &to_receive->messages, message_t, chain, prio);
            to_receive -> msgs ++;
            if (ordo) {
                ordonnance();
            }
        }
        return 0;
    } else if(to_receive -> length > 0) {
        // no messages but senders !
        wproc_t *cell = queue_out(&to_receive->waiting_proc, wproc_t, chain);
        if (message != 0) {
            *message = *(cell->message);
        }
        process_tab[cell->pid].state = WAITING;
        queue_add(&process_tab[cell->pid], &process_queue, process_t, chain, prio);
        bool ordo = cell->prio > getprio(getpid());
        mem_free(cell, sizeof(wproc_t));
        if (ordo) {
            ordonnance();
        }
        return 0;
    } else  {
        // fall asleep

        wproc_t * me = mem_alloc(sizeof(wproc_t));
        me->pid = mon_pid();
        me->prio = getprio(getpid());
        me->message = message;
        INIT_LINK(&me->chain);
        queue_add(me, &to_receive->waiting_proc, wproc_t, chain, prio);

        cur_proc -> state = BLOCKED_IO;
        ordonnance();
        // TODO check that queue wasn't deleted / reseted
        return 0;
    }
}

int pprio (int fid, int pid, int prio) {

    if(fid < 0 || fid > NB_QUEUE - 1 || ! queue_tab[fid].alive){
        return -1;
    }
    wproc_t * ptr_elem;
    queue_for_each(ptr_elem, &queue_tab[fid].waiting_proc, wproc_t, chain) {
        if(ptr_elem->pid == pid) {
            ptr_elem -> prio = prio;
            queue_del(ptr_elem, chain);
            queue_add(ptr_elem, &queue_tab[fid].waiting_proc, wproc_t, chain, prio);
            return 0;
        }
    }
    return -1;
};
