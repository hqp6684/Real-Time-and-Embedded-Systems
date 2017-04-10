// Z. Weeden Mar. 29, 2017

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <pthread.h>

pthread_mutex_t lock;

/* createQueue function takes argument the maximum number of elements the Queue can hold, creates
   a Queue according to it and returns a pointer to the Queue. */
Queue * createQueue(int maxElements){
    /* Create a Queue */
    Queue *Q;
    Q = (Queue *)malloc(sizeof(Queue));
    /* Initialize its properties */
    Q->elements = (int *)malloc(sizeof(int)*maxElements);
    Q->size = 0;
    Q->capacity = maxElements;
    Q->front = 0;
    Q->rear = -1;
    /* Return the pointer */
    return Q;
}

/* Moves the head of the passed queue to the next element. This simulates the dequeue of 
   the first element.*/
void Dequeue(Queue *Q){
    /* If Queue size is zero then it is empty. So we cannot pop */
    pthread_mutex_lock( &lock );
    if(Q->size==0){ //Queue is Empty
        return;
    }
    /* Removing an element is equivalent to incrementing index of front by one */
    else{
        Q->size--;
        Q->front++;
        /* As we fill elements in circular fashion */
        if(Q->front==Q->capacity){
            Q->front=0;
        }
    }
    pthread_mutex_unlock( &lock );
    return;
}

/* Returns the last element of the passed queue. */
int rear(Queue *Q){
    if(Q->size==0){ 
        //Queue is Empty
    }
    /* Return the element which is at the rear*/
    else{
        return Q->elements[Q->rear];
    }
}

/* Returns the front of the passed queue. */
int front(Queue *Q){
    if(Q->size==0){
        //Queue is Empty
    }
    /* Return the element which is at the front*/
    else{
        return Q->elements[Q->front];
    }
}

/* Takes a passed queue and element and appends the element to the end of that particular 
   queue instance. */
void Enqueue(Queue *Q,int element){
    pthread_mutex_lock( &lock );
    /* If the Queue is full, we cannot push an element into it as there is no space for it.*/
    if(Q->size == Q->capacity){
        //Queue is Full
    }
    else{
        Q->size++;
        Q->rear = Q->rear + 1;
        /* As we fill the queue in circular fashion */
        if(Q->rear == Q->capacity){
            Q->rear = 0;
        }
        /* Insert the element in its rear side */
        Q->elements[Q->rear] = element;
    }
    pthread_mutex_unlock( &lock );
    return;
}
