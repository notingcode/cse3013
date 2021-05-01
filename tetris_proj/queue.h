#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct _Queue{
    void* element;
    struct _Queue *link;
} Queue;

int qNum;
Queue *qHead, *qTail;

int addQueue(void* node);

void* delQueue();

int emptyQueue();

#endif