#include "queue.h"

int addQueue(void* node){
    Queue* new = (Queue*)malloc(sizeof(Queue));
    if(!new){
        printf("Not enough memory for new queue\n");
        return -1;
    }
    new->element = node;
    new->link = NULL;

    if(!qHead){
        qHead = new;
    }
    else{
        qTail->link = new;
    }
    qTail = new;

    return 0;
}

void* delQueue(){
    void* tempNode;
    Queue* tempQ;

    if(!qHead){
        printf("Queue empty!\n");
        return NULL;
    }
    if(qHead==qTail) qTail = NULL;
    tempQ = qHead;
    tempNode = qHead->element;
    qHead = qHead->link;
    free(tempQ);

    return tempNode;
}

int emptyQueue(){
    Queue* temp;
    if(!qHead){
        printf("Queue empty!\n");
        return -1;
    }
    while(!qHead){
        temp = qHead;
        qHead = qHead->link;
        free(temp);
    }
    qTail=NULL;

    return 0;
}