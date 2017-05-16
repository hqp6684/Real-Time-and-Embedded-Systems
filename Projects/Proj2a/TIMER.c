#include "timer.h"

/* Sets various registers properly to set up a PWM output on pina PA0 and PA1 */
void pwmstart(uint32_t servo_null){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;//enable clock
    GPIOA->MODER &= ~(0xFFFFFFFF); //PA0 TIM2 CH1
    GPIOA->MODER |= 10; //set PA0 AF
    GPIOA->MODER |= 10<<2; //set PA1 AF
    GPIOA->AFR[0] |= 1; //PA0/PA1 AF1 TIM2_CH1
    GPIOA->AFR[0] |= 1<<4; //PA0/PA1 AF1 TIM2_CH2
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; //timer2 enable
    TIM2->PSC = 8000; //20ms period
    TIM2->CCMR1 &= ~(0x303);
    TIM2->CCMR1 |= 1 << 3; // preload enable
    TIM2->CCMR1 |= 1 << 11; // preload enable 2
    TIM2->CCMR1 |= 110 << 4; // pwm mode 1 OCM1
    TIM2->CCMR1 |= 110 << 12; // pwm mode 1 OCM2
    TIM2->CCMR1 &= ~(1 << 10);
    TIM2->CCMR1 &= ~(11 << 8);
    TIM2->CR1 |= 1 <<7;// preload auto reload
    TIM2->CCER  |= 1; //cc1 output
    //TIM2->CCER |= 1<<1; //output polarity active high
    TIM2->CCER  |= 1<<4; //cc2 output
    //TIM2->CCER |= 1<<5; // c2 polarit active high;
    TIM2->ARR = 200; // auto reload register load with 200
    TIM2->CCR1 |= servo_null;
    TIM2->CCR2 |= servo_null;
    TIM2->EGR |= TIM_EGR_UG; //load prescaler
    TIM2->SR &= ~TIM_SR_UIF; // clear update flag
    TIM2->CR1 = 0x1; //enable counter
}

/* This function is responsible for changing the duty cycle of the PWM
   signal by adjsut the compare capture registers valule. Used to movedata
   servos accordingly. */
void changePWM(uint8_t pwm_num, uint32_t pwmrange){
    if(pwm_num == PWM0){
        TIM2->CCR1 = pwmrange;
    }
    if(pwm_num == PWM1){ 
        TIM2->CCR2 = pwmrange; 
    }
}
