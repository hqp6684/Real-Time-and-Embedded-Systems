#include "timer.h"

int pwmRunning = 1;
uintptr_t ctrlHandle;
uintptr_t portA;

typedef struct {
    int arg1;
} PWMArgs;

int Duty_cycle[2] = {700000, 700000};

/* This updates the duty cycle for the PWM signal. 
   Used for moving the servos */
void changePWM(uint8_t PWMNO, uint32_t PWMVALUE){
    if(PWMNO == PWM0){
        Duty_cycle[PWMNO] = PWMVALUE;
    }
    
    if(PWMNO == PWM1){ 
        Duty_cycle[PWMNO] = PWMVALUE;
    }
    
}

/* Thread to process in single pwm channel takes in the PWMArgs for proper signal output. 
   Makes use of timespec struct and sleep to produce signal. */
void * pwmThreadFunc(void *arguments){
    int reference;
    uintptr_t port_adder;
    struct timespec initialTime, finishTime, elapsedTime, totalTime, sleepTime;
    PWMArgs *args = arguments;
    reference = args -> arg1;
    ThreadCtl(_NTO_TCTL_IO, NULL);
    if(reference){
        port_adder = mmap_device_io(8, BASE + 9); 
    } else{
        port_adder = mmap_device_io(8, BASE + 8); 
    }
    sleepTime.tv_sec = (time_t) 0;
    totalTime.tv_sec = (time_t) 0;
    totalTime.tv_nsec = 20000000 - 2000000;
    while (pwmRunning) {    // When pwm starts to run..
        clock_gettime(CLOCK_MONOTONIC, &initialTime);
        out8(port_adder, 0xFF); 
        sleepTime.tv_nsec = Duty_cycle[reference]; 
        nanospin( &sleepTime); 
        out8(port_adder, 0x00); 
        clock_gettime(CLOCK_MONOTONIC, &finishTime);
        elapsedTime.tv_nsec = (finishTime.tv_nsec - initialTime.tv_nsec);  // calculates the time passed in clock
        sleepTime.tv_nsec = totalTime.tv_nsec - elapsedTime.tv_nsec;   // subracts the clock that has been passed
        clock_nanosleep(CLOCK_MONOTONIC, 0, &sleepTime, NULL);
    }
    out8(port_adder, 0x00);
    return NULL;
}

/* Handles PWM initialization and memory mapping into our address space for QNX as well as root access request. */ 
void pwmInit(){
    PWMArgs pwmArgs;
    ThreadCtl(_NTO_TCTL_IO, NULL);
    uintptr_t ctrl_handle = mmap_device_io(8, BASE + 10);
    if(ctrlHandle == MAP_DEVICE_FAILED)
    {
        perror("map control register fails");
    }
    out8(ctrl_handle, 0x00 );
    for (j = 0; j < NUMOFPWM; j++) {
        pwmArgs.arg1 = j;
        pthread_create (NULL, NULL, pwmThreadFunc, (void *)&pwmArgs);
    }
}
