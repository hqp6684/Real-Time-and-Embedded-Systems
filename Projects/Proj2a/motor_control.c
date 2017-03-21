
/* 
 * File:   motor_control.c
 * Author: dinesh
 * Contributor: Zachary Weeden
 * Created on March 2, 2017, 1:30 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "stm32l476xx.h"

/*This part defines the 5 different duty cycles which defines the 5 different position of servo motors*/
/********************************************************/
#define Duty_cycle_0 (4)  // replace the value of xxxx with 5 different duty cycles in the 20ms range
#define Duty_cycle_1 (7) 
#define Duty_cycle_2 (10) 
#define Duty_cycle_3 (13) 
#define Duty_cycle_4 (17) 
#define Duty_cycle_5 (21) 

/********************************************************/
/*setting the duty cycles to the server motor 1 and 2 */
/***********************************************************/

void motor1_position0 ( void)
{
    TIM2 -> CCR1 = Duty_cycle_0; 
}

void motor1_position1 ( void)
{
    TIM2 -> CCR1 = Duty_cycle1; 
}

void motor1_position2 ( void)
{
    TIM2 -> CCR1 = Duty_cycle2; 
}

void motor1_position3 ( void)
{
    TIM2 -> CCR1 = Duty_cycle3; 
}

void motor1_position4 ( void)
{
    TIM2 -> CCR1 = Duty_cycle4; 
}

void motor1_position5 ( void)
{
    TIM2 -> CCR1 = Duty_cycle5; 
}

void motor2_position0 ( void)
{
    TIM2 -> CCR2 = Duty_cycle_0; 
}

void motor2_position1 ( void)
{
    TIM2 -> CCR2 = Duty_cycle1; 
}

void motor2_position2 ( void)
{
    TIM2 -> CCR2 = Duty_cycle2; 
}

void motor2_position3 ( void)
{
    TIM2 -> CCR2 = Duty_cycle3; 
}

void motor2_position4 ( void)
{
    TIM2 -> CCR2 = Duty_cycle4; 
}

void motor2_position5 ( void)
{
    TIM2 -> CCR2 = Duty_cycle5; 
}


/*************************************************************/
/* setting up the position of the servo motor 1 and 2  to the a unsigned character */
/***************************************/
// void Currentposition ( uint8_t position1, uint8_t position 2);      // defined current position of servo motors 1 and 2 
void position_setup_motor1 ( uint8_t position );
{
   uint8_t Position_motor1;       // char that holds the current position of the motor 1
   switch (position) {
	   case 0: 
        motor1_position0();
        Position_motor1 =0; 
        break ; 
	   
       case 1: 
        motor1_position1();
        Position_motor1 =1; 
        break ; 
       
       case 2: 
        motor1_position2();
        Position_motor1 =2; 
        break ;  
       
       case 3: 
        motor1_position3();
        Position_motor1 =3; 
        break ; 
       
       case 4: 
        motor1_position4();
        Position_motor1 =4; 
        break ;  
       
        case 5: 
         motor1_position5();
         Position_motor1 =5; 
         break ; 
   }
}


void position_setup_motor2 ( uint8_t position );
{
   uint8_t Position_motor2;   // char that holds the current position of the motor 2
   switch (position) {
	   case 0: 
        motor2_position0();
        Position_motor2 =0; 
        break ;
	   
       case 1: 
        motor2_position1();
        Position_motor2 =1; 
        break ; 
       
       case 2: 
        motor2_position2();
        Position_motor2 =2; 
        break ;  
       
       case 3: 
        motor2_position3();
        Position_motor2 =3; 
        break ; 
       
       case 4: 
        motor2_position4();
        Position_motor2 =4; 
        break ;  
       
        case 5: 
         motor2_position5();
         Position_motor2 =5; 
         break ;        
   }
}

/*****************************************/
/* Controlling the position of the servo motor left and right*/
// just calling this structure for the user defined action, will move the servo motor accordingly
/*******************************************/
void servo_motor1_left ( void )
{
   if (Position_motor1 != 0) 
   {
       Position_motor1 = Position_motor1 - 1 ; 
   }
}

void servo_motor1_right ( void )
{
   if (Position_motor1 != 5) 
   {
       Position_motor1 = Position_motor1 + 1 ; 
   }
}


void servo_motor2_left ( void )
{
   if (Position_motor1 != 0) 
   {
       Position_motor1 = Position_motor1 - 1 ; 
   }
}

void servo_motor2_right ( void )
{
   if (Position_motor1 != 5) 
   {
       Position_motor1 = Position_motor1 + 1 ; 
   }
}
/*********************************************/
