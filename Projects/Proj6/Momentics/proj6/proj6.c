#include <stdio.h>
#include <stdlib.h>       /* for EXIT_* */
#include <stdint.h>       /* for uintptr_t */
#include <hw/inout.h>     /* for in*() and out*() functions */
#include <sys/neutrino.h> /* for ThreadCtl() */
#include <sys/syspage.h>  /* for for cycles_per_second */
#include <sys/mman.h>     /* for mmap_device_io() */
#include <unistd.h>       /* for sleep() */

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

double analog_to_digital(uintptr_t baseHandle, uintptr_t adMSBHandle, uintptr_t adGainStatusHandle){
    int LSB, MSB = 0; //check these datatypes need proper sizeof()
    long a_d_val = 0; //
    int i;
    double volts;

    //Wait for analog circuit to settle
    while( (in8(adGainStatusHandle) & 0x20) ){ //base+3 bit 5 is not less than 32 0010 0000 - subject to 'hardware fault'
        ;                                   //A/D is setting new value
    }
    //bit 5 went low - ok to start conversion

    //Initiate conversion
    out8(baseHandle,0x80);                //1000 0000 STRTAD start A/D

    //Wait for conversion to finish
    while( (in8(adGainStatusHandle) & 0x80) ){ //base+3 bit 7 is not less than 128 1000 0000 - subject to 'hardware fault'
        ;                                   //converstion still in progress
    }
    //bit 7 went low conversion complete

    //Resolving adc value
    LSB = in8(baseHandle);
    MSB = in8(adMSBHandle);
    a_d_val = MSB * 256 + LSB; //essentially shifts MSB over 8bits and appends the lsb
    volts = (a_d_val/32768.0)*5.0;
    if (volts > 5.0 || volts < -5.0){
        ;
    }
    else{
        for (i=0; i<10; i++){
            if (i-5 > (int)volts){
                printf(' ');
            }
            else{
                printf('|');
            }
        }
        printf('\n');
    }
    return volts;
}

int determine_polarity(double volts){
    int isNegative = 0;
    if ((int)volts > 0x8){ // 1000 - negative number
        isNegative = 1;
    }
    if (0x8 > (int)volts){ // less than 1000 - positive number
        isNegative = 0;
    }
    return isNegative;
}

int abs_voltage(double volts){ //makes it so that 3 pins + 1 polarity pin are needed to relay information
    switch ((int)volts & 0xF){ //bottom nibble
        case 0xB:
        case 0x5:
            return 5;
            break; //+-5
        case 0xC:
        case 0x4:
            return 4;
            break; //+-4
        case 0xD:
        case 0x3:
            return 3;
            break; //+-3
        case 0xE:
        case 0x2:
            return 2;
            break; //+-2
        case 0xF:
        case 0x1:
            return 1;
            break; //+-1
        case 0x0:
            return 0;
            break; //0
    }
}

int main(void){
    double convertedAD = 0.0;
    uintptr_t baseHandle, adMSBHandle, adChannelHandle, adGainStatusHandle, portAHandle, portBHandle, dataDirectionHandle;

    if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1){ // request access rights to the hardware I/O for the thread
        perror("Failed to get I/O access permission");
        return 1;
    }

    baseHandle = mmap_device_io(IO_PORT_SIZE, BASE_ADDRESS); // Now have a handle to the device register which you can use in a call to any of the in*() or out*() functions that QNX provides.
    if(baseHandle == MAP_DEVICE_FAILED){
        perror("Failed to map base addr");
        return 2;
    }

    adGainStatusHandle = mmap_device_io(IO_PORT_SIZE, A_D_GAIN_STATUS); // Now have a handle to the device register which you can use in a call to any of the in*() or out*() functions that QNX provides.
    if(adGainStatusHandle == MAP_DEVICE_FAILED){
        perror("Failed to map A/D gain status register");
        return 2;
    }

    adChannelHandle = mmap_device_io(IO_PORT_SIZE, A_D_CHANNEL); // Now have a handle to the device register which you can use in a call to any of the in*() or out*() functions that QNX provides.
    if(adChannelHandle == MAP_DEVICE_FAILED){
        perror("Failed to map A/D channel register");
        return 2;
    }

    adMSBHandle = mmap_device_io(IO_PORT_SIZE, A_D_MSB); // Now have a handle to the device register which you can use in a call to any of the in*() or out*() functions that QNX provides.
    if(adMSBHandle == MAP_DEVICE_FAILED){
        perror("Failed to map A/D MSB register");
        return 2;
    }

    portAHandle = mmap_device_io(IO_PORT_SIZE,PORT_A_OUT); //
    if(portAHandle == MAP_DEVICE_FAILED){
        perror("Failed to map port a register");
        return 2;
    }

    portBHandle = mmap_device_io(IO_PORT_SIZE,PORT_B_OUT); //
    if(portBHandle == MAP_DEVICE_FAILED){
        perror("Failed to map port b register");
        return 2;
    }

    dataDirectionHandle = mmap_device_io(IO_PORT_SIZE,DIRECTION_CONTROL); //
    if(dataDirectionHandle == MAP_DEVICE_FAILED){
        perror("Failed to map data direction register");
        return 2;
    }

    //Select input channel
    out8(adChannelHandle,0xF0);                 //1111 0000 Read channels 0 through 15

    //Select input range
    out8(adGainStatusHandle, 0x01);            //0000 0001 bipolar +-5V gain of 2

    while(1){
        convertedAD = analog_to_digital(baseHandle, adMSBHandle, adGainStatusHandle); //double digital voltage - should we pass the ports?
        while (convertedAD > 5.0 || convertedAD < -5.0){ //voltage has gone over accepted value
            printf("Voltage has gone over/under +/-5V\n"); //indicate failure in momentics/qnx
            convertedAD = analog_to_digital(baseHandle, adMSBHandle, adGainStatusHandle); //double digital voltage - should we pass the ports?    
        }
        out8(portAHandle, convertedAD); //output on portA
    }
    return 0;
}
