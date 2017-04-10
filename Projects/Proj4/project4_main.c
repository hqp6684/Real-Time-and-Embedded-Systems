// Z. Weeden Mar. 29, 2017

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "queue.h"
#include "report.h"

#define MIN_ARRIVAL (60)                                    // 1 minute
#define MAX_ARRIVAL (240)                                   // 4 minutes
#define MIN_TRANSACTION (30)                                // 30 seconds
#define MAX_TRANSACTION (480)                               // 8 minutes
#define SECONDS_OPEN (25200)                                // 7 hours (9am to 4pm)
#define MAX_AMOUNT_OF_CUSTOMERS (SECONDS_OPEN/MIN_ARRIVAL)  // (25200/60) = 420 customers - this assumes that customers come every 60 seconds which is the quickest arrival time for consecutive customers
#define BILLION (1000000000L)

pthread_mutex_t lock;
int total_customers = 0;
int current_queue_depth = 0;
int max_queue_depth = 0;
int customers_served_by_teller1 = 0;
int customers_served_by_teller2 = 0;
int customers_served_by_teller3 = 0;
int current_teller1_customer = 0;
int current_teller2_customer = 0;
int current_teller3_customer = 0;
int time_teller1_worked = 0;
int time_teller2_worked = 0;
int time_teller3_worked = 0;
int tellers_total_work_time = 0;
int teller1_longest_transaction = 0;
int teller2_longest_transaction = 0;
int teller3_longest_transaction = 0;

double time_customers_wait_for_teller1 = 0.0;
double time_customers_wait_for_teller2 = 0.0;
double time_customers_wait_for_teller3 = 0.0;
struct timespec epoch_customer_starts_waiting_in_queue, epoch_customer_leaves_queue_to_teller;

double time_waited_by_teller1_for_customer = 0.0;
struct timespec epoch_teller1_starts_to_wait_for_customer, epoch_teller1_receives_customer;

double time_waited_by_teller2_for_customer = 0.0;
struct timespec epoch_teller2_starts_to_wait_for_customer, epoch_teller2_receives_customer;

double time_waited_by_teller3_for_customer = 0.0;
struct timespec epoch_teller3_starts_to_wait_for_customer, epoch_teller3_receives_customer;

double current_teller1_customer_wait_time;
double max_time_waited_by_teller1_customer = 0.0;
double current_teller2_customer_wait_time;
double max_time_waited_by_teller2_customer = 0.0;
double current_teller3_customer_wait_time;
double max_time_waited_by_teller3_customer = 0.0;

