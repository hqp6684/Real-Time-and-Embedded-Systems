// L. Kiser Feb. 8, 2017

#include "stm32l476xx.h"
#include "input_pa0_test.h"
#include "UART.h"
#include <string.h>
#include <stdio.h>

uint8_t buffer[BufferSize];
// Turn on the peripheral clock for GPIOA
// and set PA0 to be input mode
void init_pa0( void )
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ;		// enable clock for A group of GPIO
	GPIOA->MODER &= ~3 ;										// clear out bits 0 and 1 for PA0
																					// PA0 is now in input mode
	// GPIOA->MODER |= 2 ;									// Enable alternate function mode (binary 10) for PA0
}

// Read the state of PA0 (high or low)
int read_pa0( void )
{
	return GPIOA->IDR & 1 ;				// read the bottom bit of the GPIOA input lines which is PA0.
																// returns 1 if high, 0 if low.
}

// This is an infinite loop that will show if PA0 is changing state
void monitor_pa0( void )
{
	int n;
	int numberOfHighPulses=0;
	int highPulseTicks=0;
	int incrementOnce = 0;
	while ( 1 ){
		if ( read_pa0() ){ //high part of pulse detected
			//if timecounter=0 - end timer
			//else start time
			if (incrementOnce == 0){
				numberOfHighPulses++;
				incrementOnce=1;
			}
			if (numberOfHighPulses<500){
					highPulseTicks++;
				  n = sprintf((char *)buffer, "%d\r\n", highPulseTicks);
				  USART_Write(USART2, buffer, n);	
					//USART_Write(USART2, (uint8_t *)"High Detected\r\n\r\n", 32);
			}
		}
		else{ //in the low part of the pulse
		  n = sprintf((char *)buffer, "\r\n");
			USART_Write(USART2, buffer, n); //print blank ticks to indicate we're low
			highPulseTicks=0;
			incrementOnce=0;
		}
	}

}
