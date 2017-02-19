// L. Kiser Feb. 16, 2017
// Some ideas for writing the SWEN 563 recipe related code.

#include <stdio.h>
#include <stdlib.h>

// Define all of the commands that are valid
#define MOV (0x20) //001x xxxx
#define WAIT (0x40)//010x xxxx
#define LOOP (0x80)//100x xxxx
#define END_LOOP (0xA0)//101x xxxx

#define RECIPE_END (0)

// Examples of simple recipes
// Note that, because the bottom 5 bits are zeros adding or bitwise or'ing
// in the values for the bottom 5 bits are equivalent. However, using a bitwise
// or is better at communicating your purpose.
unsigned char recipe1[] = { MOV + 3, MOV | 5, RECIPE_END } ;
unsigned char recipe2[] = { MOV | 5, MOV | 2, RECIPE_END } ;

// If you set up an array like this then you can easily switch recipes
// using an additional user input command.
unsigned char *recipes[] = { recipe1, recipe2, NULL } ;

// This is a good way to define the states of a state machine.
enum status 
	{
		running,
		paused,
		command_error,
		nested_error 
	} ;

// A simple main that just prints out the hex value of the first entry in each recipe.
void main()
{
	int i = 0 ;

	while ( recipes[ i ] )
	{
		printf( "%02x\n", *recipes[ i++ ] ) ;	// printf first hex entry of each recipe

	}
}
