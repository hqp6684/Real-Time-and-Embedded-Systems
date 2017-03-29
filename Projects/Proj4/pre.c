
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN_ARRIVAL (60) //1 minute
#define MAX_ARRIVAL (240)//4 minutes

#define MIN_TRANSACTION (30) //30 seconds
#define MAX_TRANSACTION (480)//8 minutes


int array[]; //Need this to be dynamic - keep appended customers to queue - initialize dont specify size

int randMToN(int M, int N)
{
	return M + (rand() / ( RAND_MAX / (N+1-M) ) ) ;
}

int main(void) {
	int i=0;
	int arrivalTime=0;
	int transactionTime=0;
	while(1){
		arrivalTime=randMToN(MIN_ARRIVAL,MAX_ARRIVAL);					//generate random arrival time of customer
		if (arrivalTime+currentTime)<closingTime{						//pseudo code for checking hours of operations condition
			transactionTime=randMToN(MIN_TRANSACTION,MAX_TRANSACTION);	//generate random transaction time of customer
			sleep(arrivalTime); 										//dont append to queue until after sleep
			array[i]=transactionTime; 									//queue will be array of customer transaction times
			i++;
		}
		else{
			//Customer cant be seen because it is past hours - bank is closed
			break;
		}
	}
	report();//report function here to display metrics
	return EXIT_SUCCESS;
}
