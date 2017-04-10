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
#define BILLION 1000000000L

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
int totalTeller1WorkTime = 0;
int totalTeller2WorkTime = 0;
int totalTeller3WorkTime = 0;
int totalTellerWorkTime = 0;
int teller1MaxTransaction = 0;
int teller2MaxTransaction = 0;
int teller3MaxTransaction = 0;

double customersWaitForTeller1 = 0.0;
double customersWaitForTeller2 = 0.0;
double customersWaitForTeller3 = 0.0;
struct timespec startCustomer, stopCustomer;

double teller1Wait;
struct timespec teller1WaitStart, teller1WaitEnd;

double teller2Wait = 0.0;
struct timespec teller2WaitStart, teller2WaitEnd;

double teller3Wait = 0.0;
struct timespec teller3WaitStart, teller3WaitEnd;

double customer1WaitTime;
double maxCustomer1Wait = 0.0;
double customer2WaitTime;
double maxCustomer2Wait = 0.0;
double customer3WaitTime;
double maxCustomer3Wait = 0.0;

double teller1WaitTime;
double maxWaitTeller1 = 0.0;
double teller2WaitTime;
double maxWaitTeller2 = 0.0;
double teller3WaitTime;
double maxWaitTeller3 = 0.0;

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

double msRealToSim(double ms){
    return (ms/100.0)*60.0;
}
void* tellerThread1(void *vargp){
    while(1){
        if (bankOpen==1){//bank open
            clock_gettime( CLOCK_REALTIME, &stopCustomer);
            pthread_mutex_lock( &lock );
            if (Q->size){//there are customers
                currentCustomerTeller1 = front(Q);

                if (stopCustomer.tv_sec + stopCustomer.tv_nsec > startCustomer.tv_sec + startCustomer.tv_nsec && teller1Customers > 0){
                    customer1WaitTime=(( stopCustomer.tv_sec - startCustomer.tv_sec )+ (double)( stopCustomer.tv_nsec - startCustomer.tv_nsec ) / (double)BILLION);
                    customersWaitForTeller1+=customer1WaitTime;
                }

                if (customer1WaitTime>maxCustomer1Wait){
                    maxCustomer1Wait=customer1WaitTime;
                }
                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &teller1WaitEnd);
                if (teller1WaitEnd.tv_sec + teller1WaitEnd.tv_nsec > teller1WaitStart.tv_sec + teller1WaitStart.tv_nsec && teller1Customers > 0){
                    teller1WaitTime=(( teller1WaitEnd.tv_sec - teller1WaitStart.tv_sec )+ (double)( teller1WaitEnd.tv_nsec - teller1WaitStart.tv_nsec ) / (double)BILLION);
                    teller1Wait+=teller1WaitTime;
                }
                if (teller1WaitTime>maxWaitTeller1){
                    maxWaitTeller1=teller1WaitTime;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller1 is taking a customer        (%d)...\n",currentCustomerTeller1);
                msSleep(convertToSimulationTime(currentCustomerTeller1));
                
                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &teller1WaitStart);
                system(0);
                printf("Teller1 is done with their customer (%d)...\n",currentCustomerTeller1);
                if (currentCustomerTeller1 > teller1MaxTransaction){
                    teller1MaxTransaction = currentCustomerTeller1;
                }
                totalTeller1WorkTime+=currentCustomerTeller1;
                teller1Customers += 1;
            }
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            clock_gettime( CLOCK_REALTIME, &stopCustomer);
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
                currentCustomerTeller1 = front(Q);
                
                if (stopCustomer.tv_sec + stopCustomer.tv_nsec > startCustomer.tv_sec + startCustomer.tv_nsec && teller1Customers > 0){
                    customer1WaitTime=(( stopCustomer.tv_sec - startCustomer.tv_sec )+ (double)( stopCustomer.tv_nsec - startCustomer.tv_nsec ) / (double)BILLION);
                    customersWaitForTeller1+=customer1WaitTime;
                }

                if (customer1WaitTime>maxCustomer1Wait){
                    maxCustomer1Wait=customer1WaitTime;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &teller1WaitEnd);
                if (teller1WaitEnd.tv_sec + teller1WaitEnd.tv_nsec > teller1WaitStart.tv_sec + teller1WaitStart.tv_nsec && teller1Customers > 0){
                    teller1WaitTime=(( teller1WaitEnd.tv_sec - teller1WaitStart.tv_sec )+ (double)( teller1WaitEnd.tv_nsec - teller1WaitStart.tv_nsec ) / (double)BILLION);
                    teller1Wait+=teller1WaitTime;
                }
                if (teller1WaitTime>maxWaitTeller1){
                    maxWaitTeller1=teller1WaitTime;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller1 is taking a customer        (%d)...\n",currentCustomerTeller1);
                msSleep(convertToSimulationTime(currentCustomerTeller1));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &teller1WaitStart);
                system(0);
                printf("Teller1 is done with their customer (%d)...\n",currentCustomerTeller1);
                if (currentCustomerTeller1 > teller1MaxTransaction){
                    teller1MaxTransaction = currentCustomerTeller1;
                }
                totalTeller1WorkTime+=currentCustomerTeller1;
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
            clock_gettime( CLOCK_REALTIME, &stopCustomer);
            pthread_mutex_lock( &lock );
            if (Q->size){ //there are customers
                currentCustomerTeller2 = front(Q);
                
                if (stopCustomer.tv_sec + stopCustomer.tv_nsec > startCustomer.tv_sec + startCustomer.tv_nsec && teller2Customers > 0){
                    customer2WaitTime=(( stopCustomer.tv_sec - startCustomer.tv_sec )+ (double)( stopCustomer.tv_nsec - startCustomer.tv_nsec ) / (double)BILLION);
                    customersWaitForTeller2+=customer2WaitTime;
                }

                if (customer2WaitTime>maxCustomer2Wait){
                    maxCustomer2Wait=customer2WaitTime;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &teller2WaitEnd);
                if (teller2WaitEnd.tv_sec + teller2WaitEnd.tv_nsec > teller2WaitStart.tv_sec + teller2WaitStart.tv_nsec && teller2Customers > 0){
                    teller2WaitTime=(( teller2WaitEnd.tv_sec - teller2WaitStart.tv_sec )+ (double)( teller2WaitEnd.tv_nsec - teller2WaitStart.tv_nsec ) / (double)BILLION);
                    teller2Wait+=teller2WaitTime;
                }
                if (teller2WaitTime>maxWaitTeller2){
                    maxWaitTeller2=teller2WaitTime;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller2 is taking a customer        (%d)...\n",currentCustomerTeller2);
                msSleep(convertToSimulationTime(currentCustomerTeller2));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &teller2WaitStart);
                system(0);
                printf("Teller2 is done with their customer (%d)...\n",currentCustomerTeller2);
                if (currentCustomerTeller2 > teller2MaxTransaction){
                    teller2MaxTransaction = currentCustomerTeller2;
                }
                totalTeller2WorkTime+=currentCustomerTeller2;
                teller2Customers += 1;
            }
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            clock_gettime( CLOCK_REALTIME, &stopCustomer);
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
                currentCustomerTeller2 = front(Q);
                
                if (stopCustomer.tv_sec + stopCustomer.tv_nsec > startCustomer.tv_sec + startCustomer.tv_nsec && teller2Customers > 0){
                    customer2WaitTime=(( stopCustomer.tv_sec - startCustomer.tv_sec )+ (double)( stopCustomer.tv_nsec - startCustomer.tv_nsec ) / (double)BILLION);
                    customersWaitForTeller2+=customer2WaitTime;
                }

                if (customer2WaitTime>maxCustomer2Wait){
                    maxCustomer2Wait=customer2WaitTime;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &teller2WaitEnd);
                if (teller2WaitEnd.tv_sec + teller2WaitEnd.tv_nsec > teller2WaitStart.tv_sec + teller2WaitStart.tv_nsec && teller2Customers > 0){
                    teller2WaitTime=(( teller2WaitEnd.tv_sec - teller2WaitStart.tv_sec )+ (double)( teller2WaitEnd.tv_nsec - teller2WaitStart.tv_nsec ) / (double)BILLION);
                    teller2Wait+=teller2WaitTime;
                }
                if (teller2WaitTime>maxWaitTeller2){
                    maxWaitTeller2=teller2WaitTime;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller2 is taking a customer        (%d)...\n",currentCustomerTeller2);
                msSleep(convertToSimulationTime(currentCustomerTeller2));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &teller2WaitStart);
                system(0);
                printf("Teller2 is done with their customer (%d)...\n",currentCustomerTeller2);
                if (currentCustomerTeller2 > teller2MaxTransaction){
                    teller2MaxTransaction = currentCustomerTeller2;
                }
                totalTeller2WorkTime+=currentCustomerTeller2;
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
            clock_gettime( CLOCK_REALTIME, &stopCustomer);
            pthread_mutex_lock( &lock );
            if (Q->size){ //there are customers
                currentCustomerTeller3 = front(Q);
                
                if (stopCustomer.tv_sec + stopCustomer.tv_nsec > startCustomer.tv_sec + startCustomer.tv_nsec && teller3Customers > 0){
                    customer3WaitTime=(( stopCustomer.tv_sec - startCustomer.tv_sec )+ (double)( stopCustomer.tv_nsec - startCustomer.tv_nsec ) / (double)BILLION);
                    customersWaitForTeller3+=customer3WaitTime;
                }

                if (customer3WaitTime>maxCustomer3Wait){
                    maxCustomer3Wait=customer3WaitTime;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &teller3WaitEnd);
                if (teller3WaitEnd.tv_sec + teller3WaitEnd.tv_nsec > teller3WaitStart.tv_sec + teller3WaitStart.tv_nsec && teller3Customers > 0){
                    teller3WaitTime=(( teller3WaitEnd.tv_sec - teller3WaitStart.tv_sec )+ (double)( teller3WaitEnd.tv_nsec - teller3WaitStart.tv_nsec ) / (double)BILLION);
                    teller3Wait+=teller3WaitTime;
                }
                if (teller3WaitTime>maxWaitTeller3){
                    maxWaitTeller3=teller3WaitTime;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller3 is taking a customer        (%d)...\n",currentCustomerTeller3);
                msSleep(convertToSimulationTime(currentCustomerTeller3));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &teller3WaitStart);
                system(0);
                printf("Teller3 is done with their customer (%d)...\n",currentCustomerTeller3);
                if (currentCustomerTeller3 > teller3MaxTransaction){
                    teller3MaxTransaction = currentCustomerTeller3;
                }
                totalTeller3WorkTime+=currentCustomerTeller3;
                teller3Customers += 1;
            }
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            clock_gettime( CLOCK_REALTIME, &stopCustomer);
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
                currentCustomerTeller3 = front(Q);
                
                if (stopCustomer.tv_sec + stopCustomer.tv_nsec > startCustomer.tv_sec + startCustomer.tv_nsec && teller3Customers > 0){
                    customer3WaitTime=(( stopCustomer.tv_sec - startCustomer.tv_sec )+ (double)( stopCustomer.tv_nsec - startCustomer.tv_nsec ) / (double)BILLION);
                    customersWaitForTeller3+=customer3WaitTime;
                }

                if (customer3WaitTime>maxCustomer3Wait){
                    maxCustomer3Wait=customer3WaitTime;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &teller3WaitEnd);
                if (teller3WaitEnd.tv_sec + teller3WaitEnd.tv_nsec > teller3WaitStart.tv_sec + teller3WaitStart.tv_nsec && teller3Customers > 0){
                    teller3WaitTime=(( teller3WaitEnd.tv_sec - teller3WaitStart.tv_sec )+ (double)( teller3WaitEnd.tv_nsec - teller3WaitStart.tv_nsec ) / (double)BILLION);
                    teller3Wait+=teller3WaitTime;
                }
                if (teller3WaitTime>maxWaitTeller3){
                    maxWaitTeller3=teller3WaitTime;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller3 is taking a customer        (%d)...\n",currentCustomerTeller3);
                msSleep(convertToSimulationTime(currentCustomerTeller3));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &teller3WaitStart);
                system(0);
                printf("Teller3 is done with their customer (%d)...\n",currentCustomerTeller3);
                if (currentCustomerTeller3 > teller3MaxTransaction){
                    teller3MaxTransaction = currentCustomerTeller3;
                }
                totalTeller3WorkTime+=currentCustomerTeller3;
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

/* Thread for handling enqueuing and arrival of customers. Generates two random numbers arrival and transaction time.
nanosleeps for the arrival time generated and then enqueues the trnasaction time. Each element in queue is a number 
represeting that specific customer's tranasaction time.*/
void* queueThread(void *vargp){
    int i = 0;
    int arrivalTime = 0;
    int transactionTime = 0;
    while(1){
        if (bankOpen == 1){                                                                                 // checking hours of operations condition
            arrivalTime = getRandomWithRange(MIN_ARRIVAL, MAX_ARRIVAL);                                     // generate random arrival time of customer
            transactionTime = getRandomWithRange(MIN_TRANSACTION, MAX_TRANSACTION);                         // generate random transaction time of customer
            msSleep(convertToSimulationTime(arrivalTime));                                                  // dont append to queue until after sleep
            Enqueue(Q,transactionTime);
            clock_gettime( CLOCK_REALTIME, &startCustomer);
            system(0);
            printf("Size of line: %d\n",Q->size);
            printf("Customer Added to Queue!\n\n");
            i++; //# OF CUSTOMER TOTAL
            queueDepth = Q->size;
            if (queueDepth > maxDepth){
                maxDepth = queueDepth;
            }
        }
        else{ //Arriving customer cant be seen because it is after hours
            break;
        }
    }
    return NULL;
}

int main(void) {
    int printFlag = 0;
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
    
    printf("People in queue still: %d\n",Q->size);
    printf("Bank is now closed!\n\n");

    totalCustomers = (teller1Customers + teller2Customers + teller3Customers);
    totalTellerWorkTime = (totalTeller1WorkTime + totalTeller2WorkTime + totalTeller3WorkTime);
    printf("1.) Total number of customers serviced: %d customers\n", totalCustomers);
    printf("2.) Average time customer spends in queue: %lf seconds\n",msRealToSim((customersWaitForTeller1+customersWaitForTeller2+customersWaitForTeller3)*1000)/totalCustomers); //needs to be checked
    printf("3.) Average time customer spends with teller: %d seconds\n",(totalTellerWorkTime/totalCustomers));
    printf("4.) Average time teller waits for customer: %lf seconds\n",msRealToSim((teller1Wait+teller2Wait+teller3Wait)*1000)/totalCustomers);//needs checking
    maxCustomer1Wait=msRealToSim(maxCustomer1Wait*1000);
    maxCustomer2Wait=msRealToSim(maxCustomer2Wait*1000);
    maxCustomer3Wait=msRealToSim(maxCustomer3Wait*1000);
    if (maxCustomer1Wait>=maxCustomer2Wait && maxCustomer1Wait>=maxCustomer3Wait && printFlag==0){//tellers could have same max but dont care - just the value
        printFlag = 1;
        printf("5.) Maximum customer wait time in queue: %f seconds\n",maxCustomer1Wait);
    }
    if (maxCustomer2Wait>=maxCustomer1Wait && maxCustomer2Wait>=maxCustomer3Wait && printFlag==0){
        printFlag = 1;
        printf("5.) Maximum customer wait time in queue: %f seconds\n",maxCustomer2Wait);
    }
    if (maxCustomer3Wait>=maxCustomer1Wait && maxCustomer3Wait>=maxCustomer2Wait && printFlag==0){
        printf("5.) Maximum customer wait time in queue: %f seconds\n",maxCustomer3Wait);
    }
    printFlag=0;

    printf("%f\n%f\n%f\n",maxWaitTeller1,maxWaitTeller2,maxWaitTeller3);
    maxWaitTeller1=msRealToSim(maxWaitTeller1*1000);
    maxWaitTeller2=msRealToSim(maxWaitTeller2*1000);
    maxWaitTeller3=msRealToSim(maxWaitTeller3*1000);
    if (maxWaitTeller1>=maxWaitTeller2 && maxWaitTeller1>=maxWaitTeller3 && printFlag==0){//tellers could have same max but dont care - just the value
        printFlag = 1;
        printf("6.) Maximum wait time for tellers waiting for customers: %f seconds\n",maxWaitTeller1);
    }
    if (maxWaitTeller2>=maxWaitTeller1 && maxWaitTeller2>=maxWaitTeller3 && printFlag==0){
        printFlag = 1;
        printf("6.) Maximum wait time for tellers waiting for customers: %f seconds\n",maxWaitTeller2);
    }
    if (maxWaitTeller3>=maxWaitTeller1 && maxWaitTeller3>=maxWaitTeller2 && printFlag==0){
        printf("6.) Maximum wait time for tellers waiting for customers: %f seconds\n",maxWaitTeller3);
    }
    printFlag=0;

    if (teller1MaxTransaction>=teller2MaxTransaction && teller1MaxTransaction>=teller3MaxTransaction && printFlag==0){//tellers could have same max but dont care - just the value
        printFlag = 1;
        printf("7.) Maximum transaction time for the tellers: %d seconds\n",teller1MaxTransaction);
    }
    if (teller2MaxTransaction>=teller1MaxTransaction && teller2MaxTransaction>=teller3MaxTransaction && printFlag==0){
        printFlag = 1;
        printf("7.) Maximum transaction time for the tellers: %d seconds\n",teller2MaxTransaction);
    }
    if (teller3MaxTransaction>=teller1MaxTransaction && teller3MaxTransaction>=teller2MaxTransaction && printFlag==0){
        printf("7.) Maximum transaction time for the tellers: %d seconds\n",teller3MaxTransaction);
    }
    printf("8.) Maximum depth of customer queue: %d customers\n\n",maxDepth);
    pthread_mutex_destroy(&lock);
    return EXIT_SUCCESS;
}
