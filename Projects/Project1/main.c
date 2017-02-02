#include "stm32l476xx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_LOW 950
#define DEFAULT_HIGH 1050 
#define SAMPLES 1000

int POST(unsigned char *PIN); //prototype for Power On Self Test
int main(void){
	//POST - Power on self test
	//GPIO sees at leasr 1 pulse ever 100ms
	//If fails, option to POST again - pass then move to UI
	
	//UI
	//Display lower and upper limits
	//-> 950 micro to 1050 micro
	//Prompt user for acceptance or new limits
	//CONSTRAINT! Upper limit must be 100 micro above lower
	//Lower can range from 50 micro to 9950 micro
	//Start prompt and wait for enter to begin
	//Take 1000 measurements(1001 rising edges)
	//DNon-zero histogram count in ascending on vTerminal
	//Dont display 
	
	unsigned char *GPIOA_ptr = GPIOA_BASE; //GPIO pin A
	int pass;
	pass=POST(GPIOA_ptr); //POST function on GPIO pin A
	if (pass==0){
		return(0);
	}
	else{
		//Capture 1000 measurements here - implement interupts
	}
}

int POST(unsigned char *PIN){
	//80MHZ clk - .0000000125 sec poll
	//implement static delcaration for original GPIO pin discover and if on toggle/difference
	int counter=0;
	char input;
	while(counter<8000000){ // (.100/.0000000125)
		if(*PIN==1){ //pulse see
			break; //add functionality to see the resulkt of when pulse was seen
			return(1);
		}
		else{
			printf("POST failed! Pulse not seen in 100ms. Rerun? (Y or N): ");
			scanf("%c",&input);
			if (input == 'Y'||input=='y'){
				POST(PIN);
				return 0;
			}
			else{ //input wasn't y or Y
				return 0;
			}
		}
		counter++;
	}
}