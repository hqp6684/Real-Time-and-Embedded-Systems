
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN (30)
#define MAX (50)
#define size (200)

int array[size];

int randMToN(int M, int N)
{
	return M + (rand() / ( RAND_MAX / (N+1-M) ) ) ;
}

int main(void) {
	int i, j, bigCount, temp;
	int myNum=0;
	for (i=0;i<size;i++){
		myNum=randMToN(MIN,MAX);
		printf("%d\n",myNum);
		array[i]=myNum;
	}
	bigCount = array[0];
	  for (i = 0; i < size; ++i)
	    if(array[i] > bigCount)
	      bigCount = array[i];
	  temp = bigCount;
	  printf("\n\n");

	for (i = 0; i < bigCount; ++i) {
	    printf("\n%3d|", bigCount - i);
	     for (j = 0; j < size; ++j)
	       if (array[j] < temp)
	     printf("   ");
	       else {
	     printf("  #");
	     --array[j];
	       }
	     --temp;
	  }
	return EXIT_SUCCESS;
}
