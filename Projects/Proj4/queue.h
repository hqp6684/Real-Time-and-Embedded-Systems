/*
 * queue.h
 *
 *  Created on: Apr 2, 2017
 *      Author: zweeden
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdlib.h>

/*Queue has five properties. capacity stands for the maximum number of elements Queue can hold.
  Size stands for the current size of the Queue and elements is the array of elements. front is the
 index of first element (the index at which we remove the element) and rear is the index of last element
 (the index at which we insert the element) */
typedef struct Queue{
    int capacity;
    int size;
    int front;
    int rear;
    int *elements;
}Queue;
Queue * createQueue(int maxElements);
void Dequeue(Queue *Q);
int rear(Queue *Q);
int front(Queue *Q);
void Enqueue(Queue *Q,int element);

#endif /* QUEUE_H_ */
