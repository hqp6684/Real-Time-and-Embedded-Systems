// Z. Weeden Feb. 18, 2017
#include "stm32l476xx.h" // Board
#include "SysClock.h"    // Sysclock
#include "LED.h"         // LED
#include "UART.h"        // UART
#include "TIMER.h"       // TIMER
#include "input_pa0_pa1_test.h"
#include "motor_control.h"
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

void parse_override_cmd(char *commandArray){
    switch(commandArray[0]){ //motor1Cmd
        case 'P':
            pauseMotor1();
            break;
        case 'p':
            pauseMotor1();
            break;
        case 'C':
            continueMotor1();
            break;
        case 'c':
            continueMotor1();
            break;
        case 'R':
            servo_motor1_right();
            break;
        case 'r':
            servo_motor1_right();
            break;
        case 'L':
            servo_motor1_left();
            break;
        case 'l':
            servo_motor1_left();
            break;
        case 'N': //pass - no op
            break;
        case 'n': //pass - no op
            break;
        case 'B':
            restartMotor1();
            break;
        case 'b':
            restartMotor1();
            break;
    }
    switch(commandArray[1]){ //motor2Cmd
        case 'P':
            pauseMotor2();
            break;
        case 'p':
            pauseMotor2();
            break;
        case 'C':
            continueMotor2();
            break;
        case 'c':
            continueMotor2();
            break;
        case 'R':
            servo_motor2_right();
            break;
        case 'r':
            servo_motor2_right();
            break;
        case 'L':
            servo_motor2_left();
            break;
        case 'l':
            servo_motor2_left();
            break;
        case 'N': //pass - no op
            break;
        case 'n': //pass - no op
            break;
        case 'B':
            restartMotor2();
            break;
        case 'b':
            restartMotor2();
            break;
    }
}

void handle_input(void){ // Maybe return array of the input or individually parsed chars for motor1cmd motor2cmd 
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
        if (rxByte == 'X' || rxByte == 'x'){
            cancel = 1;
            break;
        }
        if (rxByte != 'P' || rxByte != 'p' || rxByte != 'C' || rxByte != 'c' || rxByte != 'R' || rxByte != 'r' || rxByte != 'L' || rxByte != 'l' || rxByte != 'N' || rxByte != 'n' || rxByte != 'B' || rxByte != 'b'){
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
    if (cancel == 0 && typo == 0){ // Maybe return overrideCmd array
        sscanf(overrideCmd[0], "%c", &motor1Cmd); // UNUSED
        sscanf(overrideCmd[1], "%c", &motor2Cmd); // UNUSED      
    }
    else if (cancel == 1){// x was entered do not process '>' override char written
        USART_Write(USART2, (uint8_t *)">\r\n", 3);
        cancel = 0; // Reset cancel flag
        handle_input();
    }
    else if (typo == 1){// x was entered do not process '>' override char written
        USART_Write(USART2, (uint8_t *)"Invalid command seen!\r\n", 23);
        typo = 0; // Reset cancel flag
        handle_input();
    }
    else{ //check this condition
        handle_input();
    }
    parse_override_cmd(overrideCmd); // Need interrupt  
}

int main (void){
    System_Clock_Init();    // System Clock = 80 MHz
    LED_Init();
    UART2_Init();
    init_pa0_pa1();
    //POST(); 
    //mnemonicControl(35, 0);  // example (MOV) 0010 (3) 0011 motor0
    //mnemonicControl(34, 1);  // example (MOV) 0010 (2) 0010 motor1
    while (1){
        handle_input();
    }
}
