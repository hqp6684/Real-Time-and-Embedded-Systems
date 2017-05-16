#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "timer.h"
#include "recipe.h"

/* Control flow for project 2a */
int main(void){
    System_Clock_Init();
    UART2_Init();
    LED_Init();
    pwmstart(5);
    USART_Delay(5000000);
    runRecipes();
}
