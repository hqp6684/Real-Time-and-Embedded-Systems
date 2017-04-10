// Z. Weeden Mar. 29, 2017

#include <stdio.h>
#include <stdlib.h>
#include "report.h"

/* This is the function used to report all the metrics. It is basically a parameterized wrapper around all metrics/calculations completed in the main program.
   Parameters include those that are defined in main. Basic placeholder prints can be found here as well as minor calulations including averages over the total
   amount of customers. A print flag is also used to ensure that some print statements are not shown in duplicate as a result of same maximum values for multiple
   tellers. */
void report_metrics(int total_customers, double total_time_customers_waited_for_all_tellers, int tellers_total_work_time, double total_time_waited_by_all_tellers_for_customer, double max_time_waited_by_teller1_customer, double max_time_waited_by_teller2_customer, double max_time_waited_by_teller3_customer, double max_time_waited_by_teller1_for_customer, double max_time_waited_by_teller2_for_customer, double max_time_waited_by_teller3_for_customer, int teller1_longest_transaction, int teller2_longest_transaction, int teller3_longest_transaction, int max_queue_depth){
    int printFlag = 0;
	printf("1.) Total number of customers serviced: %d customers\n", total_customers);
    printf("2.) Average time customer spends in queue: %lf seconds\n",total_time_customers_waited_for_all_tellers/total_customers);
    printf("3.) Average time customer spends with teller: %d seconds\n",(tellers_total_work_time/total_customers));
    printf("4.) Average time teller waits for customer: %lf seconds\n",total_time_waited_by_all_tellers_for_customer/total_customers);
    if (max_time_waited_by_teller1_customer>=max_time_waited_by_teller2_customer && max_time_waited_by_teller1_customer>=max_time_waited_by_teller3_customer && printFlag==0){ //Find maximum OF maximum for each individual teller - could have multiple maximums only print once
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
    if (max_time_waited_by_teller1_for_customer>=max_time_waited_by_teller2_for_customer && max_time_waited_by_teller1_for_customer>=max_time_waited_by_teller3_for_customer && printFlag==0){ //Find maximum OF maximum for each individual teller - could have multiple maximums only print once
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
    if (teller1_longest_transaction>=teller2_longest_transaction && teller1_longest_transaction>=teller3_longest_transaction && printFlag==0){ //Find maximum OF maximum for each individual teller - could have multiple maximums only print once
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
}
