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
int totalCustomers = 0;
int queueDepth = 0;
int maxDepth = 0;
int teller1Customers = 0;
int teller2Customers = 0;
int teller3Customers = 0;
int arrivalArrayLength = 0;
double averageTransaction = 0.0;
double averageArrival = 0.0;

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
    //dequeue from array
    sleep(5);
    printf("From teller1 \n");
    teller1Customers+=1;
    return NULL;
}

void *tellerThread2(void *vargp){
    //dequeue from array
    sleep(7);
    printf("From teller2 \n");
    teller2Customers+=1;
    return NULL;
}

void *tellerThread3(void *vargp){
    //dequeue from array
    sleep(1);
    printf("From teller3 \n");
    teller3Customers+=1;
    return NULL;
}

/* This function takes in an int array and calculates the average of all elements. */
double getAverage(int *myArray, int length) {
   int i;
   int sum = 0;
   double average = 0.0;
   for (i = 0; i < length; i++) {
      sum = (sum + myArray[i]);
   }
   average = (double)sum / length;
   return average;
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
            totalCustomers = teller1Customers + teller2Customers + teller3Customers;
            arrivalArrayLength = sizeof(arrivalTimeArray)/sizeof(arrivalTimeArray[0]);
            averageArrival = arrayAverage(arrivalTimeArray, arrivalArrayLength);
            break;
        }
        queueDepth = sizeof(transactionQueueArray)/sizeof(transactionQueueArray[0]);
        if (queueDepth > maxDepth){
            maxDepth = queueDepth;
        }
    }

    report(averageArrival, arrivalTimeArray, transactionQueueArray, maxDepth, totalCustomers); // parameterized report function here to display metrics
    return EXIT_SUCCESS;
}
