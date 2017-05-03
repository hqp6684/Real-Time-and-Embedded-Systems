#include <sys/neutrino.h>
#include <stdint.h>
#include <sys/mman.h>

#define IO_PORT_SIZE                (1)
#define BASE_ADDRESS                (0x280) //QNX base address - a/d lsb
#define A_D_MSB                     (BASE_ADDRESS+1) //
#define A_D_CHANNEL                 (BASE_ADDRESS+2) //
#define A_D_GAIN_STATUS             (BASE_ADDRESS+3) //
#define INTRPT_DMA_CONTROL_COUNTER  (BASE_ADDRESS+4)
#define FIFO_THRESHOLD              (BASE_ADDRESS+5)
#define D_A_LSB                     (BASE_ADDRESS+6)
#define D_A_MSB                     (BASE_ADDRESS+7)
#define PORT_A_OUT                  (BASE_ADDRESS+8)
#define PORT_B_OUT                  (BASE_ADDRESS+9)
#define PORT_C_OUT                  (BASE_ADDRESS+0xA)
#define DIRECTION_CONTROL           (BASE_ADDRESS+0xB) //
#define COUNTER_TIMER_B0            (BASE_ADDRESS+0xC)
#define COUNTER_TIMER_B1            (BASE_ADDRESS+0xD) //
#define COUNTER_TIMER_B2            (BASE_ADDRESS+0xE)
#define COUNTER_TIMER_CONTROL       (BASE_ADDRESS+0xF)

/*

0 voltage = position 3
- voltage = positions 4 and 5
+ voltage = position 0, 1, and 2 

VOLTAGE   -5   -4   -3   -2   -1      0    1    2    3    4    5 
POSITION   5           4              3       2        1       0
CCRx       21          16            13       10       7       4  

*/

//function to be run on QNX
void convertToDigitalAndOutput(void){
    while (1){
        voltageIn = fetchVoltageFromGenerator(); //continually update
        digitalVoltage = convertADC(voltageIn);
        //route digitalVoltage to pin
        if (digitalVoltage < 0 && digitalVoltage > -5){  //negative voltage leftmost position
            printf("Valid negative voltage\n");
        }
        else if (digitalVoltage == 0){ 
            printf("Valid neutral voltage\n");
        }
        else if (digitalVoltage > 0 && digitalVoltage < 5){ //positive voltage rightmost position
            printf("Valid positive voltage\n");

        }
        else if (digitalVoltage > 5){ //voltage has gone over accepted value
            printf("Voltage has gone over +5V\n"); //indicate failure in momentics/qnx
        }
        else if (digitalVoltage < -5){ //voltage has gone lower accepted value
            printf("Voltage has gone under -5V\n"); //indicate failure in momentics/qnx
        }
        else{
            ;
        }
    }
}

void main(void){
    int LSB, MSB, a_d_val = 0;
    if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1){ // request access rights to the hardware I/O for the thread
        perror("Failed to get I/O access permission");
        return 1;
    }

    uintptr_t baseHandle;
    baseHandle = mmap_device_io(IO_PORT_SIZE, BASE_ADDRESS); // Now have a handle to the device register which you can use in a call to any of the in*() or out*() functions that QNX provides.
    if(baseHandle == MAP_DEVICE_FAILED){
        perror("Failed to map base addr");
        return 2;
    }

    //Select input channel
    outp(A_D_CHANNEL,0xF0);                 //1111 0000 Read channels 0 through 15
    
    //Select input range
    outp(A_D_GAIN_STATUS, 0x01);            //0000 0001 bipolar +-5V gain of 2
    
    //Wait for analog circuit to settle
    while( (inp(A_D_GAIN_STATUS) & 0x20) ){ //base+3 bit 5 is not less than 32 0010 0000 
        ;                                   //A/D is setting new value
    }
    //bit 5 went low - ok to start conversion

    //Initiate conversion
    outp(BASE_ADDRESS,0x80);                //1000 0000 STRTAD start A/D
    
    //Wait for conversion to finish
    while( (inp(A_D_GAIN_STATUS) & 0x80) ){ //base+3 bit 7 is not less than 128 1000 0000 
        ;                                   //converstion still in progress
    }
    //bit 7 went low conversion complete

    //Resolving adc value
    LSB = inp(BASE_ADDRESS);
    MSB = inp(A_D_MSB);
    a_d_val = MSB * 256 + LSB;

}