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

/*

0 voltage = position 3
- voltage = positions 4 and 5
+ voltage = position 0, 1, and 2 

VOLTAGE   -5   -4   -3   -2   -1      0    1    2    3    4    5 
POSITION   5           4              3       2        1       0
CCRx       21          16            13       10       7       4  

*/

//function to be ran on STM
void monitorADCVoltagePinUpdateCCR(void){
    static int joystickButton = 0; //monitor josytick press
    //joystickButton = register?;
    while (1){
        if (joystickButton == 1){
            joystickButton = 1; //acts as init - doesnt need to be held high
            voltage = fetchVOLTAGEFromPIN(); //continually update
            if (voltage < 0 && voltage > -5){  //negative voltage leftmost position
                RED_LED_OFF(); //valid voltage
                CCRx = 21 //position5;
            }
            else if (voltage == 0){ 
                RED_LED_OFF(); //valid voltage
                CCRx = 13 //position3;
            }
            else if (voltage > 0 && voltage < 5){ //positive voltage rightmost position
                RED_LED_OFF(); //valid voltage
                CCRx = 4 //position0;
            }
            else if (voltage > 5){ //voltage has gone over accepted value
                RED_LED_ON(); //indicate failure on STM/servo
                //printf("Voltage has gone over +5V\n"); //indicate failure in momentics/qnx
            }
            else if (voltage < -5){ //voltage has gone lower accepted value
                RED_LED_ON(); //indicate failure on STM/servo
                //printf("Voltage has gone under -5V\n"); //indicate failure in momentics/qnx
            }
            else{
                ;
            }
        }
    }
}

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
        ;
    }
}
