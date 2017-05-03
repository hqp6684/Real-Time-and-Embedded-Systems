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

    /* Steps for A/D conversion
    Select input channel
    (To set the board to channel 4 only, write 0x44 to Base + 2.)
    (To set the board to read channels 0 through 15, write 0xF0 to Base + 2. )

    Select input range
    (For ±5V range (gain of 2), write 0x01 to Base + 3.)

    Wait for analog input circuit to settle
    ( Monitor the WAIT bit at Base + 3 bit 5.
    When it is 1 the circuit is actively settling on the input signal. When it is 0 the board is ready to
    perform A/D conversions.)
    
    Initiate A/D conversion
    (outp(base,0x80);)

    Wait for conversion to finish
    (
    int checkstatus() // returns 0 if ok, -1 if error
    int i;
    for (i = 0; i < 10000; i++){
        if !(inp(base+3) & 0x80) then return(0); // conversion completed
    }
    return(-1); // conversion didn’t complete)
    )

    Read data from board
    (
    LSB = inp(base);
    MSB = inp(base+1);
    Data = MSB * 256 + LSB; // combine the 2 bytes into a 16-bit value
    )

    Convert numerical data to a meaningful value
    (Input voltage = A/D value / 32768 * Full-scale input range)
    
    */




}