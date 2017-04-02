#include <stdio.h>
#include <stdlib.h>

typedef struct Queue Queue;
Queue * createQueue(int maxElements);
void Dequeue(Queue *Q);
int rear(Queue *Q);
int front(Queue *Q);
void Enqueue(Queue *Q,int element);
