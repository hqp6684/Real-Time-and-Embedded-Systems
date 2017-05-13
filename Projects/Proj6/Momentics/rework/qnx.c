// Z. Weeden 2017

#include <stdio.h>
#include <stdlib.h>
#include "a_to_d.h"

/* Control program flow for QNX subsystem. Requests I/O access and maps ports needed.
   Indefinite loop that continually converts the analog signal seen. Prompts if converted voltage
   is out of range and outputs to STM. */
int main(void){
    double converted_signal = 0.0;
    request_io_access_permission();
    map_io_ports();
    a_d_init();
    while(1){
        converted_signal = analog_to_digital();                  //double digital voltage
        if (converted_signal > 5.0 || converted_signal < -5.0){       //voltage has gone over accepted value
            printf("Voltage has gone over/under +/-5V\n");  //indicate failure in momentics/qnx
        }
        output_to_stm(scale_converted_signal(converted_signal));                   //kick it back to isolated qnx files - output to stm
    }
    return 0;
}
