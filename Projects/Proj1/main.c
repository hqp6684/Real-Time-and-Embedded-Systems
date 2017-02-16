// Z. Weeden Feb. 12, 2017
#include "stm32l476xx.h" // Board
#include "SysClock.h"    // Sysclock
#include "LED.h"         // LED
#include "UART.h"        // UART
#include "TIMER.h"       // TIMER
#include "input_pa0_test.h"
#include "binSearch.h"
#include "sort.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLES 1000                // Number of samples to be taken
#define POST_REQ_TIME 100000        // 100000 microseconds = 100ms

uint8_t buffer[BufferSize]; 
uint8_t bounds[BufferSize]; 
uint8_t nbounds[BufferSize]; 
char boundBuff[5]; // User input number buffer space

char str[] = "POST failed! Pulse not seen in 100ms. Rerun? (Y or N):\r\n";
char defaultBounds[] = "Using default bounds! (950 micro & 1050 micro) Change? (Y or N):\r\n";
char rerun[] = "Rerun? (Y or N):\r\n";
int measurements[SAMPLES];  // Initialize array of 1000 elements
int defaultLow = 950;       // Default Low boundary micro
int defaultHigh = 1050;     // Default High boundry micro

/* This function is called when the POST fails. It prompts the user if they wish to rerun the POST.
The output of this function is either 0 or 1, implicity from the rerun POST function. This is used 
in a conditional in main. */
int FAIL( void ){
    char rxByte;
    USART_Write(USART2, (uint8_t *)str, strlen(str));
    rxByte = USART_Read(USART2);
    if (rxByte == 'N' || rxByte == 'n'){ // User wants to quit
        USART_Write(USART2, (uint8_t *)"Exitting\r\n\r\n", 14);
        return 0;
    }
    else if (rxByte == 'Y' || rxByte == 'y'){ // User wants to rerun
        USART_Write(USART2, (uint8_t *)"Rerunning POST\r\n\r\n", 20);
        return POST();
    }
    else { // User entered neither 'Y'/'y' or 'N'/'n'
        USART_Write(USART2, (uint8_t *)"Invalid Response\r\n\r\n", 22);
        return FAIL();
    }
}
/* This is the POST function. This checks to see if an event/pulse was seen within the alloted 
100 milliseconds. If an event is seen the POST has passed and returns a 1. If a pulse is not seen
in 100 milliseconds, the implicit return of the called FAIL function is returned. */
int POST( void ) {
    int beginPostTime = 0;
    start_timer();
    beginPostTime = timer_now();
    while( 1 ){
        if ( is_event() ) { // Event seen!
            if ( ( timer_now() - beginPostTime ) <= POST_REQ_TIME ){ // Received an edge before 100000 microseconds (100ms) - Pass!
                USART_Write(USART2, (uint8_t *)"POST passed!\r\n\r\n", 18);
                stop_timer();
                return 1;
            }
            else{ // Test received edge but failed time requirements - Fail!
                USART_Write(USART2, (uint8_t *)"POST failed!\r\n\r\n", 17);
                stop_timer();
                return FAIL();
            }
        }
        if ( ( timer_now() - beginPostTime ) > POST_REQ_TIME ){ // No event seen and elapsed time has exceeded 100ms - Fail!
            USART_Write(USART2, (uint8_t *)"POST failed!\r\n\r\n", 17);
            stop_timer();
            return FAIL();
        }
    }
}
/* This function is the acutal sampling of measurements. Nothing is returned but rather measurements 
are stored in an array for further use. User is prompted whether they should change bounds. An array 
was then filled with null terminators to act as padding/breakpoints of input. After bounds 
are set, 1000 samples of event captures are taken with each result stored in an array. */
void run( void ){
    int j;
    char rxByte;
    int n;
    int index = 0;
    int numOfSample = 0;
    int beginSampleTime = 0;
    defaultLow = 950;
    defaultHigh = 1050;
    USART_Write(USART2, (uint8_t *)defaultBounds, strlen(defaultBounds));
    rxByte = USART_Read(USART2);
    if (rxByte == 'N' || rxByte == 'n'){ // User wants default bounds
        n = sprintf((char *)bounds, "Running with [%d] and [%d]\r\n\r\n", defaultLow, defaultHigh);
        USART_Write(USART2, bounds, n); 
    }
    else if (rxByte == 'Y' || rxByte == 'y'){ //
        USART_Write(USART2, (uint8_t *)"Changing Bounds\r\n", 17);
        USART_Write(USART2, (uint8_t *)"Enter New Lower Bound between 50 and 9950: ", 43);
        rxByte = USART_Read(USART2);
        for (j = 0; j < sizeof(boundBuff)/sizeof(boundBuff[0]); j++){ // Populate array with null terminators
            boundBuff[j] = '\0';
        }
        while(rxByte != 0xD){ // No carriage return seen - oxD is hex ascii equivalent of \r - keep appending input to buffer
            memset( buffer, '\0', sizeof(buffer)); // Null terminator to indicate end of string
            sprintf((char *)buffer, "%c", rxByte);
            USART_Write(USART2, buffer, sizeof(buffer));
            boundBuff[index] = rxByte;
            index++;
            rxByte = USART_Read(USART2);
        }
        sscanf(boundBuff, "%d", &defaultLow);
        if ( defaultLow < 50 || defaultLow > 9950 ) { // Ensure that the new lower bound satisfies requirements (between 50 and 9950 micro)
            USART_Write(USART2, (uint8_t *)"\r\nPlease enter a number between 50 and 9950\r\n\r\n", 47);
            run();
        }
        defaultHigh = defaultLow + 100; // Upper bound must be 100 more than the lower bound
        n = sprintf((char *)nbounds, "\r\nNow running with [%d] and [%d]\r\n\r\n", defaultLow, defaultHigh);
        USART_Write(USART2, nbounds, n); 
    }
    else {
        USART_Write(USART2, (uint8_t *)"Invalid Response\r\n\r\n", 22);
        run();
    }
    for ( numOfSample = 0; numOfSample < SAMPLES; numOfSample++ ){ // Take 1000 measurements - store in array
        start_timer();
        beginSampleTime = timer_now();
        while( 1 ){
            if ( is_event() ){ // Event seen!
                measurements[numOfSample] = (timer_now() - beginSampleTime);
                stop_timer();
                break;
            }
            else{ // Edge not detected - sample not taken continue
                ;
            }
        }
    }
}
/* This function shows the result on the UART. The measurement array is sorted and then traversed to enumerate
each element. The function then goes on to ensure that duplicate entries as well as entries out of the defined 
bounds are not shown on the UART. */
void UART_graph( void ){
    int sizeOfArray = sizeof(measurements)/sizeof(measurements[0]); // Size of array
    int individualSample; // Individual element of the array for a pass
    int countOfSample;    // Count of the individual element
    int i;                // Counter for the measurements loop
    int n;
    sort_array(measurements); // Sort array in ascending order
    USART_Write(USART2, (uint8_t *)"Number || Tally\r\n", 17);
    USART_Write(USART2, (uint8_t *)"===============\r\n\r\n", 19);
    for ( i = 0; i < sizeOfArray; i++ ){ // Loop through entire array and print out sample and the enumeration of the element
        countOfSample = 0;
        individualSample = measurements[i]; // Element to be counted in measurements[]
        if ( individualSample >= defaultLow && individualSample <= defaultHigh ){ // Sample is within defined bounds
            if ( i == 0 ){ // First element in array
                countOfSample = count(measurements, individualSample, sizeOfArray);
                n = sprintf((char *)buffer, "%d || %d\r\n", individualSample, countOfSample);
                USART_Write(USART2, buffer, n); 
            }
            else{ // Not first element in array
                if ( individualSample == measurements[i-1] ){ // Current element is the same as the last element in sorted array - duplicate element - skip display
                    ;
                }
                else{ // Current element has not occurred before - display
                    countOfSample = count(measurements, individualSample, sizeOfArray);
                    n = sprintf((char *)buffer, "%d || %d\r\n", individualSample, countOfSample);
                    USART_Write(USART2, buffer, n);
                }
            }
        }
        else{ // Sample does not meet bound requirements - do not show
            ;
        }
    }
}
/* This function is called when the program has completed 1 successful pass of 1000 measurements and UART display, 
bypassing the POST. The user is prompted whether they wish to rerun. Depending on the choice either a 0 or 1 is returned;
the decision to terminate or to rerun respectively. */
int rerunFunc( void ){
    char rxByte;
    USART_Write(USART2, (uint8_t *)rerun, strlen(rerun));
    rxByte = USART_Read(USART2);
    if (rxByte == 'N' || rxByte == 'n'){ // User wants to quit
        USART_Write(USART2, (uint8_t *)"Exitting\r\n\r\n", 14);
        return 0;
    }
    else if (rxByte == 'Y' || rxByte == 'y'){ // User wants to rerun
        USART_Write(USART2, (uint8_t *)"Rerunning Program\r\n\r\n", 23);
        return 1;
    }
    else { // User entered neither 'Y'/'y' or 'N'/'n'
        USART_Write(USART2, (uint8_t *)"Invalid Response\r\n\r\n", 22);
        return rerunFunc();
    }
}
/* Main - controls the top level flow of the program. Returns an int (EXIT_SUCCESS). Initializes various components 
and uses returned values of functions to control flow of the program. An indefinite loop allows the user to rerun the
program as many times as they wish. */
int main( void ){
    int pass;
    int rerunProgram;
    System_Clock_Init();    // System Clock = 80 MHz
    LED_Init();
    UART2_Init();
    init_pa0();
    pass = POST(); // Power On Self Test return value stored
    if ( pass == 1 ){ // POST passed - continue operation
        while( 1 ){ // Core program indefinite loop
            run();
            UART_graph();
            rerunProgram=rerunFunc(); // Prompt for rerun
            if ( rerunProgram == 1 ){ // User chose to rerun
                ;
            }
            else{ // Exit program
                break;
            }
        }
    }
    else{ // Received something other than 1 from POST
        ;
    }
    return EXIT_SUCCESS;
}
