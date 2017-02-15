// Z. Weeden Feb. 12, 2017
#include "stm32l476xx.h" //board
#include "SysClock.h"    //Sysclock
#include "LED.h"         //LED
#include "UART.h"        //UART
#include "TIMER.h"       //TIMER
#include "input_pa0_test.h"
#include "binSearch.h"
#include "sort.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLES 1000                //Number of samples to be taken
#define POST_REQ_TIME 100000        //100000 microseconds = 100ms

uint8_t buffer[BufferSize]; 
uint8_t bounds[BufferSize]; 
uint8_t nbounds[BufferSize]; 
char boundBuff[5]; //TEST

char str[] = "POST failed! Pulse not seen in 100ms. Rerun? (Y or N):\r\n";
char defaultBounds[] = "Using default bounds! (950 micro & 1050 micro) Change? (Y or N):\r\n";
char rerun[] = "Rerun? (Y or N):\r\n";
int measurements[SAMPLES];  //initialize array of 1000 elements
int defaultLow = 950;       //Default Low boundary micro
int defaultHigh = 1050;     //Default High boundry micro

int FAIL( void ){
    char rxByte;
    USART_Write(USART2, (uint8_t *)str, strlen(str));
    rxByte = USART_Read(USART2);
    if (rxByte == 'N' || rxByte == 'n'){
        USART_Write(USART2, (uint8_t *)"Exitting\r\n\r\n", 14);
        //FAIL and exit program
        return 0;
    }
    else if (rxByte == 'Y' || rxByte == 'y'){
        USART_Write(USART2, (uint8_t *)"Rerunning POST\r\n\r\n", 20);
        return POST();
    }
    else {
        USART_Write(USART2, (uint8_t *)"Invalid Response\r\n\r\n", 22);
        return FAIL();
    }
}

int POST( void ) {
    int beginPostTime = 0;
    start_timer();          // start capture
    beginPostTime = timer_now();
    while( 1 ){
        if ( is_event() ) { // received edge TIM2->CCR1
            if ( ( timer_now() - beginPostTime ) <= POST_REQ_TIME ){ // test received an edge before 100000micro (100ms)
                //DISPLAY success on UART/Time seen
                USART_Write(USART2, (uint8_t *)"POST passed!\r\n\r\n", 18);
                stop_timer();
                return 1;
            }
            else{ // test received edge but too long
                //TEST FAILED!
                //OPTION TO RUN AGAIN
                USART_Write(USART2, (uint8_t *)"POST failed!\r\n\r\n", 17);
                stop_timer();
                return FAIL();
            }
        }
        //else { // nothing seen yet TIM2->CNT - ADJUST for no sig on pin
        if ( ( timer_now() - beginPostTime ) > POST_REQ_TIME ){ // exceeded 100ms 
            //TEST FAILED!
            //OPTION TO RUN AGAIN
            USART_Write(USART2, (uint8_t *)"POST failed!\r\n\r\n", 17);
            stop_timer();
            return FAIL();
        //  }
        }
    }
}

void run( void ){
    char rxByte;
    int n;
    int index=0;
    int numOfSample = 0;
    int beginSampleTime = 0;
    defaultLow = 950;
    defaultHigh = 1050;
    USART_Write(USART2, (uint8_t *)defaultBounds, strlen(defaultBounds));
    rxByte = USART_Read(USART2);
    if (rxByte == 'N' || rxByte == 'n'){//print with bounds, use var in write method
        n = sprintf((char *)bounds, "Running with [%d] and [%d]\r\n\r\n", defaultLow, defaultHigh);
        USART_Write(USART2, bounds, n); 
    }
    else if (rxByte == 'Y' || rxByte == 'y'){
        USART_Write(USART2, (uint8_t *)"Changing Bounds\r\n", 17);
        USART_Write(USART2, (uint8_t *)"Enter New Lower Bound between 50 and 9950: ", 43);

        rxByte = USART_Read(USART2);
        while((rxByte != 0xD)){
            memset( buffer, '\0', sizeof(buffer));
            sprintf((char *)buffer, "%c", rxByte);
            USART_Write(USART2, buffer, sizeof(buffer));
            boundBuff[index] = rxByte;
            index++;
            rxByte = USART_Read(USART2);
        }
        sscanf(boundBuff, "%d", &defaultLow);

        if ( defaultLow <= 50 || defaultLow >=9950 ) {
            USART_Write(USART2, (uint8_t *)"\r\nPlease enter a number between 50 and 9950\r\n\r\n", 47);
            run();
        }
        defaultHigh = defaultLow + 100;
        n = sprintf((char *)nbounds, "\r\nNow running with [%d] and [%d]\r\n\r\n", defaultLow, defaultHigh);
        USART_Write(USART2, nbounds, n); 
        //Display new bounds and wait for enter
    }
    else {
        USART_Write(USART2, (uint8_t *)"Invalid Response\r\n\r\n", 22);
        run();
    }

    //capture 1000 pulses
    //Return time it took - store in array in main
    for ( numOfSample = 0; numOfSample < SAMPLES; numOfSample++ ){//detect pulse and duration (multiply by 2 for total period?)
        start_timer();
        beginSampleTime = timer_now();
        while( 1 ){
            if ( is_event() ){
                measurements[numOfSample] = (timer_now() - beginSampleTime); //(possibly multiplied by 2)
                stop_timer(); // might need to reset
                break;
            }
            else{//edge not detected
                ;
            }
        }
    }
}

