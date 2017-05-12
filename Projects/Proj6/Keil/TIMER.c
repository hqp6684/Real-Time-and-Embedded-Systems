// Z. Weeden 2017

#include "stm32l476xx.h"
#include "SysClock.h"   // Sysclock
#include <stdio.h>
#include <stdlib.h>

/* Timer init and pin config for PWM on PA0 and input PB. */
void init_timer(void){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;    // clock enable on GPIO
    GPIOA->MODER &= ~(0xFFFFFFFF);          // bit mask and clear register 31..0
    GPIOA->MODER |= 0xA;                    // 1010 pa0 alternate function
    GPIOA->AFR[0] |= 0x11;                  // 0001 0001 pa0  alternate function
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;   // TIM2 timer clock enable
    
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;    // port b input
    GPIOB->MODER &= ~GPIO_MODER_MODE2;      // pb2 input
    GPIOB->PUPDR &= 0;
    GPIOB->PUPDR &= 0x2;                    // pulldown resistor
    
    TIM2->PSC = 8000;   
    TIM2->CCMR1 &= ~(0x00000003);   // 0000 0011 channels 1 output
    TIM2->CCMR1 |= (0x00000068);    // 1010 1000 preload enable and compare modes
    TIM2->CR1 |= (0x00000080);      // 1000 0000 autoload arr
    TIM2->CCER |= (0x00000001);     // 0000 0001 enable channel
    TIM2->CCER |= TIM_CCER_CC1P;    // active high ch1
    TIM2->ARR = 200;
    TIM2->CCR1 = 4;             // initial pwm var for postition 0
    TIM2->EGR |= TIM_EGR_UG;    // create update event for prescale force
    TIM2->CR1 = 0x1;            // control register counter enabled
}

/* Read PB0 used for indicator */
int pb_event(void){
    return (GPIOB->IDR & 1);                                              
}

/* Read voltage of Port B */
int fetch_voltage_port_b(void){
    return (GPIOB->IDR & 255);
}

