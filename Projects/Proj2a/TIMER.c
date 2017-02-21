// Z. Weeden Feb. 18, 2017
#include "stm32l476xx.h"
#include "input_pa0_pa1_test.h"
#include "SysClock.h"    //Sysclock
#include "TIMER.h"
#include <string.h>
#include <stdio.h>

#define SYSTEM_CLK 8000   // STM 80MHZ

/* This starts the timer by setting the enable flag of the control reg. */
void start_timer( void ) {
    TIM2->CR1 = 0x1;                        // control register counter enabled
}
/* This is the initial confiuguration of the timer. */
void init_timer( void ) {
    GPIOA->AFR[0] |= 0x11;                  // (0001 0001) PA0 and PA1 alternate function 1 (TIM2_CH1)
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;   // TIM2 timer clock enable
    TIM2->PSC = SYSTEM_CLK;                 // Prescaler value
    TIM2->CCMR1 &= ~(0xFFFFFFFF);           // clear 0011 0000 0011
    TIM2->CCMR1 |= 0x6868;                  // CC1 and CC2 as outputs with preload enables and output compare mode in PWM (0110 1000 0110 1000)
    TIM2->CR1 |= 0x80;                      // autoreload register is buffered (preload enabled) (1000 0000)
    //TIM2->CCER &= ~(0xFFFFFFFF);            // turn off capture input until we're ready with updates
    TIM2->CCER |= 0x11;                     // enable capture input cc1e and cc2e - signal is output on pin (0001 0001)
    TIM2->ARR = 200;                        
		TIM2->CCR1 = 0x4;
		TIM2->CCR2 = 0x4;

    //NEED TO MODIFY - initial conditions 
    TIM2->EGR |= TIM_EGR_UG;                // create update event for prescale force
    start_timer();

}
/* This fetches the value in the capt/compare reg (the timer's current value). The
value returned is the time elapsed since enabled in microseconds. */
unsigned int timer_now( void ) {
    return (unsigned int)TIM2->CCR1;        // capture/compare register
}
/* This stops the timer by disabling the control register. */
void stop_timer( void ) {
    TIM2->CR1 = 0x0;                        // control register counter diabled
}
/* This determines if an edge/event has been seen. The return value is the 
flag of the status registers bit 1. */
int is_event( void ) {
    return (TIM2->SR & 0x2);                // return flag indicating capture/compare interrupt flag
}
