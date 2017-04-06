// Z. Weeden Mar. 29, 2017

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include "queue.h"

#define MIN_ARRIVAL (60)                                    // 1 minute
#define MAX_ARRIVAL (240)                                   // 4 minutes
#define MIN_TRANSACTION (30)                                // 30 seconds
#define MAX_TRANSACTION (480)                               // 8 minutes
#define SECONDS_OPEN (25200)                                // 7 hours (9am to 4pm)
#define MAX_AMOUNT_OF_CUSTOMERS (SECONDS_OPEN/MIN_ARRIVAL)  // (25200/60) = 420 customers - this assumes that customers come every 60 seconds which is the quickest arrival time for consecutive customers

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int arrivalTimeArray[MAX_AMOUNT_OF_CUSTOMERS];
int transactionQueueArray[MAX_AMOUNT_OF_CUSTOMERS];
int totalCustomers = 0;
int queueDepth = 0;
int maxDepth = 0;
int teller1Customers = 0;
int teller2Customers = 0;
int teller3Customers = 0;
int arrivalArrayLength = 0;
double averageTransaction = 0.0;
double averageArrival = 0.0;
int currentCustomerTeller1 = 0;
int currentCustomerTeller2 = 0;
int currentCustomerTeller3 = 0;
int bankOpen = 0;
Queue *Q;

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
    //sleep(5);
    //dequeue from array
    //need while here
    printf("teller1\n");
    while (!bankOpen || !(Q->size)){
        //bank is closed or there is no line
    }
    while (bankOpen || Q->size){
        pthread_mutex_lock( &mutex );
        while(!(Q->size)){
            //bank open but queue empty
        }
        currentCustomerTeller1 = front(Q);
        printf("Teller1 is taking a customer        (%d)...\n",currentCustomerTeller1);
        Dequeue(Q); //make sure this is dequeuing proper customer
        msSleep(convertToSimulationTime(currentCustomerTeller1));
        printf("Teller1 is done with their customer (%d)...\n",currentCustomerTeller1);
        teller1Customers += 1;
        pthread_mutex_unlock( &mutex );
    }
    return NULL;
}

void *tellerThread2(void *vargp){
    //sleep(7);
    //dequeue from array
    //need while here
    printf("teller2\n");
    while (!bankOpen || !(Q->size)){
        //bank is closed or there is no line
    }
    while (bankOpen || Q->size){
        pthread_mutex_lock( &mutex );
        while(!(Q->size)){
            //bank open but queue empty
        }
        currentCustomerTeller2 = front(Q);
        printf("Teller2 is taking a customer        (%d)...\n",currentCustomerTeller2);
        Dequeue(Q); //make sure this is dequeuing proper customer
        msSleep(convertToSimulationTime(currentCustomerTeller2));
        printf("Teller2 is done with their customer (%d)...\n",currentCustomerTeller2);
        teller2Customers += 1;
        pthread_mutex_unlock( &mutex );
    }
    return NULL;
}

void *tellerThread3(void *vargp){
    //sleep(1);
    //dequeue from array
    //need while here
    printf("teller3\n");
    while (!bankOpen || !(Q->size)){
        //bank is closed or there is no line
    }
    while (bankOpen || Q->size){
        pthread_mutex_lock( &mutex );
        while(!(Q->size)){
            //bank open but queue empty
        }
        currentCustomerTeller3 = front(Q);
        printf("Teller3 is taking a customer        (%d)...\n",currentCustomerTeller3);
        Dequeue(Q); //make sure this is dequeuing proper customer
        msSleep(convertToSimulationTime(currentCustomerTeller3));
        printf("Teller3 is done with their customer (%d)...\n",currentCustomerTeller3);
        teller3Customers += 1;
        pthread_mutex_unlock( &mutex );
    }
    return NULL;
}

/* This function takes in an int array and calculates the average of all elements. */
double arrayAverage(int *myArray, int length) {
   int i;
   int sum = 0;
   double average = 0.0;
   for (i = 0; i < length; i++) {
      sum = (sum + myArray[i]);
   }
   average = (double)sum / length;
   return average;
}


void *queueThread(void *vargp){
    int i = 0;
    int arrivalTime = 0;
    int transactionTime = 0;
    printf("queue\n");
    while(1){
        if (bankOpen == 1){                                                                                 // checking hours of operations condition
            pthread_mutex_lock( &mutex );
            arrivalTime = getRandomWithRange(MIN_ARRIVAL, MAX_ARRIVAL);                                     // generate random arrival time of customer
            arrivalTimeArray[i] = arrivalTime;                                                              // append the arrival time to the array because it is valid in within hours of operation
            transactionTime = getRandomWithRange(MIN_TRANSACTION, MAX_TRANSACTION);                         // generate random transaction time of customer
            msSleep(convertToSimulationTime((arrivalTimeArray[i])));                                        // dont append to queue until after sleep
            transactionQueueArray[i] = transactionTime;                                                     // queue will be array of customer transaction times
            Enqueue(Q,transactionTime);
            printf("Size of line: %d\n",Q->size);
            i++; //# OF CUSTOMER TOTAL
            //queueDepth = sizeof(transactionQueueArray)/sizeof(transactionQueueArray[0]);
            queueDepth = Q->size;
            if (queueDepth > maxDepth){
                maxDepth = queueDepth;
            }
            pthread_mutex_unlock( &mutex );
        }
        else{
            // ARRIVING ustomer cant be seen because it is past hours - bank is closed
            // Make sure Q->size==0 - customers in queue can still be seen
            printf("Bank closed\n");
            totalCustomers = teller1Customers + teller2Customers + teller3Customers;
            arrivalArrayLength = sizeof(arrivalTimeArray)/sizeof(arrivalTimeArray[0]);
            averageArrival = arrayAverage(arrivalTimeArray, arrivalArrayLength);
            break;
        }
    }
    return NULL;
}

int main(void) {
    Q = createQueue(MAX_AMOUNT_OF_CUSTOMERS);
    // Thread Ids
    pthread_t tid0;
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    printf("Bank is now open!\n");
    bankOpen = 1; // Bank is now Open

    // Creating threads
    pthread_create(&tid0, NULL, queueThread, NULL);
    pthread_create(&tid1, NULL, tellerThread1, NULL);
    pthread_create(&tid2, NULL, tellerThread2, NULL);
    pthread_create(&tid3, NULL, tellerThread3, NULL);

    // Joining to Pool
    pthread_join(tid0, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    //sleep((SECONDS_OPEN/600)); // (25200/600) = 42 seconds
    sleep(42);
    bankOpen = 0; // Bank is now Closed - still need to wait for queue to be empty
    printf("Bank is now closed!\n");

    //report(averageArrival, arrivalTimeArray, transactionQueueArray, maxDepth, totalCustomers); // parameterized report function here to display metrics
    return EXIT_SUCCESS;
}
