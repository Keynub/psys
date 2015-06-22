#ifndef _SYNC_QUEUE_H_
#define _SYNC_QUEUE_H_

#include "process.h"
#include "queue.h"

// synchronization queue using queue.h bleeding internally screaming internally

typedef struct {
    // two queues, messages can only contain up to capacity, length contains the current number of messages
    // if messages is full, sending processes go into waiting_proc
    // if messages is empty, receiving processes go into waiting_proc
    link waiting_proc; // of type pidcell_t;
    link messages; // of type message_t;
    int capacity;
    int length;
} sync_queue_t;

typedef struct {
    int message;
    link chain;
    int prio;
} message_t;

int pcreate(int count);

int pdelete(int fid);

int pcount(int fid, int *count);

int preceive(int fid,int *message);

int preset(int fid);

int psend(int fid, int message);

#endif