double time_teller1_waited_for_customer;
double max_time_waited_by_teller1_for_customer = 0.0;
double time_teller2_waited_for_customer;
double max_time_waited_by_teller2_for_customer = 0.0;
double time_teller3_waited_for_customer;
double max_time_waited_by_teller3_for_customer = 0.0;
double total_time_customers_waited_for_all_tellers;
double total_time_waited_by_all_tellers_for_customer;

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
    double converted_time_in_ms = 0;
    converted_time_in_ms = ((seconds)/600.0)*1000; // milliseconds expressed as whole number eg. (60/600)*1000 = 100
    return (int)converted_time_in_ms;
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
            clock_gettime( CLOCK_REALTIME, &epoch_customer_leaves_queue_to_teller);
            pthread_mutex_lock( &lock );
            if (Q->size){//there are customers
                current_teller1_customer = front(Q);

                if (epoch_customer_leaves_queue_to_teller.tv_sec + epoch_customer_leaves_queue_to_teller.tv_nsec > epoch_customer_starts_waiting_in_queue.tv_sec + epoch_customer_starts_waiting_in_queue.tv_nsec && customers_served_by_teller1 > 0){
                    current_teller1_customer_wait_time=(( epoch_customer_leaves_queue_to_teller.tv_sec - epoch_customer_starts_waiting_in_queue.tv_sec )+ (double)( epoch_customer_leaves_queue_to_teller.tv_nsec - epoch_customer_starts_waiting_in_queue.tv_nsec ) / (double)BILLION);
                    time_customers_wait_for_teller1+=current_teller1_customer_wait_time;
                }

                if (current_teller1_customer_wait_time>max_time_waited_by_teller1_customer){
                    max_time_waited_by_teller1_customer=current_teller1_customer_wait_time;
                }
                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &epoch_teller1_receives_customer);
                if (epoch_teller1_receives_customer.tv_sec + epoch_teller1_receives_customer.tv_nsec > epoch_teller1_starts_to_wait_for_customer.tv_sec + epoch_teller1_starts_to_wait_for_customer.tv_nsec && customers_served_by_teller1 > 0){
                    time_teller1_waited_for_customer=(( epoch_teller1_receives_customer.tv_sec - epoch_teller1_starts_to_wait_for_customer.tv_sec )+ (double)( epoch_teller1_receives_customer.tv_nsec - epoch_teller1_starts_to_wait_for_customer.tv_nsec ) / (double)BILLION);
                    time_waited_by_teller1_for_customer+=time_teller1_waited_for_customer;
                }
                if (time_teller1_waited_for_customer>max_time_waited_by_teller1_for_customer){
                    max_time_waited_by_teller1_for_customer=time_teller1_waited_for_customer;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller1 is taking a customer        (%d)...\n",current_teller1_customer);
                msSleep(convertToSimulationTime(current_teller1_customer));
                
                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &epoch_teller1_starts_to_wait_for_customer);
                system(0);
                printf("Teller1 is done with their customer (%d)...\n",current_teller1_customer);
                if (current_teller1_customer > teller1_longest_transaction){
                    teller1_longest_transaction = current_teller1_customer;
                }
                time_teller1_worked+=current_teller1_customer;
                customers_served_by_teller1 += 1;
            }
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            clock_gettime( CLOCK_REALTIME, &epoch_customer_leaves_queue_to_teller);
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
                current_teller1_customer = front(Q);
                
                if (epoch_customer_leaves_queue_to_teller.tv_sec + epoch_customer_leaves_queue_to_teller.tv_nsec > epoch_customer_starts_waiting_in_queue.tv_sec + epoch_customer_starts_waiting_in_queue.tv_nsec && customers_served_by_teller1 > 0){
                    current_teller1_customer_wait_time=(( epoch_customer_leaves_queue_to_teller.tv_sec - epoch_customer_starts_waiting_in_queue.tv_sec )+ (double)( epoch_customer_leaves_queue_to_teller.tv_nsec - epoch_customer_starts_waiting_in_queue.tv_nsec ) / (double)BILLION);
                    time_customers_wait_for_teller1+=current_teller1_customer_wait_time;
                }

                if (current_teller1_customer_wait_time>max_time_waited_by_teller1_customer){
                    max_time_waited_by_teller1_customer=current_teller1_customer_wait_time;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &epoch_teller1_receives_customer);
                if (epoch_teller1_receives_customer.tv_sec + epoch_teller1_receives_customer.tv_nsec > epoch_teller1_starts_to_wait_for_customer.tv_sec + epoch_teller1_starts_to_wait_for_customer.tv_nsec && customers_served_by_teller1 > 0){
                    time_teller1_waited_for_customer=(( epoch_teller1_receives_customer.tv_sec - epoch_teller1_starts_to_wait_for_customer.tv_sec )+ (double)( epoch_teller1_receives_customer.tv_nsec - epoch_teller1_starts_to_wait_for_customer.tv_nsec ) / (double)BILLION);
                    time_waited_by_teller1_for_customer+=time_teller1_waited_for_customer;
                }
                if (time_teller1_waited_for_customer>max_time_waited_by_teller1_for_customer){
                    max_time_waited_by_teller1_for_customer=time_teller1_waited_for_customer;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller1 is taking a customer        (%d)...\n",current_teller1_customer);
                msSleep(convertToSimulationTime(current_teller1_customer));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &epoch_teller1_starts_to_wait_for_customer);
                system(0);
                printf("Teller1 is done with their customer (%d)...\n",current_teller1_customer);
                if (current_teller1_customer > teller1_longest_transaction){
                    teller1_longest_transaction = current_teller1_customer;
                }
                time_teller1_worked+=current_teller1_customer;
                customers_served_by_teller1 += 1;
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
            clock_gettime( CLOCK_REALTIME, &epoch_customer_leaves_queue_to_teller);
            pthread_mutex_lock( &lock );
            if (Q->size){ //there are customers
                current_teller2_customer = front(Q);
                
                if (epoch_customer_leaves_queue_to_teller.tv_sec + epoch_customer_leaves_queue_to_teller.tv_nsec > epoch_customer_starts_waiting_in_queue.tv_sec + epoch_customer_starts_waiting_in_queue.tv_nsec && customers_served_by_teller2 > 0){
                    current_teller2_customer_wait_time=(( epoch_customer_leaves_queue_to_teller.tv_sec - epoch_customer_starts_waiting_in_queue.tv_sec )+ (double)( epoch_customer_leaves_queue_to_teller.tv_nsec - epoch_customer_starts_waiting_in_queue.tv_nsec ) / (double)BILLION);
                    time_customers_wait_for_teller2+=current_teller2_customer_wait_time;
                }

                if (current_teller2_customer_wait_time>max_time_waited_by_teller2_customer){
                    max_time_waited_by_teller2_customer=current_teller2_customer_wait_time;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &epoch_teller2_receives_customer);
                if (epoch_teller2_receives_customer.tv_sec + epoch_teller2_receives_customer.tv_nsec > epoch_teller2_starts_to_wait_for_customer.tv_sec + epoch_teller2_starts_to_wait_for_customer.tv_nsec && customers_served_by_teller2 > 0){
                    time_teller2_waited_for_customer=(( epoch_teller2_receives_customer.tv_sec - epoch_teller2_starts_to_wait_for_customer.tv_sec )+ (double)( epoch_teller2_receives_customer.tv_nsec - epoch_teller2_starts_to_wait_for_customer.tv_nsec ) / (double)BILLION);
                    time_waited_by_teller2_for_customer+=time_teller2_waited_for_customer;
                }
                if (time_teller2_waited_for_customer>max_time_waited_by_teller2_for_customer){
                    max_time_waited_by_teller2_for_customer=time_teller2_waited_for_customer;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller2 is taking a customer        (%d)...\n",current_teller2_customer);
                msSleep(convertToSimulationTime(current_teller2_customer));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &epoch_teller2_starts_to_wait_for_customer);
                system(0);
                printf("Teller2 is done with their customer (%d)...\n",current_teller2_customer);
                if (current_teller2_customer > teller2_longest_transaction){
                    teller2_longest_transaction = current_teller2_customer;
                }
                time_teller2_worked+=current_teller2_customer;
                customers_served_by_teller2 += 1;
            }
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            clock_gettime( CLOCK_REALTIME, &epoch_customer_leaves_queue_to_teller);
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
                current_teller2_customer = front(Q);
                
                if (epoch_customer_leaves_queue_to_teller.tv_sec + epoch_customer_leaves_queue_to_teller.tv_nsec > epoch_customer_starts_waiting_in_queue.tv_sec + epoch_customer_starts_waiting_in_queue.tv_nsec && customers_served_by_teller2 > 0){
                    current_teller2_customer_wait_time=(( epoch_customer_leaves_queue_to_teller.tv_sec - epoch_customer_starts_waiting_in_queue.tv_sec )+ (double)( epoch_customer_leaves_queue_to_teller.tv_nsec - epoch_customer_starts_waiting_in_queue.tv_nsec ) / (double)BILLION);
                    time_customers_wait_for_teller2+=current_teller2_customer_wait_time;
                }

                if (current_teller2_customer_wait_time>max_time_waited_by_teller2_customer){
                    max_time_waited_by_teller2_customer=current_teller2_customer_wait_time;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &epoch_teller2_receives_customer);
                if (epoch_teller2_receives_customer.tv_sec + epoch_teller2_receives_customer.tv_nsec > epoch_teller2_starts_to_wait_for_customer.tv_sec + epoch_teller2_starts_to_wait_for_customer.tv_nsec && customers_served_by_teller2 > 0){
                    time_teller2_waited_for_customer=(( epoch_teller2_receives_customer.tv_sec - epoch_teller2_starts_to_wait_for_customer.tv_sec )+ (double)( epoch_teller2_receives_customer.tv_nsec - epoch_teller2_starts_to_wait_for_customer.tv_nsec ) / (double)BILLION);
                    time_waited_by_teller2_for_customer+=time_teller2_waited_for_customer;
                }
                if (time_teller2_waited_for_customer>max_time_waited_by_teller2_for_customer){
                    max_time_waited_by_teller2_for_customer=time_teller2_waited_for_customer;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller2 is taking a customer        (%d)...\n",current_teller2_customer);
                msSleep(convertToSimulationTime(current_teller2_customer));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &epoch_teller2_starts_to_wait_for_customer);
                system(0);
                printf("Teller2 is done with their customer (%d)...\n",current_teller2_customer);
                if (current_teller2_customer > teller2_longest_transaction){
                    teller2_longest_transaction = current_teller2_customer;
                }
                time_teller2_worked+=current_teller2_customer;
                customers_served_by_teller2 += 1;
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
            clock_gettime( CLOCK_REALTIME, &epoch_customer_leaves_queue_to_teller);
            pthread_mutex_lock( &lock );
            if (Q->size){ //there are customers
                current_teller3_customer = front(Q);
                
                if (epoch_customer_leaves_queue_to_teller.tv_sec + epoch_customer_leaves_queue_to_teller.tv_nsec > epoch_customer_starts_waiting_in_queue.tv_sec + epoch_customer_starts_waiting_in_queue.tv_nsec && customers_served_by_teller3 > 0){
                    current_teller3_customer_wait_time=(( epoch_customer_leaves_queue_to_teller.tv_sec - epoch_customer_starts_waiting_in_queue.tv_sec )+ (double)( epoch_customer_leaves_queue_to_teller.tv_nsec - epoch_customer_starts_waiting_in_queue.tv_nsec ) / (double)BILLION);
                    time_customers_wait_for_teller3+=current_teller3_customer_wait_time;
                }

                if (current_teller3_customer_wait_time>max_time_waited_by_teller3_customer){
                    max_time_waited_by_teller3_customer=current_teller3_customer_wait_time;
                }

                //END OF WAITING FOR CUSTOMER
                clock_gettime( CLOCK_REALTIME, &epoch_teller3_receives_customer);
                if (epoch_teller3_receives_customer.tv_sec + epoch_teller3_receives_customer.tv_nsec > epoch_teller3_starts_to_wait_for_customer.tv_sec + epoch_teller3_starts_to_wait_for_customer.tv_nsec && customers_served_by_teller3 > 0){
                    time_teller3_waited_for_customer=(( epoch_teller3_receives_customer.tv_sec - epoch_teller3_starts_to_wait_for_customer.tv_sec )+ (double)( epoch_teller3_receives_customer.tv_nsec - epoch_teller3_starts_to_wait_for_customer.tv_nsec ) / (double)BILLION);
                    time_waited_by_teller3_for_customer+=time_teller3_waited_for_customer;
                }
                if (time_teller3_waited_for_customer>max_time_waited_by_teller3_for_customer){
                    max_time_waited_by_teller3_for_customer=time_teller3_waited_for_customer;
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q); //make sure this is dequeuing proper customer
                printf("Teller3 is taking a customer        (%d)...\n",current_teller3_customer);
                msSleep(convertToSimulationTime(current_teller3_customer));

                //STARTING TO WAIT FOR NEXT CUSTOMER HERE
                clock_gettime( CLOCK_REALTIME, &epoch_teller3_starts_to_wait_for_customer);
                system(0);
                printf("Teller3 is done with their customer (%d)...\n",current_teller3_customer);
                if (current_teller3_customer > teller3_longest_transaction){
                    teller3_longest_transaction = current_teller3_customer;
                }
                time_teller3_worked+=current_teller3_customer;
                customers_served_by_teller3 += 1;
            }
            else{                                                                                       // Bank is open but no customers!
                pthread_mutex_unlock( &lock );
            }
        }
        else{                                                                                           // Bank is now closed
            clock_gettime( CLOCK_REALTIME, &epoch_customer_leaves_queue_to_teller);                     // Fetch current time - customer leaving queue to teller3
            pthread_mutex_lock( &lock );
            if (Q->size){                                                                               // But there are still customers
                current_teller3_customer = front(Q);                                                    // Current customer for teller3 is the next one in the queue
                
                if (epoch_customer_leaves_queue_to_teller.tv_sec + epoch_customer_leaves_queue_to_teller.tv_nsec > epoch_customer_starts_waiting_in_queue.tv_sec + epoch_customer_starts_waiting_in_queue.tv_nsec && customers_served_by_teller3 > 0){
                    current_teller3_customer_wait_time=(( epoch_customer_leaves_queue_to_teller.tv_sec - epoch_customer_starts_waiting_in_queue.tv_sec )+ (double)( epoch_customer_leaves_queue_to_teller.tv_nsec - epoch_customer_starts_waiting_in_queue.tv_nsec ) / (double)BILLION);
                    time_customers_wait_for_teller3+=current_teller3_customer_wait_time;                // Update time waited by customers to be seen by teller3
                }

                if (current_teller3_customer_wait_time>max_time_waited_by_teller3_customer){
                    max_time_waited_by_teller3_customer=current_teller3_customer_wait_time;             // Update max time waited by customer to be seen by teller3 if needed
                }

                clock_gettime( CLOCK_REALTIME, &epoch_teller3_receives_customer);                       // End wait for customer - take from queue
                if (epoch_teller3_receives_customer.tv_sec + epoch_teller3_receives_customer.tv_nsec > epoch_teller3_starts_to_wait_for_customer.tv_sec + epoch_teller3_starts_to_wait_for_customer.tv_nsec && customers_served_by_teller3 > 0){
                    time_teller3_waited_for_customer=(( epoch_teller3_receives_customer.tv_sec - epoch_teller3_starts_to_wait_for_customer.tv_sec )+ (double)( epoch_teller3_receives_customer.tv_nsec - epoch_teller3_starts_to_wait_for_customer.tv_nsec ) / (double)BILLION);
                    time_waited_by_teller3_for_customer+=time_teller3_waited_for_customer;              // Update time waited by teller3 for customer
                }
                if (time_teller3_waited_for_customer>max_time_waited_by_teller3_for_customer){
                    max_time_waited_by_teller3_for_customer=time_teller3_waited_for_customer;           // Update max time of teller3 waiting for customer if needed
                }
                pthread_mutex_unlock( &lock );
                Dequeue(Q);                                                                             // Next customer in queue taken to conduct business
                printf("Teller3 is taking a customer        (%d)...\n",current_teller3_customer);
                msSleep(convertToSimulationTime(current_teller3_customer));                             // Sleep for current customer's transaction time to simulate business
                clock_gettime( CLOCK_REALTIME, &epoch_teller3_starts_to_wait_for_customer);             // Start to wait for next customer
                system(0);
                printf("Teller3 is done with their customer (%d)...\n",current_teller3_customer);
                if (current_teller3_customer > teller3_longest_transaction){
                    teller3_longest_transaction = current_teller3_customer;                             // Update the max transaction time completed by teller3 if needed
                }
                time_teller3_worked+=current_teller3_customer;                                          // Increment the time worked by teller3
                customers_served_by_teller3 += 1;                                                       // Increment the number of customers teller3 has processed
            }
            else{                                                                                       // Bank closed and no customers!
                pthread_mutex_unlock( &lock );
                break;
            }
        }
    }
    return NULL;
}

