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

char overrideCmd[3]; // User input number buffer space - 
uint8_t bounds[BufferSize]; 
uint8_t buffer[BufferSize]; 

/* Does not return a value - more of initial setup/configuration.
Visually inspect that the servos move 1 position to the left and then all 
the way to the right.*/
void POST(void){
    //Move 1 to the left and then 5 to the right.
    //This starts us in a known position
}
 
void handle_input(void){ //Maybe return array of the input or individually parsed chars for motor1cmd motor2cmd 
    int j;
    char rxByte;
    int typo = 0;
    int index = 0;
    int cancel = 0;
    char motor1Cmd;
    char motor2Cmd;
    USART_Write(USART2, (uint8_t *)"Enter command: \r\n", 17);
    rxByte = USART_Read(USART2);
    for (j = 0; j < sizeof(overrideCmd)/sizeof(overrideCmd[0]); j++){ // Populate array with null terminators
        overrideCmd[j] = '\0';
    }
    while(rxByte != 0xD){ // No carriage return seen - oxD is hex ascii equivalent of \r - keep appending input to buffer
        if (rxByte == 0x78 || rxByte == 0x58){ //if input is 'x' or 'X'
            cancel = 1;
            break;
        }
        //Valid characters:  Pp Cc Rr Ll Nn Bb
        if (rxByte != 0x50 || rxByte != 0x70 || rxByte != 0x43 || rxByte != 0x63 || rxByte != 0x52 || rxByte != 0x72 || rxByte != 0x4C || rxByte != 0x6C || rxByte != 0x4E || rxByte != 0x6E || rxByte != 0x42 || rxByte != 0x62){
            typo = 1;
            break;
        }
        memset( buffer, '\0', sizeof(buffer)); // Null terminator to indicate end of string
        sprintf((char *)buffer, "%c", rxByte);
        USART_Write(USART2, buffer, sizeof(buffer));
        overrideCmd[index] = rxByte;
        index++;
        rxByte = USART_Read(USART2);
    }
    if (cancel == 0 && typo == 0){ //maybe return overrideCmd array
        sscanf(overrideCmd[0], "%c", &motor1Cmd);
        sscanf(overrideCmd[1], "%c", &motor2Cmd);        
    }
    else if (cancel == 1){//x was entered do not process '>' override char written
        USART_Write(USART2, (uint8_t *)">\r\n", 3);
        cancel = 0; //reset cancel flag
        handle_input();
    }
    else if (typo == 1){//x was entered do not process '>' override char written
        USART_Write(USART2, (uint8_t *)"Invalid command seen!\r\n", 23);
        typo = 0; //reset cancel flag
        handle_input();
    }
    else{ //check this condition
        handle_input();
    }
}

int main (void){
    System_Clock_Init();    // System Clock = 80 MHz
    LED_Init();
    UART2_Init();
    init_pa0_pa1();
    //POST(); 
    while (1){
        handle_input();
	}
}
