// Z. Weeden 2017

#include <stdio.h>
#include <stdlib.h>       /* for EXIT_* */
#include <stdint.h>       /* for uintptr_t */
#include <hw/inout.h>     /* for in*() and out*() functions */
#include <sys/neutrino.h> /* for ThreadCtl() */
#include <sys/syspage.h>  /* for for cycles_per_second */
#include <sys/mman.h>     /* for mmap_device_io() */
#include <unistd.h>       /* for sleep() */
#include <math.h>

#define IO_PORT_SIZE                (1)
#define BASE_ADDRESS                (0x280) //QNX base address - a/d lsb
#define A_D_MSB                     (BASE_ADDRESS+1)
#define A_D_INPUT_CHANNEL           (BASE_ADDRESS+2)
#define A_D_GAIN_STATUS             (BASE_ADDRESS+3)
#define INTRPT_DMA_CONTROL_COUNTER  (BASE_ADDRESS+4)
#define FIFO_THRESHOLD              (BASE_ADDRESS+5)
#define D_A_LSB                     (BASE_ADDRESS+6)
#define D_A_MSB                     (BASE_ADDRESS+7)
#define PORT_A_OUT                  (BASE_ADDRESS+8)
#define PORT_B_OUT                  (BASE_ADDRESS+9)
#define PORT_C_OUT                  (BASE_ADDRESS+0xA)
#define DIRECTION_CONTROL           (BASE_ADDRESS+0xB)
#define COUNTER_TIMER_B0            (BASE_ADDRESS+0xC)
#define COUNTER_TIMER_B1            (BASE_ADDRESS+0xD)
#define COUNTER_TIMER_B2            (BASE_ADDRESS+0xE)
#define COUNTER_TIMER_CONTROL       (BASE_ADDRESS+0xF)

uintptr_t baseHandle;
uintptr_t adMSBHandle;
uintptr_t adChannelHandle;
uintptr_t adGainHandle;
uintptr_t dioPortAHandle;
uintptr_t dioDirectionHandle;

void request_io_access_permission(void){
    if ( ThreadCtl(_NTO_TCTL_IO, NULL) == -1){
        perror("Failed to get I/O access permission");
    }
}

void map_io_ports(void){
    baseHandle = mmap_device_io(IO_PORT_SIZE, BASE_ADDRESS);
    if(baseHandle == MAP_DEVICE_FAILED){
        perror("Failed to map base address");
    }

    adMSBHandle = mmap_device_io(IO_PORT_SIZE, A_D_MSB);
    if(adMSBHandle == MAP_DEVICE_FAILED){
        perror("Failed to map base address");
    }

    adChannelHandle = mmap_device_io(IO_PORT_SIZE, A_D_INPUT_CHANNEL);
    if(adChannelHandle == MAP_DEVICE_FAILED){
        perror("Failed to map base address");
    }

    adGainHandle = mmap_device_io(IO_PORT_SIZE, A_D_GAIN_STATUS);
    if(adGainHandle == MAP_DEVICE_FAILED){
        perror("Failed to map base address");
    }

    dioPortAHandle = mmap_device_io(IO_PORT_SIZE, PORT_A_OUT);
    if(dioPortAHandle == MAP_DEVICE_FAILED){
        perror("Failed to map base address");
    }

    dioDirectionHandle = mmap_device_io(IO_PORT_SIZE, DIRECTION_CONTROL);
    if(dioDirectionHandle == MAP_DEVICE_FAILED){
        perror("Failed to map base address");
    }
}

void a_d_init(void){
    //select the input channel
    out8(adChannelHandle,0x44); //Vin4

    //Select the input range
    out8(adGainHandle,0x01) //+-5V bipolar

    //DIO direction
    out8(dioDirectionHandle,0x00); //DIRA DIRAB output
}

double analog_to_digital(void){
    int LSB, MSB = 0
    short a_d_code; //signed short int
    double voltage = 0.0;
    //Wait for A/D cucruit to settle because we wrote to channel and range reg
    while (in8(adGainHandle) & 0x20){
        ; //A/D settling - 0 indicates ok to start
    }

    //Begin conversion
    out8(baseHandle,0x80);

    //Wait for conversion to finish
    while (in8(adGainHandle) & 0x80){
        ; //STS - A/D status 1 in progress, 0 idle
    }
    LSB = in8(baseHandle);
    MSB = in8(adMSBHandle);
    a_d_code = (MSB*256)+LSB;
    voltage = ((double)a_d_code/32768.0)*5.0;
    printf("%f\n",voltage);
    return voltage; //-5V to 5V
}

int scale_converted_signal(double voltage_post_conversion){
    return round((voltage_post_conversion+5.0)*25.5); //O to 255 scaled for output -> STM eq. rounded(-0.0667*scaled_sig)+21.242
    //return round((voltage_post_conversion+5.0)*6.3); //O to 63 scaled for output -> STM eq. rounded(-0.2698*scaled_sig)+21.237
    //return round((voltage_post_conversion+5.0)*1.5); //O to 15 scaled for output -> STM eq. rounded(-1.1333*scaled_sig)+21.227
}

void output_to_stm(int scaled_signal){
    out8(dioPortAHandle,scaled_signal)
}
