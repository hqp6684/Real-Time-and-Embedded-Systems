#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L;




void startTime(void){

}
int main(void){
    double accum;
    struct timespec start, stop;

    clock_gettime( CLOCK_REALTIME, &start);
    system(0);
    sleep(2);
    clock_gettime( CLOCK_REALTIME, &stop);
    accum = ( stop.tv_sec - start.tv_sec )+ (double)( stop.tv_nsec - start.tv_nsec ) / (double)BILLION;
    printf( "Time elapsed 1: %lf\n", accum );

    return EXIT_SUCCESS;
}