/* Thread for handling enqueuing and arrival of customers. Generates two random numbers arrival and transaction time.
   nanosleeps for the arrival time generated and then enqueues the trnasaction time. Each element in queue is a number 
   represeting that specific customer's transaction time.*/
void* queueThread(void *vargp){
    int arrivalTime = 0;
    int transactionTime = 0;
    while(1){
        if (bankOpen == 1){                                                             // Check if bank open
            arrivalTime = getRandomWithRange(MIN_ARRIVAL, MAX_ARRIVAL);                 // Generate random arrival time of customer
            transactionTime = getRandomWithRange(MIN_TRANSACTION, MAX_TRANSACTION);     // Generate random transaction time of customer
            msSleep(convertToSimulationTime(arrivalTime));                              // Don't append to queue until after sleep - simulates wait for customer arrival
            Enqueue(Q,transactionTime);                                                 // Enqueue the transaction time of the customer to the queue (this works two fold as the customer's pseudo-id and their transaction time)
            clock_gettime( CLOCK_REALTIME, &epoch_customer_starts_waiting_in_queue);    // Fetch current time - used to calculate when the customer was enqueue and how long they'll wait to be seen
            system(0);
            printf("Size of line: %d\n",Q->size);
            printf("Customer Added to Queue!\n\n");
            current_queue_depth = Q->size;                                              // Check size of queue
            if (current_queue_depth > max_queue_depth){                                 // Update max queue size with current if needed
                max_queue_depth = current_queue_depth;
            }
        }
        else{                                                                           // Arriving customer can't be seen because it is after hours.
            break;
        }
    }
    return NULL;
}

