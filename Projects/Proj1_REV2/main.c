#include "stm32l476xx.h" //board
#include "SysClock.h"    //Sysclock
#include "LED.h"         //LED
#include "UART.h"        //UART
#include "TIMER.h"       //TIMER
#include "input_pa0_test.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLES 1000				//Number of samples to be taken
#define DEFAULT_LOW 950				//Default Low boundary micro
#define DEFAULT_HIGH 1050			//Default High boundry micro
#define POST_REQ_TIME 100000		//100000 microseconds = 100ms

char str[] = "POST failed! Pulse not seen in 100ms. Rerun? (Y or N):\r\n";
char defaultBounds[] = "Using default bounds! (950 micro & 1050 micro) Change? (Y or N):\r\n";

int measurements[SAMPLES]; // initialize array of 1000 elements

int FAIL( void ){
	char rxByte;
	int		n ;
	USART_Write(USART2, (uint8_t *)str, strlen(str));
	rxByte = USART_Read(USART2);
	if (rxByte == 'N' || rxByte == 'n'){
		USART_Write(USART2, (uint8_t *)"Exitting\r\n\r\n", 14);
		//FAIL and exit program
		return 0;
	}
	else if (rxByte == 'Y' || rxByte == 'y'){
		USART_Write(USART2, (uint8_t *)"Rerunning POST\r\n\r\n", 20);
		return POST();
	}
	else {
		USART_Write(USART2, (uint8_t *)"Invalid Response\r\n\r\n", 22);
		return FAIL();
	}
}

int POST( void ) {
	start_timer();			// start capture
	beginPostTime = timer_now();
	while( 1 ){
		if ( is_event() ) {	// received edge TIM2->CCR1
			if ( ( timer_now() - beginPostTime ) <= POST_REQ_TIME ){ // test received an edge before 100000micro (100ms)
				//DISPLAY success on UART/Time seen
				return 1;
				break;
			}
			else{ // test received edge but too long
				//TEST FAILED!
				//OPTION TO RUN AGAIN
				return FAIL();
				break;
			}
		}
		else { // nothing seen yet TIM2->CNT
			if ( ( timer_now() - beginPostTime ) > POST_REQ_TIME ){ // exceeded 100ms 
				//TEST FAILED!
				//OPTION TO RUN AGAIN
				return FAIL();
				break;
			}
		}
	}
	stop_timer();
}

void run( void ){
	char rxByte;
	int		n ;
	USART_Write(USART2, (uint8_t *)defaultBounds, strlen(defaultBounds));
	rxByte = USART_Read(USART2);
	if (rxByte == 'N' || rxByte == 'n'){
		USART_Write(USART2, (uint8_t *)"Running with defaults\r\n\r\n", 27);
		//RUN AS USUAL
	}
	else if (rxByte == 'Y' || rxByte == 'y'){
		USART_Write(USART2, (uint8_t *)"Changing Bounds\r\n\r\n", 21);
		//Change bounds handle user input 
	}
	else {
		USART_Write(USART2, (uint8_t *)"Invalid Response\r\n\r\n", 22);
		run();
	}

	//capture 1000 pulses
	//Return time it took - store in array in main
	for ( int numOfSample = 0; numOfSample < SAMPLES; numOfSample++ ){
		//detect pulse and duration (multiply by 2 for total period?)
		start_timer();
		beginSampleTime = timer_now();
		while( 1 ){
			if ( is_event() ){
				measurements[numOfSample] = (timer_now() - beginSampleTime); //(possibly multiplied by 2)
				break;
			}
			else{
				//edge not detected
				;
			}
		}
		stop_timer(); // might need to reset
	}

}

void UART_graph( void ){
	//maybe take in array as param instead of using global
	int n;
	for ( int i = 0; i < SAMPLES; i++ ){
		n = sprintf((char *)buffer, "%d\r\n", measurements[i]);
		USART_Write(USART2, buffer, n);	
	}
}

int main( void )
{
	
	System_Clock_Init();	// System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	init_pa0();
	
	//Implement indefinite loop here
	pass = POST();			// Power On Self Test
	if ( pass == 1 ){
		// POST PASSED! - continue operation
		// display default bounds and prompt user for upper and lower bounds
		// 50micro<=lower<-9950micro
		// upper >=lower + 100micro
		run(); //store return value into (global) array
		UART_graph();
	}
	else{
		//RECEIVED SOMETHING NOT 1
		//EXIT PROGRAM
		return 0;
	}
	//monitor_pa0();		// spin forever
}