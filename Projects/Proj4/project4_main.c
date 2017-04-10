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

double teller1Wait; //time_waited_by_teller1_for_customer
struct timespec epoch_teller1_starts_to_wait_for_customer, epoch_teller1_receives_customer;

double teller2Wait = 0.0;
struct timespec epoch_teller2_starts_to_wait_for_customer, epoch_teller2_receives_customer;

double teller3Wait = 0.0;
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
                    teller1Wait+=time_teller1_waited_for_customer;
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
                    teller1Wait+=time_teller1_waited_for_customer;
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
                    teller2Wait+=time_teller2_waited_for_customer;
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
                    teller2Wait+=time_teller2_waited_for_customer;
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
                    teller3Wait+=time_teller3_waited_for_customer;
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
            else{//bank is open but there is no one in line!
                pthread_mutex_unlock( &lock );
            }
        }
        else{ //bank closed
            clock_gettime( CLOCK_REALTIME, &epoch_customer_leaves_queue_to_teller);
            pthread_mutex_lock( &lock );
            if (Q->size){ //but there are customers!
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
                    teller3Wait+=time_teller3_waited_for_customer;
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
            clock_gettime( CLOCK_REALTIME, &epoch_customer_starts_waiting_in_queue);
            system(0);
            printf("Size of line: %d\n",Q->size);
            printf("Customer Added to Queue!\n\n");
            i++; //# OF CUSTOMER TOTAL
            current_queue_depth = Q->size;
            if (current_queue_depth > max_queue_depth){
                max_queue_depth = current_queue_depth;
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

    total_customers = (customers_served_by_teller1 + customers_served_by_teller2 + customers_served_by_teller3);
    tellers_total_work_time = (time_teller1_worked + time_teller2_worked + time_teller3_worked);
    printf("1.) Total number of customers serviced: %d customers\n", total_customers);
    printf("2.) Average time customer spends in queue: %lf seconds\n",msRealToSim((time_customers_wait_for_teller1+time_customers_wait_for_teller2+time_customers_wait_for_teller3)*1000)/total_customers); //needs to be checked
    printf("3.) Average time customer spends with teller: %d seconds\n",(tellers_total_work_time/total_customers));
    printf("4.) Average time teller waits for customer: %lf seconds\n",msRealToSim((teller1Wait+teller2Wait+teller3Wait)*1000)/total_customers);//needs checking
    max_time_waited_by_teller1_customer=msRealToSim(max_time_waited_by_teller1_customer*1000);
    max_time_waited_by_teller2_customer=msRealToSim(max_time_waited_by_teller2_customer*1000);
    max_time_waited_by_teller3_customer=msRealToSim(max_time_waited_by_teller3_customer*1000);
    if (max_time_waited_by_teller1_customer>=max_time_waited_by_teller2_customer && max_time_waited_by_teller1_customer>=max_time_waited_by_teller3_customer && printFlag==0){//tellers could have same max but dont care - just the value
        printFlag = 1;
        printf("5.) Maximum customer wait time in queue: %f seconds\n",max_time_waited_by_teller1_customer);
    }
    if (max_time_waited_by_teller2_customer>=max_time_waited_by_teller1_customer && max_time_waited_by_teller2_customer>=max_time_waited_by_teller3_customer && printFlag==0){
        printFlag = 1;
        printf("5.) Maximum customer wait time in queue: %f seconds\n",max_time_waited_by_teller2_customer);
    }
    if (max_time_waited_by_teller3_customer>=max_time_waited_by_teller1_customer && max_time_waited_by_teller3_customer>=max_time_waited_by_teller2_customer && printFlag==0){
        printf("5.) Maximum customer wait time in queue: %f seconds\n",max_time_waited_by_teller3_customer);
    }
    printFlag=0;

    max_time_waited_by_teller1_for_customer=msRealToSim(max_time_waited_by_teller1_for_customer*1000);
    max_time_waited_by_teller2_for_customer=msRealToSim(max_time_waited_by_teller2_for_customer*1000);
    max_time_waited_by_teller3_for_customer=msRealToSim(max_time_waited_by_teller3_for_customer*1000);
    if (max_time_waited_by_teller1_for_customer>=max_time_waited_by_teller2_for_customer && max_time_waited_by_teller1_for_customer>=max_time_waited_by_teller3_for_customer && printFlag==0){//tellers could have same max but dont care - just the value
        printFlag = 1;
        printf("6.) Maximum wait time for tellers waiting for customers: %f seconds\n",max_time_waited_by_teller1_for_customer);
    }
    if (max_time_waited_by_teller2_for_customer>=max_time_waited_by_teller1_for_customer && max_time_waited_by_teller2_for_customer>=max_time_waited_by_teller3_for_customer && printFlag==0){
        printFlag = 1;
        printf("6.) Maximum wait time for tellers waiting for customers: %f seconds\n",max_time_waited_by_teller2_for_customer);
    }
    if (max_time_waited_by_teller3_for_customer>=max_time_waited_by_teller1_for_customer && max_time_waited_by_teller3_for_customer>=max_time_waited_by_teller2_for_customer && printFlag==0){
        printf("6.) Maximum wait time for tellers waiting for customers: %f seconds\n",max_time_waited_by_teller3_for_customer);
    }
    printFlag=0;

    if (teller1_longest_transaction>=teller2_longest_transaction && teller1_longest_transaction>=teller3_longest_transaction && printFlag==0){//tellers could have same max but dont care - just the value
        printFlag = 1;
        printf("7.) Maximum transaction time for the tellers: %d seconds\n",teller1_longest_transaction);
    }
    if (teller2_longest_transaction>=teller1_longest_transaction && teller2_longest_transaction>=teller3_longest_transaction && printFlag==0){
        printFlag = 1;
        printf("7.) Maximum transaction time for the tellers: %d seconds\n",teller2_longest_transaction);
    }
    if (teller3_longest_transaction>=teller1_longest_transaction && teller3_longest_transaction>=teller2_longest_transaction && printFlag==0){
        printf("7.) Maximum transaction time for the tellers: %d seconds\n",teller3_longest_transaction);
    }
    printf("8.) Maximum depth of customer queue: %d customers\n\n",max_queue_depth);
    pthread_mutex_destroy(&lock);
    return EXIT_SUCCESS;
}
