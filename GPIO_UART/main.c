#include <string.h>
#include <stdio.h>

#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "input_pa0_test.h"
#include "UART.h"

int main(void)
{
	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init();
	init_pa0() ;
	monitor_pa0() ;		// loops forever!	
}

