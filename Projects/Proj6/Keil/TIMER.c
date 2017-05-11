// Z. Weeden 2017

#include "stm32l476xx.h"
#include "SysClock.h"   // Sysclock
#include <stdio.h>
#include <stdlib.h>

/* Timer init and pin config for PWM on PA0. */
void init_timer(void){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;    // clock enable on GPIO
    GPIOA->MODER &= ~(0xFFFFFFFF);          // bit mask and clear register 31..0
    GPIOA->MODER |= 0x2;                    // 0000 0010 pa0 alternate function
    GPIOA->AFR[0] |= 0x1;                   // 0001 pa0  alternate function
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;   // TIM2 timer clock enable
    TIM2->PSC = 8000;   
    TIM2->CCMR1 &= ~(0x00000003);   // 0000 0011 channels 1 output
    TIM2->CCMR1 |= (0x00000068);    // 1010 1000 preload enable and compare modes
    TIM2->CR1 |= (0x00000080);      // 1000 0000 autoload arr
    TIM2->CCER |= (0x00000001);     // 0000 0001 enable channel
    TIM2->ARR = 200;
    TIM2->CCR1 = 4;             // initial pwm var for postition 0
    TIM2->EGR |= TIM_EGR_UG;    // create update event for prescale force
    TIM2->CR1 = 0x1;            // control register counter enabled
}

/* Delay function that takes an int param that specifies the number of milliseconds to decrement in spin. */
void delay(int ms) {
    int time_elapsed = 0;
    if(ms > 0){ // handle wait 0 recipe
        time_elapsed = (100000 * ms)/7;
        while(time_elapsed > 0){ // decrement input count until 0 to simulate wait
            time_elapsed--;
        }
    }
}

/* This determines if an edge/event has been seen. The return value is the 
flag of the status registers bit 1. */
int is_event( void ) {
    return (TIM2->SR & 0x2);                // return flag indicating capture/compare interrupt flag
}
