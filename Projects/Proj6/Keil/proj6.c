// Z. Weeden 2017

#include "stm32l476xx.h"    // Board
#include "SysClock.h"       // Sysclock
#include "LED.h"            // LED
#include "UART.h"           // UART
#include "TIMER.h"          // TIMER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// voltage         +5V        0V    -5V
// position        0  1   2   3   4   5
int pwm_vars[6] = {4, 7, 10, 13, 16, 21};

/* pseudo range for resolving ccr

if (input_voltage < -5.0 || input_voltage > 5.0){
    RED_LED_ON();
    USART_Write(USART2, (uint8_t *)"\r\nVoltage out of range.\r\n", 25);
}
else{
    RED_LED_OFF();
    double fine_ccr = (-1.70*input_voltage)+12.727;
    int rounded_ccr = round(fine_ccr);
    TIM2->CCR1 = rounded_ccr;
}
*/
int main(void){
    System_Clock_Init();
    LED_Init();
    UART2_Init();
    init_timer();
    USART_Write(USART2, (uint8_t *)"\r\nWelcome to Proj6\r\n", 20);
    
    TIM2->CCR1 = pwm_vars[0]; // voltage is +5V
    //TIM2->CCR1 = pwm_vars[3]; // voltage is 0V
    //TIM2->CCR1 = pwm_vars[5]; // voltage is -5V
    //TIM2->CCR2 = pwm_vars[3]; // other motor example CCR2

    while(1){ // Poll indefinitely to read output of purplebox
        if (is_event()){
            //action seen on pin from purple box
        }
    }
}
