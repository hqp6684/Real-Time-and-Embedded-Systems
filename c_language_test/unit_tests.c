// unit_tests.c
// Larry Kiser October 30, 2015

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "c_language_test.h"
#include "unit_tests.h"

// used by stdout switching functions
static int stdout_file_descriptor = 0 ;
static fpos_t stdout_position ;

// Redirect printf's to the specified text file (redirect stdout).
static void switch_stdout_to_file( char *filename )
{
	fflush( stdout ) ;
	fgetpos( stdout, &stdout_position ) ;
	stdout_file_descriptor = dup( fileno( stdout ) ) ;
	freopen( filename, "w", stdout ) ;
}

// Restore normal printf behavior (restore stdout).
// MUST be called only after a prior call to switch_stdout_to_file!
static void restore_stdout( void )
{
	if ( stdout_file_descriptor )
	{
		fflush( stdout ) ;
		dup2( stdout_file_descriptor, fileno( stdout ) ) ;
		close( stdout_file_descriptor ) ;
		clearerr( stdout ) ;
		fsetpos( stdout, &stdout_position ) ;
	}
}

// Simple boolean assert function for unit testing
int assert( int test_result, char error_format[], ... ) {
	va_list arguments ;
	static int test_number = 1 ;
	int result = 1 ;	// return 1 for test passed or 0 if failed
	// bogus code that does nothing except avoid compiler warning on these two unused functions.
	#pragma GCC diagnostic ignored "-Waddress"
	if ( switch_stdout_to_file && restore_stdout ) ;
	#pragma GCC diagnostic warning "-Waddress"
	
	if ( ! test_result ) {
		va_start( arguments, error_format ) ;
		printf( "Test # %d failed: ", test_number ) ;
		vprintf( error_format, arguments ) ;
		printf( "\n" ) ;
		result = 0 ;
	}
	test_number++ ;
	return result ;
}

//////////////////////////////////////////////////////////////////////////
// Begin unit tests //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// do the unit tests
int test()
{
	int passcount = 0 ;
	int failcount = 0 ;
	char *test1 = "findfinstring" ;
	int xvalues1[] = { 0, 5, 2 } ;
	int yvalues1[] = { -1, 9, 3 } ;
	char bad_code_string[] = "badcode" ;
	char updated_bad_code_string[] = "badcodZ" ;
	
	printf( "\nTests for get_pointer_at_position...................\n" ) ;
	// Test 1
	assert( get_pointer_at_position( test1, 0 ) == test1,
		"Expected pointer to the beginning of the string" )
		? passcount++ : failcount++ ;
	// Test 2
	assert( get_pointer_at_position( test1, 4 ) == test1 + 4,
		"Expected pointer at position 4" )
		? passcount++ : failcount++ ;
	// Test 3
	assert( get_pointer_at_position( test1, 12 ) == test1 + 12,
		"Expected pointer to last character" )
		? passcount++ : failcount++ ;
	// Test 4
	assert( get_pointer_at_position( test1, 13 ) == NULL,
		"Expected NULL pointer" )
		? passcount++ : failcount++ ;
	
	printf( "\nTests for convert_and_sum...........................\n" ) ;
	// Test 5
	assert( convert_and_sum( xvalues1, 2, -1, 3 ) == 11,
		"Sum of y values must be 11" )
		? passcount++ : failcount++ ;
	// Test 6
	assert( memcmp( xvalues1, yvalues1, sizeof( xvalues1 ) ) == 0,
		"Must match expected y values" )
		? passcount++ : failcount++ ;

	printf( "\nTests for same_array................................\n" ) ;
	// Test 7
	assert( same_array( xvalues1, xvalues1 ) == 1,
		"Expect 1 since it is the same array" )
		? passcount++ : failcount++ ;
	// Test 8
	assert( same_array( xvalues1, &xvalues1[1] ) == 0,
		"Expect 0 since a different position in the array" )
		? passcount++ : failcount++ ;
	// Test 9
	assert( same_array( NULL, xvalues1 ) == 0,
		"Expect 0 since one is NULL" )
		? passcount++ : failcount++ ;
	// Test 10
	assert( same_array( xvalues1, NULL ) == 0,
		"Expect 0 since 2nd is NULL" )
		? passcount++ : failcount++ ;
		
	printf( "\nTests for bool_flip_flop............................\n" ) ;
	// Test 11
	assert( bool_flip_flop() == 1,
		"Expect 1 this is the first time it is called" )
		? passcount++ : failcount++ ;
	// Test 12
	assert( bool_flip_flop() == 0,
		"Expect 0 this is the second time it is called" )
		? passcount++ : failcount++ ;
	// Test 13
	assert( bool_flip_flop() == 1,
		"Expect 1 this is the third time it is called" )
		? passcount++ : failcount++ ;
	// Test 14
	assert( bool_flip_flop() == 0,
		"Expect 0 this is the fourth time it is called" )
		? passcount++ : failcount++ ;
		
	printf( "\nTests for fix_bad_code..............................\n" ) ;
	// Test 15
	assert( fix_bad_code(bad_code_string) == 1,
		"Expect 1 since it is not any empty string" )
		? passcount++ : failcount++ ;
	// Test 16
	assert( strcmp( bad_code_string, updated_bad_code_string ) == 0,
		"Expect Z in last position of bad_code_string but got this string %s", bad_code_string )
		? passcount++ : failcount++ ;
	
	printf( "\nTests for the get_bit_value function\n" ) ;
	// Test 17
	assert( 1 == get_bit_value( 0x100, 8 ),
		"Expect 1 because bit 5 (0x100) is a one " )
		? passcount++ : failcount++ ;
	
	// Test 18
	assert( 0 == get_bit_value( 0xf7, 3 ),
		"Expect 0 because bit 3 (0x8) is a zero " )
		? passcount++ : failcount++ ;
	
	// Test 19
	assert( 1 == get_bit_value( 0xff, 3 ),
		"Expect 1 because bit 3 (0x08) is a one " )
		? passcount++ : failcount++ ;
	
		
	printf( "\nSummary of unit tests:\n%d tests passed\n%d tests failed\n\n", passcount, failcount ) ;
	
	return failcount ;
}