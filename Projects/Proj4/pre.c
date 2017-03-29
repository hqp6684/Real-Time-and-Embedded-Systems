
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN_ARRIVAL (60) //1 minute
#define MAX_ARRIVAL (240)//4 minutes

int array[]; //Need this to be dynamic - keep appended customers to queue - initialize dont specify size

int randMToN(int M, int N)
{
	return M + (rand() / ( RAND_MAX / (N+1-M) ) ) ;
}

int main(void) {

	int i=0;
	int arrivalTime=0;
	
	while(1){ //check hours of operations condition
		arrivalTime=randMToN(MIN_ARRIVAL,MAX_ARRIVAL);//generate random arrival time of customer
		if (arrivalTime+currentTime)<closingTime{
			sleep(arrivalTime); //dont append to queue until after sleep
			array[i]=arrivalTime;
			i++;
		}
		else{
			//Customer cant be seen because it is past hours - bank is closed
		}
	}

	return EXIT_SUCCESS;
}
