// L. Kiser Feb. 8, 2017
// Contributor: Zachary Weeden
#include "stm32l476xx.h"
#include "input_pa0_test.h"
#include "UART.h"
#include "TIMER.h"
#include <string.h>
#include <stdio.h>

// Turn on the peripheral clock for GPIOA
// and set PA0 to be input mode
void init_pa0( void ){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ;       // enable clock for A group of GPIO
    GPIOA->MODER &= ~15 ;                       // clear out bits 0 and 1 for PA0 and bits 2 and 3 for PA1 (xxxx)
                                                // PA0 is now in input mode
    GPIOA->MODER |= 10 ;                        // Enable alternate function mode (binary 1010) for PA0 and PA1 
    init_timer();                               // AF for pin and configure timer
}
