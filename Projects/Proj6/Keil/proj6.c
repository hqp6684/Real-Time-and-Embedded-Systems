// Z. Weeden 2017

#include "stm32l476xx.h"    // Board
#include "SysClock.h"       // Sysclock
#include "LED.h"            // LED
#include "UART.h"           // UART
#include "TIMER.h"          // TIMER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t bounds[50]; //UART print buffer

/* Control program flow of STM portion. Continually monitor pins e15-10 and h1-0 for voltage from 
   QNX - resolves CCR value with equation from linear regression trendline found in
   excel. This equation scales and converts 0-255 values to a CCR approiate for the determined 
   scaled voltage. */
int main(void){
    int n;
    int voltage_on_pins;
    double fine_ccr;
    int rounded_ccr;
    System_Clock_Init();
    LED_Init();
    UART2_Init();
    init_pwm();
    init_gpio();
    USART_Write(USART2, (uint8_t *)"\r\nWelcome to Proj6\r\n", 20);

    while(1){ // Poll indefinitely to read output of purplebox
        voltage_on_pins = fetch_voltage();
        if (voltage_on_pins < 0 || voltage_on_pins > 255){ //New scaling from QNX
            Red_LED_On();
            USART_Write(USART2, (uint8_t *)"\r\nVoltage out of range.\r\n", 25);
        }
        else{
            Red_LED_Off();
            fine_ccr = (-0.0667*(double)voltage_on_pins)+21.242; //Equation to convert qnx scale to a ccr value - dependent on QNX scale used
            rounded_ccr = (int)(fine_ccr+.5); //work around for no math round function
            n = sprintf((char *)bounds, "Seen voltage: [%f] || CCR: [%f], rounded [%d]\r\n\r\n", ((voltage_on_pins/25.5)-5.0),fine_ccr,rounded_ccr);
            USART_Write(USART2, bounds, n);
            TIM2->CCR1 = rounded_ccr;
        }
    }
}
