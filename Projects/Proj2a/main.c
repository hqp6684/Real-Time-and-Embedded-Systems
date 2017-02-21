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

int main (void){
	System_Clock_Init();    // System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	init_pa0_pa1();
}
