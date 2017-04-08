// Z. Weeden Mar. 29, 2017

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "queue.h"

#define MIN_ARRIVAL (60)                                    // 1 minute
#define MAX_ARRIVAL (240)                                   // 4 minutes
#define MIN_TRANSACTION (30)                                // 30 seconds
#define MAX_TRANSACTION (480)                               // 8 minutes
#define SECONDS_OPEN (25200)                                // 7 hours (9am to 4pm)
#define MAX_AMOUNT_OF_CUSTOMERS (SECONDS_OPEN/MIN_ARRIVAL)  // (25200/60) = 420 customers - this assumes that customers come every 60 seconds which is the quickest arrival time for consecutive customers

pthread_mutex_t lock;
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
    double convertedTimeInMS = 0;
    convertedTimeInMS = ((seconds)/600.0)*1000; // milliseconds expressed as whole number eg. (60/600)*1000 = 100
    return (int)convertedTimeInMS;
}

/* This generates a random number within range of the passed parameters inclusively, while overall
producing a uniform distribution of generated values. */
int getRandomWithRange(int lower, int upper){
    return lower + (rand() / (RAND_MAX / (upper + 1 - lower))) ;
}

void* tellerThread1(void *vargp){
    while(1){
        if (bankOpen==1){//bank open
            pthread_mutex_lock( &lock );
            if (Q->size){//there are customers
                currentCustomerTeller1 = front(Q);
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                msSleep(convertToSimulationTime(currentCustomerTeller1));
                printf("Teller1 is taking a customer        (%d)...\n",currentCustomerTeller1);
                printf("Teller1 is done with their customer (%d)...\n",currentCustomerTeller1);
                teller1Customers += 1;
                //pthread_mutex_unlock( &lock );
            }
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
                currentCustomerTeller1 = front(Q);
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                //pthread_mutex_unlock( &lock );
                printf("Teller1 is taking a customer        (%d)...\n",currentCustomerTeller1);
                msSleep(convertToSimulationTime(currentCustomerTeller1));
                printf("Teller1 is done with their customer (%d)...\n",currentCustomerTeller1);
                teller1Customers += 1;
            }
            else{ //bank closed and no customers!
                pthread_mutex_unlock( &lock );
                break;
            }
        }
    }
    return NULL;
}

void* tellerThread2(void *vargp){
    while(1){
        if (bankOpen==1){ //bank open
            pthread_mutex_lock( &lock );
            if (Q->size){ //there are customers
                currentCustomerTeller2 = front(Q);
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                //pthread_mutex_unlock( &lock );
                printf("Teller2 is taking a customer        (%d)...\n",currentCustomerTeller2);
                msSleep(convertToSimulationTime(currentCustomerTeller2));
                printf("Teller2 is done with their customer (%d)...\n",currentCustomerTeller2);
                teller2Customers += 1;
            }
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
                currentCustomerTeller2 = front(Q);
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                //pthread_mutex_unlock( &lock );
                printf("Teller2 is taking a customer        (%d)...\n",currentCustomerTeller2);
                msSleep(convertToSimulationTime(currentCustomerTeller2));
                printf("Teller2 is done with their customer (%d)...\n",currentCustomerTeller2);
                teller2Customers += 1;
            }
            else{ //bank closed and no customers!
                pthread_mutex_unlock( &lock );
                break;
            }
        }
    }
    return NULL;
}

void* tellerThread3(void *vargp){
    while(1){
        if (bankOpen==1){ //bank open
            pthread_mutex_lock( &lock );
            if (Q->size){ //there are customers
                pthread_mutex_unlock( &lock );
                currentCustomerTeller3 = front(Q);
                Dequeue(Q); //make sure this is dequeuing proper customer
                //pthread_mutex_unlock( &lock );
                printf("Teller3 is taking a customer        (%d)...\n",currentCustomerTeller3);
                msSleep(convertToSimulationTime(currentCustomerTeller3));
                printf("Teller3 is done with their customer (%d)...\n",currentCustomerTeller3);
                teller3Customers += 1;
            }
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
                currentCustomerTeller3 = front(Q);
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                //pthread_mutex_unlock( &lock );
                printf("Teller3 is taking a customer        (%d)...\n",currentCustomerTeller3);
                msSleep(convertToSimulationTime(currentCustomerTeller3));
                printf("Teller3 is done with their customer (%d)...\n",currentCustomerTeller3);
                teller3Customers += 1;
            }
            else{ //bank closed and no customers!
                pthread_mutex_unlock( &lock );
                break;
            }
        }
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

/* Thread for handling enqueuing and arrival of customers. Generates two random numbers arrival and transaction time.
nanosleeps for the arrival time generated and then enqueues the trnasaction time. Each element in queue is a number 
represeting that specific customer's tranasaction time.*/
void* queueThread(void *vargp){
    int i = 0;
    int arrivalTime = 0;
    int transactionTime = 0;
    while(1){
        if (bankOpen == 1){                                                                                 // checking hours of operations condition
            //pthread_mutex_lock( &lock );
            arrivalTime = getRandomWithRange(MIN_ARRIVAL, MAX_ARRIVAL);                                     // generate random arrival time of customer
            transactionTime = getRandomWithRange(MIN_TRANSACTION, MAX_TRANSACTION);                         // generate random transaction time of customer
            msSleep(convertToSimulationTime(arrivalTime));                                                  // dont append to queue until after sleep
            Enqueue(Q,transactionTime);
            printf("Size of line: %d\n",Q->size);
            printf("Customer Added to Queue: %d\n\n",rear(Q));
            i++; //# OF CUSTOMER TOTAL
            queueDepth = Q->size;
            if (queueDepth > maxDepth){
                maxDepth = queueDepth;
            }
            //pthread_mutex_unlock( &lock );
        }
        else{ //Arriving customer cant be seen because it is after hours
            break;
        }
    }
    return NULL;
}

int main(void) {
    srand(time(NULL));                          //seed the randomizer with epoch
    Q = createQueue(MAX_AMOUNT_OF_CUSTOMERS);   //create queue instance w/ capacity for maximum queue possible

    // Thread Ids
    pthread_t tid0;
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("Mutex init failed\n");
        return 1;
    }

    printf("\nBank is now open!\n\n");
    bankOpen = 1; // Bank is now Open

    // Creating threads
    pthread_create(&tid1, NULL, tellerThread1, NULL);
    pthread_create(&tid2, NULL, tellerThread2, NULL);
    pthread_create(&tid3, NULL, tellerThread3, NULL);
    pthread_create(&tid0, NULL, queueThread, NULL);

    sleep(42);
    bankOpen = 0; // Bank is now Closed - still need to wait for queue to be empty
    printf("Bank is now closed!\n\n");
    printf("People in queue still: %d\n",Q->size);
    printf("Teller1 served: %d\n", teller1Customers);
    printf("Teller2 served: %d\n", teller2Customers);
    printf("Teller3 served: %d\n", teller3Customers);
    printf("Total customers served: %d\n", (teller1Customers+teller2Customers+teller3Customers));
    printf("Max depth: %d\n",maxDepth);
    printf("\n");
    //report(averageArrival, arrivalTimeArray, transactionQueueArray, maxDepth, totalCustomers); // parameterized report function here to display metrics
    pthread_mutex_destroy(&lock);
    return EXIT_SUCCESS;
}
