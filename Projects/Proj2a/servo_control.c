// Include all required headers
#include "stm32l476xx.h"

// Define all of the commands that are valid
#define MOV (0x20)      //001x xxxx
#define WAIT (0x40)     //010x xxxx
#define LOOP (0x80)     //100x xxxx
#define END_LOOP (0xA0) //101x xxxx
#define RECIPE_END (0)
#define PARAM_BITMASK (31)	//0001 1111
#define OP_BITMASK (224)	//1110 0000

void mnemonicControl(int command, int motor) {
	int commandOpCode = command & OP_BITMASK;
	int commandParam = command & PARAM_BITMASK;
	
	switch (commandOpCode) {
		case MOV: 
			moveMotor(commandParam, motor);
			break;
		case WAIT: 		
			waitMotor(commandParam, motor);
			break;
		case LOOP: 			
			loop(commandParam, motor);
			break;
		case END_LOOP: 
			endLoop(motor);
			break;
		case RECIPE_END:
			recipeEnd(motor);
			break;
		default: //not valid
            notValidMnemonic();
	}
} 

void moveMotor(int position, int motor) {
	if (motor==0) { 
		switch (position) {
			case 0:
				TIM2->CCR1 = 4;		// .388ms + (((1.264-.388)/3)n)
				break;
			case 1:
				TIM2->CCR1 = 7;		// .68ms
				break;
			case 2:
				TIM2->CCR1 = 10;	// .97ms
				break;
			case 3:
				TIM2->CCR1 = 13;	// 1.264ms
				break;
			case 4:
				TIM2->CCR1 = 17;	// 1.702ms
				break;
			case 5:
				TIM2->CCR1 = 21;	// 2.14ms
				break;
			default: //not valid - produce error - error state -> continue override command
            	notValidPosition();
		}
	} 
	else {
		switch (position) {
			case 0:
				TIM2->CCR2 = 4;		// .4ms
				break;
			case 1:
				TIM2->CCR2 = 7;		// .68ms
				break;
			case 2:
				TIM2->CCR2 = 10;	// .97ms
				break;
			case 3:
				TIM2->CCR2 = 13;	// 1.264ms
				break;
			case 4:
				TIM2->CCR2 = 17;	// 1.702ms
				break;
			case 5:
				TIM2->CCR2 = 21;	// 2.14ms
				break;
			default: //not valid - produce error - error state -> continue override command
            	notValidPosition();
		}
	}
}
