#ifndef __RECIPE_H
#define __RECIPE_H

#include "timer.h"
#include <string.h>
#include <stdio.h>

#define TIMEOUT (500) 
#define NUMBER_OF_SERVOS (2)
#define DELAY_STEP (100000)
#define MOTOR_DELAY_PER (2)
#define MAX_RECIPE_LENGTH (100)
#define LEFT (6)
#define RIGHT (0)

#define MOV (32)
#define WAIT (64)
#define LOOP (128)
#define END_LOOP (160)
#define RECIPE_END (0)
#define JUMP (96) //Grad extention

// servo function
enum servoStates
{
    moving,
    stopped,
    unknown_state
} ;

// recipe function
enum recipeStates
{
    paused,
    running,
    userinput,
    noop,
    error,
    end
};

// program
enum runStates
{
    starting,
    checkInput,
    readInput,
    runStep,
    delayState,
    done
} ;

// servo info
typedef struct{
    enum servoStates servoState;
    enum recipeStates recipeState;
    uint8_t stepNum;
    uint16_t loopNumber;
    uint8_t delayNeeded;
    uint8_t servoLocation;
    uint8_t loopStart;
    uint8_t loopEnd;
} servoData;

void runRecipes(void);
void processUserInput(uint8_t input, uint8_t servoNumber);
void moveServo(uint8_t destination, uint8_t servo);
void runStepFunc(uint8_t step, uint8_t servo);

#endif
