// Z. Weeden 2017

#include "stm32l476xx.h"
#include "SysClock.h"   // Sysclock
#include <stdio.h>
#include <stdlib.h>
#include "UART.h"       // UART
uint8_t scaleDisplay[50];

/* Timer init and pin config for PWM on PA0 */
void init_pwm(void){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;    // clock enable on GPIO
    GPIOA->MODER &= ~(0x3);                 // bit mask and clear register 31..0
  
    GPIOA->MODER |= 0x2;                   // 0010 pa0 alternate function
    GPIOA->AFR[0] |= 0x1;                  // 0001 pa0  alternate function
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;  // TIM2 timer clock enable
    
    TIM2->PSC = 8000;   
    TIM2->CCMR1 &= ~(0x00000003);   // 0000 0011 channel 1 output
    TIM2->CCMR1 |= (0x00000068);    // 1010 1000 preload enable and compare modes
    TIM2->CR1 |= (0x00000080);      // 1000 0000 autoload arr
    TIM2->CCER |= (0x00000001);     // 0000 0001 enable channel

    TIM2->ARR = 200;
    TIM2->CCR1 = 4;             // initial pwm var for postition 0
    TIM2->EGR |= TIM_EGR_UG;    // create update event for prescale force
    TIM2->CR1 = 0x1;            // control register counter enabled  
}

/* Read voltage - order of pins right down the row. This function resolves the 
   voltage from pins to a matching bitfield in which an accumulation of voltage is
   determined using the pins output as an exponent flag. */
int fetch_voltage(void){
    int e15_bit, e14_bit, e13_bit, e12_bit, e11_bit, e10_bit, h1_bit, h0_bit = 0;
    int resolved_voltage = 0;
    e15_bit=(GPIOE->IDR & 0x8000); //pe15 - dio a0
    e14_bit=(GPIOE->IDR & 0x4000); //pe14 - dio a1
    e13_bit=(GPIOE->IDR & 0x2000); //pe13 - dio a2
    e12_bit=(GPIOE->IDR & 0x1000); //pe12 - dio a3
    e10_bit=(GPIOE->IDR & 0x400);  //pe10 - dio a4 !!Due to pin layout
    e11_bit=(GPIOE->IDR & 0x800);  //pe11 - dio a5 !!Due to pin layout
    h1_bit=(GPIOH->IDR & 0x2); //ph1 - dio a6
    h0_bit=(GPIOH->IDR & 0x1); //ph0 - dio a7
    if (e15_bit > 0){//b0
      resolved_voltage+=1;
    }
    if (e14_bit > 0){//b1
      resolved_voltage+=2;
    }
    if (e13_bit > 0){//b2
      resolved_voltage+=4;
    }
    if (e12_bit > 0){//b3
      resolved_voltage+=8;
    }
    if (e10_bit > 0){//b4 !!Pin layout
      resolved_voltage+=16;
    }
    if (e11_bit > 0){//b5 !!Pin layout
      resolved_voltage+=32;
    }
    if (h1_bit > 0){//b6
      resolved_voltage+=64;
    }
    if (h0_bit > 0){//b7
      resolved_voltage+=128;
    }
    return resolved_voltage;
}

/* Set up pins as inputs for QNX signal to be read from. */
void init_gpio(void){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;    // clock enable on GPIO
    
    GPIOE->MODER &= ~(0xFFF00000);//E15,14,13,12,11,10
    GPIOH->MODER &= ~(0x0000000F);//H0,1
}
