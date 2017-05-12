// Z. Weeden 2017

#include "stm32l476xx.h"    // Board
#include "SysClock.h"       // Sysclock
#include "LED.h"            // LED
#include "UART.h"           // UART
#include "TIMER.h"          // TIMER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// voltage         +5V        0V    -5V
// position        0  1   2   3   4   5
int pwm_vars[6] = {4, 7, 10, 13, 16, 21};

/* pseudo range for resolving ccr
if (input_voltage >= -5.0 && input_voltage < -3.33){
    RED_LED_OFF();
    TIM2->CCR1 = 21;
}
else if (input_voltage >= -3.33 && input_voltage < -1.67){
    RED_LED_OFF();
    TIM2->CCR1 = 16;
}
else if (input_voltage >= -1.67 && input_voltage < 1.25){
    RED_LED_OFF();
    TIM2->CCR1 = 13;
} 
else if (input_voltage >= 1.25 && input_voltage < 2.5){
    RED_LED_OFF();
    TIM2->CCR1 = 10;
}
else if (input_voltage >= 2.5 && input_voltage < 3.75){
    RED_LED_OFF();
    TIM2->CCR1 = 7;
}
else if (input_voltage >= 3.75 && input_voltage <= 5.0){
    RED_LED_OFF();
    TIM2->CCR1 = 4;
}
else{
    RED_LED_ON();
    USART_Write(USART2, (uint8_t *)"\r\nVoltage out of range.\r\n", 25);
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