void UART_graph( void ){
    int size = sizeof(measurements)/sizeof(measurements[0]);
    int x; //individual element of the array for a pass
    int c; //count of the individual element
    int p; //counter for the measurements loop
    int n;
    int TEST://
    sort_array(measurements);
    for (TEST=0;TEST<SAMPLES;TEST++){
        n = sprintf((char *)buffer, "%d\r\n", measurements[TEST]);
        USART_Write(USART2, buffer, n);
    }


    USART_Write(USART2, (uint8_t *)"Number || Tally\r\n", 17);
    USART_Write(USART2, (uint8_t *)"===============\r\n\r\n", 19);
    
    for ( p = 0; p < size; p++ )
    {
        c = 0;
        x = measurements[p]; // Element to be counted in measurements[]
        if ( x >= defaultLow && x <= defaultHigh ){//within defined bounds
            if ( p == 0 ){//first element
                c = count(measurements, x, size);
                n = sprintf((char *)buffer, "%d || %d\r\n", x, c);
                USART_Write(USART2, buffer, n); 
            }
            else{//not first element
                if ( x == measurements[p-1] ){//current element is the same as the last element //make sure there is no other occurence of the element in the entire array noit just the previous element
                    ;
                }
                else{
                    c = count(measurements, x, size);
                    n = sprintf((char *)buffer, "%d || %d\r\n", x, c);
                    USART_Write(USART2, buffer, n);
                }
            }
        }
        else{//measure time out of bounds, do not show
            ;
        }
    }
}

int rerunFunc( void ){
    char rxByte;
    USART_Write(USART2, (uint8_t *)rerun, strlen(rerun));
    rxByte = USART_Read(USART2);
    if (rxByte == 'N' || rxByte == 'n'){
        USART_Write(USART2, (uint8_t *)"Exitting\r\n\r\n", 14);
        return 0;
    }
    else if (rxByte == 'Y' || rxByte == 'y'){
        USART_Write(USART2, (uint8_t *)"Rerunning Program\r\n\r\n", 23);
        return 1;
    }
    else {
        USART_Write(USART2, (uint8_t *)"Invalid Response\r\n\r\n", 22);
        return rerunFunc();
    }
}

int main( void )
{
    int pass;
    int rerunProgram;
    System_Clock_Init();    // System Clock = 80 MHz
    LED_Init();
    UART2_Init();
    init_pa0();
    
    //Implement indefinite loop here
    pass = POST();          // Power On Self Test
    if ( pass == 1 ){
        // POST PASSED! - continue operation
        // display default bounds and prompt user for upper and lower bounds
        // 50micro<=lower<-9950micro
        // upper >=lower + 100micro
        while( 1 ){
            run(); //store return value into (global) array
            UART_graph();
            //prompt for rerun
            rerunProgram=rerunFunc();
            if ( rerunProgram == 1 ){//Choose to rerun
                ;
            }
            else{//Exit program
                break;
            }
            
        }
    }
    else{
        //RECEIVED SOMETHING NOT 1
        ;
    }
    return EXIT_SUCCESS;
}
