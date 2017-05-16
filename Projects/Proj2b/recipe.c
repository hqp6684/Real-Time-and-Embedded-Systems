#include "recipe.h"
servoData servos[NUMBER_OF_SERVOS];
enum runStates runState = start;
enum recipeStates userInput[NUMBER_OF_SERVOS];
char space[1000];
int input_character = 0;
int input_fed;
uint32_t location[7] = {488000, 788000, 1000000, 1264000, 1604000, 1804000, 2140000};

//recipe to be run
uint8_t recipeArray[2][100] = {
    {
        MOV+3,
        MOV+0,
        WAIT+31,
        WAIT+31,
        WAIT+31,
        MOV+4,
    },
    {
        MOV+2,
        MOV+1,
        MOV+5,
        MOV+3,
        LOOP+3,
        MOV+1,
        MOV+4,
        END_LOOP,
        MOV+0,
        MOV+2,
        WAIT+0,
        MOV+3,
        WAIT+2,
        MOV+2,
        MOV+3,
        WAIT+31,
        WAIT+31,
        WAIT+31,
        MOV+4,
    }
};

/* This function is responsible for the determination of the state of the program's nature
   with various cases to determine if we are running a recipe or reading user input etc. 
   There is also handling for the 'x' character to abort and restart a fresh line feed for 
   user input.   */
void runRecipes(){
    int lcv;
    uint8_t input;
    pthread_create (NULL, NULL, nBGet, NULL);
    printf("Running Recipe : currently Paused... \r\n");
    printf(">");
    fflush(stdout);
    while(runState != done){
        switch(runState){
            case start: //initialize variables
                for(lcv = 0; lcv<NUMBER_OF_SERVOS; lcv++){
                    servos[lcv].servoState = unknown_state;
                    servos[lcv].recipeState = paused;
                    servos[lcv].stepNumber = 0;
                    servos[lcv].loopNumber = 0;
                    servos[lcv].servoLocation = 0;
                }
                runState = checkInput;
                break;
            case checkInput : //nonblocking input check
                if(input_fed){ // make sure the input is a letter
                        runState = readInput;
                } else{
                    runState = runStep;
                }
                break;
            case readInput: //start blocking input
                if(space[1] == 'x' || space[1] == 'X' || space[0] == 'x' || space[0] == 'X'){
                    printf("\r\n>");
                    fflush(stdout);
                    input = 0xFF;
                }else{
                    printf("\r\n>");
                    fflush(stdout);
                    for(lcv = 0; lcv<NUMBER_OF_SERVOS; lcv++){ //for each servo
                        userInput[lcv] = servos[lcv].recipeState;
                        servos[lcv].recipeState = userinput;
                    }
                }
                input_fed = 0;
                runState = runStep;
                break;
            case runStep:
                for(lcv = 0; lcv<NUMBER_OF_SERVOS; lcv++){ //for each servo
                    if(servos[lcv].recipeState == noop){
                        //if noop required
                        servos[lcv].recipeState = userInput[lcv];
                    }else if(servos[lcv].recipeState == running){
                        //if running as usual
                        
                        if(servos[lcv].delayNeeded <= 0){
                            servos[lcv].servoState = stopped;
                            //if the servo has reached its location, run next step
                            runStepFunc(recipeArray[lcv][servos[lcv].stepNumber], lcv);
                            servos[lcv].stepNumber++; //incriment Program counter
                        }
                        
                    }else if(servos[lcv].recipeState == userinput){
                        //if there has been user input
                        processUserInput(space[lcv], lcv);
                    }else {
                        //if paused
                        // do nothing
                    }
                }
                runState = delayState;
                break;
            case delayState:
                //process necessary noop delay required for motor movement
                for(lcv = 0; lcv<NUMBER_OF_SERVOS; lcv++){//for each servo
                    if(servos[lcv].delayNeeded> 0){
                        //if there needs to be a delay, delay and decrement the delay car
                        //USART_Delay(DELAY_STEP);
                        usleep(DELAY_STEP/2);
                        servos[lcv].delayNeeded--;
                    }
                }
                runState = checkInput;
                break;
            case done:
                break;
        }
    }
}

