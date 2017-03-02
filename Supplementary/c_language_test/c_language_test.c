// C language self test
// SWEN-563
// Zachary Weeden Jan. 30, 2017

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "c_language_test.h"
#include "unit_tests.h"

// return a pointer to the character that is at the passed position.
// return a null if the requested position is past the end of the string.
char *get_pointer_at_position( char *pstring, int position )
{
	// Your code here
	int len1;
	len1=strlen(pstring); //get length of string
	if(position>len1-1){ //index number is greater than whole string length (out of bounds)
		return NULL;
	}
	else{
		return (pstring+position);
	}
}

// Convert array of integer x values into an array of y values using y = mx + b where m and b are constants
// passed to the function. The x values will be replaced by the y values (the x values are overwritten).
// The constants can be positive, negative or zero.
// Return the sum of the y values.
// Return 0 if the passed in px pointer is NULL.
int convert_and_sum( int *px, int m, int b, int number_of_x_values )
{
	// your code here
	if (px){//pointer is valid
		int item;
		int sumOfY=0;
		for(item=0;item<number_of_x_values;item++){
			sumOfY=sumOfY+(px[item]*m)-1;//running total of sum after operation on index 
			*(px+item)=(px[item]*m)-1;//alter contents of index post sum 
		}
		return sumOfY;
	}
	else {//px=0, NULL
		return 0 ;
	}
}

// Determine if two pointers point to the same array of numbers
// return 1 if they do, return 0 otherwise.
// return 0 if either pointer is NULL.
int same_array( int *pfirst, int *psecond )
{
	// your code here
	if (pfirst && psecond){//valid pointers 
		if (pfirst == psecond){//mem address much match - same location
			return 1;
		}
		else{//dont match
			return 0;
		}
	}
	else{//one of the pointers=0,NULL
		return 0 ;
	}
}

// The first time this is called return 1.
// The second time this is called return 0.
// Continue this pattern returning 1, then 0, then 1 and so on.
int bool_flip_flop()
{
	// your code here
	static int timesCalled=0;//initialize
	timesCalled=timesCalled+1;//tally times called
	if (timesCalled%2==1){//modulo to determin if even or odd times called, first returns 1 (return 1 every odd time called)
		return 1;
	}
	else{//even time called return 0
		return 0;
	}
}

// This function is implemented incorrectly. You need to correct it.
// When fixed it changes the last character in the passed string to 'Z'.
// It returns returns 1 on success.
// It returns 0 on if the passed string is NULL or an empty string.
int fix_bad_code( char *pstring )
{
	int len1;
	if ( !pstring || pstring[0]=='\0'){//address is NULL or contents of string are empty (NULL Terminator)
		return 0 ; 
	}
	else{//valid address and string is not empty
		len1=strlen(pstring);//gets length of string
		*(pstring+(len1-1))='Z'; //contents of last character in string[] and set to Z
		return 1;
	}
}

// value contains a value from a hardware register that has many bit settings.
// bit_position is the value we want selected from value.
// bit_position 0 means get the lowest bit.
// bit_position 1 means get the next lowest bit.
// This function returns 0 if the selected bit is a 0
// It returns 1 if the selected bit is a 1
int get_bit_value( int value, int bit_position )
{
	//0x05, 	   2
	//0000 0101  100
	//and the two and check the bit position =1 
	//Scrap that. Lets shift it and get the LSB
	if (value>>bit_position & 1){//shifting over the bit position all the way to LSB (bit 0) - bXXXXX1 & b1 
		return 1;
	}
	else{
		return 0;
	}
}
