// C language self test
// SWEN-563
// Larry Kiser Jan. 30, 2017

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "c_language_test.h"
#include "unit_tests.h"

// Run the unit tests or as a "normal program".
int main( int argc, char *argv[] ) {

	// Execute unit tests if program name contains "test".
	if ( strstr( argv[0], "test" ) )
		return test() ;

	printf( "\nDoes nothing -- please run unit tests instead.\n\n" ) ;
	
	return 0 ;
}