/* This is a thread function that is called to check if the input to the UART is \n 
   in which a flag is set that is used in the above function to break and progress 
   to the input processing. */
void * nBGet(){
    int c;
    //pthread_setschedprio(pthread_self(), 5 );
    while(1){
        int cnt = 0;
        while((c=getchar())!= '\n') {
            space[cnt] = c;
            cnt++;
        }
        input_fed = 1;
        //sleep(1);
    }
}

/* Simply prints a single character using print buffer */
void printChar(uint8_t character){
    printf( "%c", character);
    fflush(stdout);
}

/* This function is responsible for processing the input the user entered from the input field.
   Various structs are updated accordingly to the processed data. */
void processUserInput(uint8_t input, uint8_t servoNumber){
    if(input == 'P' || input == 'p'){
        //pause
        if(userInput[servoNumber] != end && userInput[servoNumber] != error){
            servos[servoNumber].recipeState = paused;
        }
    } else if (input == 'C' || input == 'c'){
        //continue
        if(userInput[servoNumber] != end && userInput[servoNumber] != error){
            servos[servoNumber].recipeState = running;
        }
    } else if (input == 'R' || input == 'r'){
        //move servo right
        if(servos[servoNumber].servoLocation > RIGHT && userInput[servoNumber] == paused ){
            servos[servoNumber].servoLocation--;
            moveServo(servos[servoNumber].servoLocation, servoNumber);
            servos[servoNumber].recipeState = paused;
        }else {
            servos[servoNumber].recipeState = paused;
        }
    }   else if (input == 'L' || input == 'l'){
        //move servo left
        if(servos[servoNumber].servoLocation < LEFT && userInput[servoNumber] == paused ){
            servos[servoNumber].servoLocation++;
            moveServo(servos[servoNumber].servoLocation, servoNumber);
            servos[servoNumber].recipeState = paused;
        }else{
            servos[servoNumber].recipeState = paused;
        }
    }   else if (input == 'N' || input == 'n'){
        //No operation
        servos[servoNumber].recipeState = noop;
    } else if (input == 'B' || input == 'b'){
        //restart
        servos[servoNumber].stepNumber = 0;
        servos[servoNumber].recipeState = running;
    } else {
        //error
    }
}

/* Responsible for moving servo as well as conditional checking of position and updates to the struct. */
void moveServo(uint8_t destination, uint8_t servo){
    //if within bounds move servo.
    if(destination> servos[servo].servoLocation){ // absolute value delay needed
        servos[servo].delayNeeded = (destination - servos[servo].servoLocation)  * MOTOR_DELAY_PER;
    } else{
        servos[servo].delayNeeded = (servos[servo].servoLocation - destination) * MOTOR_DELAY_PER;
    }
    servos[servo].servoLocation = destination;
    servos[servo].servoState = moving;
    changePWM(servo, location[destination]);
}

/* Determination of the current element in the recipe snippet array and its proper operation */
void runStepFunc(uint8_t step, uint8_t servo){
    uint8_t op = step >> 5;
    uint8_t value = step & ~(0xE0);
    
    if(op == 0x1){
        //MOV
        if(value>5){
            servos[servo].recipeState = error;
        }else{
            moveServo(value, servo);
        }
    } else if(op == 0x2){
        //WAIT
        if(value>31){
            servos[servo].recipeState = error;
        }else{
            servos[servo].delayNeeded = value + 1;
        }
    }else if(op == 0x3){
        //GOTO 
        //Moves to step provided by value
        servos[servo].stepNumber = value;
        servos[servo].recipeState = running;
    }else if(op == 0x4){
        //LOOP
        servos[servo].loopNumber = value;
        servos[servo].loopStart = servos[servo].stepNumber;
    }else if(op == 0x5){
        if (servos[servo].loopNumber > 0){
            servos[servo].loopNumber--;
            servos[servo].stepNumber = servos[servo].loopStart;
        } 
    }else if(op == 0x0){
        //RECIPE_END
        servos[servo].recipeState = end;
    } else{
        servos[servo].recipeState = error;
    }
}