/* Main of the program. Initializes/creates queue, mutex and threads and defines the hours of operation with sleep function. Does some basic calulcations for required
   metrics and calls a function for further minor calculations and printing. */
int main(void) {
    srand(time(NULL));                          // Seed the randomizer with epoch
    Q = createQueue(MAX_AMOUNT_OF_CUSTOMERS);   // Create queue instance w/ maximum capacity possible

    // Thread Ids
    pthread_t tid0;
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    if (pthread_mutex_init(&lock, NULL) != 0){  // Check mutex initialization
        printf("Mutex init failed\n");
        return 1;
    }

    printf("\nBank is now open!\n\n");
    bankOpen = 1;                               // Bank is now Open

    // Creating threads
    pthread_create(&tid1, NULL, tellerThread1, NULL);
    pthread_create(&tid2, NULL, tellerThread2, NULL);
    pthread_create(&tid3, NULL, tellerThread3, NULL);
    pthread_create(&tid0, NULL, queueThread, NULL);

    sleep(42);                                  // Simulate 9AM-4PM (7 hours) business day. (7 hours*60 minutes) = 420 minutes*.1 second = 42
    bankOpen = 0;                               // Bank is now Closed
    
    printf("People in queue still: %d\n",Q->size);
    printf("Bank is now closed!\n\n");

    total_customers = (customers_served_by_teller1 + customers_served_by_teller2 + customers_served_by_teller3);
    tellers_total_work_time = (time_teller1_worked + time_teller2_worked + time_teller3_worked);
    max_time_waited_by_teller1_customer=msRealToSim(max_time_waited_by_teller1_customer*1000);
    max_time_waited_by_teller2_customer=msRealToSim(max_time_waited_by_teller2_customer*1000);
    max_time_waited_by_teller3_customer=msRealToSim(max_time_waited_by_teller3_customer*1000);
    max_time_waited_by_teller1_for_customer=msRealToSim(max_time_waited_by_teller1_for_customer*1000);
    max_time_waited_by_teller2_for_customer=msRealToSim(max_time_waited_by_teller2_for_customer*1000);
    max_time_waited_by_teller3_for_customer=msRealToSim(max_time_waited_by_teller3_for_customer*1000);
    total_time_customers_waited_for_all_tellers=msRealToSim((time_customers_wait_for_teller1+time_customers_wait_for_teller2+time_customers_wait_for_teller3)*1000);
    total_time_waited_by_all_tellers_for_customer=msRealToSim((time_waited_by_teller1_for_customer+time_waited_by_teller2_for_customer+time_waited_by_teller3_for_customer)*1000);
    
    report_metrics(total_customers, total_time_customers_waited_for_all_tellers, tellers_total_work_time, total_time_waited_by_all_tellers_for_customer, max_time_waited_by_teller1_customer, 
        max_time_waited_by_teller2_customer, max_time_waited_by_teller3_customer, max_time_waited_by_teller1_for_customer, max_time_waited_by_teller2_for_customer, max_time_waited_by_teller3_for_customer, 
        teller1_longest_transaction, teller2_longest_transaction, teller3_longest_transaction, max_queue_depth);
    
    pthread_mutex_destroy(&lock);
    return EXIT_SUCCESS;
}
