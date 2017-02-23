// Z. Weeden Feb. 18, 2017
#include "stm32l476xx.h" // Board
#include "SysClock.h"    // Sysclock
#include "LED.h"         // LED
#include "UART.h"        // UART
#include "TIMER.h"       // TIMER
#include "input_pa0_pa1_test.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define test (1000) //DC around 3.6% - should see 36 occurrences of 1 in 1000. (1000*.036)

uint8_t bounds[BufferSize]; 

int main (void){
	  int i = 0;
	  int n;
		int hit;
    System_Clock_Init();    // System Clock = 80 MHz
    LED_Init();
    UART2_Init();
    init_pa0_pa1();
		while (i<test){
			hit = (GPIOA->IDR & 1);	
			n = sprintf((char *)bounds, "[%d]\r\n", hit);
      USART_Write(USART2, bounds, n); 
			i++;
		}
}
