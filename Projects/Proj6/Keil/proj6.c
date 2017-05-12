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

/* Control program flow of STM portion. Continually monitor port b for voltage from 
   QNX - resolves CCR value with equation from linear regression trendline found in
   excel. */
int main(void){
    int voltage_on_pins;
    double fine_ccr;
    int rounded_ccr;
    System_Clock_Init();
    LED_Init();
    UART2_Init();
    init_timer();
    USART_Write(USART2, (uint8_t *)"\r\nWelcome to Proj6\r\n", 20);

    while(1){ // Poll indefinitely to read output of purplebox
        if (pb_event()){  //action seen on pin
            voltage_on_pins = fetch_voltage_port_b();
            if (voltage_on_pins < 0 || voltage_on_pins > 255){ //New scaling from QNX
                RED_LED_ON();
                USART_Write(USART2, (uint8_t *)"\r\nVoltage out of range.\r\n", 25);
            }
            else{
                RED_LED_OFF();
                fine_ccr = (-0.0667*(double)voltage_on_pins)+21.242;
                rounded_ccr = round(fine_ccr);
                TIM2->CCR1 = rounded_ccr;
            }
        }
    }
}
