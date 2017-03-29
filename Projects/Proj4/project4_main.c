// Z. Weeden Mar. 29, 2017

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define MIN_ARRIVAL (60)        // 1 minute
#define MAX_ARRIVAL (240)       // 4 minutes
#define MIN_TRANSACTION (30)    // 30 seconds
#define MAX_TRANSACTION (480)   // 8 minutes

int arrivalTimeArray[];
int transactionQueueArray[];

/* This will sleep for a parameter of milliseconds. Parameter should have 
math to convert from world time to system perceived time. (100ms = 60seconds) */
void msSleep(int milliseconds){
    int ms = milliseconds; // length of time to sleep, in miliseconds
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = ms * 1000000L;
    nanosleep(&req, (struct timespec *)NULL);
} 

/* Unused function as of now - used to convert the randomly generated time to a 
value that can be used with our system's scaling. (60seconds => 100ms)*/
int convertToSimulationTime(int seconds){
    int convertedTimeInMS = 0;
    convertedTimeInMS = ((seconds)/600)*1000; // milliseconds expressed as whole number eg. (60/600)*1000 = 100
    return convertedTimeInMS;
}

/* This generates a random number within range of the passed parameters inclusively, while overall 
producing a uniform distribution of generated values. */
int getRandomWithRange(int lower, int upper){
    return lower + (rand() / (RAND_MAX / (upper + 1 - lower))) ;
}

// Pulled from my example of sleep prints see 'thread_dummy.c'
// In each thread make sure to handle initial condition - while queue is empty do nothing
void *tellerThread1(void *vargp){
    sleep(5);
    printf("From teller1 \n");
    return NULL;
}

void *tellerThread2(void *vargp){
    sleep(7);
    printf("From teller2 \n");
    return NULL;
}

void *tellerThread3(void *vargp){
    sleep(1);
    printf("From teller3 \n");
    return NULL;
}

int main(void) {
    int i=0;
    int arrivalTime=0;
    int transactionTime=0;

    // Thread Ids
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    // Creating threads
    pthread_create(&tid1, NULL, tellerThread1, NULL);
    pthread_create(&tid2, NULL, tellerThread2, NULL);
    pthread_create(&tid3, NULL, tellerThread3, NULL);

    // Joining to Pool 
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    while(1){
        arrivalTime = getRandomWithRange(MIN_ARRIVAL, MAX_ARRIVAL);                 // generate random arrival time of customer
        if (arrivalTime + currentTime) < closingTime{                               // pseudo code for checking hours of operations condition
            arrivalTimeArray[i] = arrivalTime;                                      // append the arrival time to the array because it is valid in within hours of operation
            transactionTime = getRandomWithRange(MIN_TRANSACTION, MAX_TRANSACTION); // generate random transaction time of customer
            sleep(arrivalTimeArray[i]);                                             // dont append to queue until after sleep
            transactionQueueArray[i] = transactionTime;                             // queue will be array of customer transaction times
            i++;
        }
        else{
            // Customer cant be seen because it is past hours - bank is closed
            break;
        }
    }
    report(arrivalTimeArray, transactionQueueArray); // parameterized report function here to display metrics
    return EXIT_SUCCESS;
}
