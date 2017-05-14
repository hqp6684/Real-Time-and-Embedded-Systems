// L. Kiser Feb. 8, 2017

#include "stm32l476xx.h"
#include "input_pa0_test.h"
#include "LED.h"

// Turn on the peripheral clock for GPIOA
// and set PA0 to be input mode
void init_pa0( void )
{ //GPIOA MODE R page 278 - as an input pin GPIO_idr used can use bottom 16
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ;		// enable clock for A group of GPIO
	GPIOA->MODER &= ~3 ;										// clear out bits 0 and 1 for PA0
																					// PA0 is now in input mode
	// GPIOA->MODER |= 2 ;									// Enable alternate function mode (binary 10) for PA0
}

GPIOE->MODER &= ~0xC0000000 ;//E15
GPIOE->MODER &= ~0x30000000 ;//E14
GPIOE->MODER &= ~0xC000000 ;//E13
GPIOE->MODER &= ~0x3000000 ;//E12
GPIOE->MODER &= ~0xC00000 ;//E11
GPIOE->MODER &= ~0x300000 ;//E10
GPIOC->MODER &= ~0xC0000000 ;//C15
GPIOC->MODER &= ~0x30000000 ;//C14

// Read the state of PA0 (high or low)
int read_pa0( void )
{
	return GPIOA->IDR & 1 ;				// read the bottom bit of the GPIOA input lines which is PA0.
																// returns 1 if high, 0 if low.
}

// This is an infinite loop that will show if PA0 is changing state
void monitor_pa0( void )
{
	while ( 1 )
		if ( read_pa0() )
			Red_LED_On();
		else
			Red_LED_Off() ;
}
