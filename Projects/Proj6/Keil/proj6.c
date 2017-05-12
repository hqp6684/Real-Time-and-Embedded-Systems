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

int main(void){
    double fine_ccr;
    int rounded_ccr;
    System_Clock_Init();
    LED_Init();
    UART2_Init();
    init_timer();
    USART_Write(USART2, (uint8_t *)"\r\nWelcome to Proj6\r\n", 20);

    while(1){ // Poll indefinitely to read output of purplebox
        if (is_event()){  //action seen on pin - need to get piece together pins for full number (input_voltage variable)
            /* pseudo range for resolving ccr
            if (input_voltage < 0 || input_voltage > 255){ //New scaling from QNX
                RED_LED_ON();
                USART_Write(USART2, (uint8_t *)"\r\nVoltage out of range.\r\n", 25);
            }
            else{
                RED_LED_OFF();
                fine_ccr = (-0.0667*(double)input_voltage)+21.242;
                rounded_ccr = round(fine_ccr);
                TIM2->CCR1 = rounded_ccr;
            }
            */
        }
    }
}
