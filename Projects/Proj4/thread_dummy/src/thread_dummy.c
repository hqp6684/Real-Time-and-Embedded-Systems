#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int j = 0;

// A normal C function that is executed as a thread when its name
// is specified in pthread_create()
void *thread1(void *vargp)
{
    while(1){
        if (j == 1){
            break;
        }
    }
    printf("From thread1 \n");
    return NULL;
}

void *thread2(void *vargp)
{
    sleep(7);
    printf("From thread2 \n");
    j=1;
    return NULL;
}

void *thread3(void *vargp)
{
    sleep(1);
    printf("From thread3 \n");
    return NULL;
}
int main()
{
    //Thread Ids
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    printf("Before Threads\n");

    pthread_create(&tid1, NULL, thread1, NULL);
    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_create(&tid3, NULL, thread3, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    printf("After Threads\n");
    exit(0);
}
