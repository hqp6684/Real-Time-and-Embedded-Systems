// C pointers light Practice Practicum Solution
// SWEN-250
// Larry Kiser November 29, 2015

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cpractice2.h"
#include "unit_tests.h"

// return a pointer to the character that is at the passed position (position starts at zero).
// return a null if the requested position is past the end of the string.
char *get_pointer_at_position( char *pstring, int position )
{
	int length = strlen( pstring ) ;
	char *presult = NULL ;	// select a smart default to make the code easier
	
	if ( position < length )	// if the position is withing the string then do pointer math to get the pointer to that exact character.
		presult = pstring + position ;
	return presult ;
}

// Convert array of integer x values into an array of y values using y = mx + b where m and b are constants
// passed to the function. The x values will be replaced by the y values (the x values are overwritten).
// The constants can be positive, negative or zero.
// Return the sum of the y values.
// Return 0 if the passed in px pointer is NULL.
int convert_and_sum( int *px, int m, int b, int number_of_x_values )
{
	int result = 0 ;	// pick a default for the failure case (px pointer is null)
	int index ;			// do not need to initialize since it is initialized in the for loop.
	
	if ( px )			// skip everything if px is NULL
		for ( index = 0 ; index < number_of_x_values ; index++ )	// loop through all x values
		{
			*px *= m ;		// multiply one x value by the scale factor m
			*px += b ;		// now add the offset value.
			result +=  *px++ ;	// *px is now the y value. add to the cumulative sum. increment to next pointer locaiton.
		}
	
	return result ;
}

// Determine if two pointers point to the same array of numbers
// return 1 if they do, return 0 otherwise.
// return 0 if either pointer is NULL.
int same_array( int *pfirst, int *psecond )
{
	int result = 0 ;	// pick a convenient default of 0 to handle either pointer being NULL
	
	if ( pfirst && psecond )	// if either pointer is NULL then this will be false and we skip to the return statement.
		if ( pfirst == psecond )	// Do the two pointers contain the same memory address?
			result = 1 ;
	return result ;
}

// The first time this is called return 1.
// The second time this is called return 0.
// Continue this pattern returning 1, then 0, then 1 and so on.
int bool_flip_flop()
{
	static int result = 0 ;	// pick the opposite of the first time return value so we can always use the same code to swap values.
	
	if ( result )		// if this is the second, fourth, sixth, etc time then result will be 1 from the previous call.
		result = 0 ;	// switch to the other value
	else
		result = 1 ;	// if result is 0 make it a 1 (we go through this path the first, third, fifth, etc. times this is called.
	
	return result ;
}

// This function is implemented incorrectly. You need to correct it.
// When fixed it changes the last character in the passed string to 'Z'.
// It returns returns 1 on success.
// It returns 0 on if the passed string is NULL or an empty string.
int fix_bad_code( char *pstring )
{
	// corrected the following to see if pstring is a NULL pointer or if it points to an empty string.
	// Note that the pstring == NULL must be done first.
	// Also, this could be simplified to: if ( ! pstring || ! *pstring )
	if ( pstring == NULL || *pstring == '\0' )
		return 0 ; 
	while ( *pstring++ != '\0' )	// added the ++ to iterate through the string.
		;
	*(pstring - 2) = 'Z' ;	// due to the above loop design we were pointing to one character after the '\0' so we have to backup two positions.
	return 1 ;
}

// value contains a value from a hardware register that has many bit settings.
// bit_position is the value we want selected from value.
// bit_position 0 means get the lowest bit.
// bit_position 1 means get the next lowest bit.
// This function returns 0 if the selected bit is a 0
// It returns 1 if the selected bit is a 1
int get_bit_value( int value, int bit_position )
{
	//return 0 ;
	if ( value & 1 << bit_position )
		return 1 ;
	return 0 ;
}

