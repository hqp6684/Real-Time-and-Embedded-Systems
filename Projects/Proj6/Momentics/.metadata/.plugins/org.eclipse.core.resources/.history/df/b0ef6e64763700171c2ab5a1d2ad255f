// Z. Weeden 2017

#include <stdio.h>
#include <stdlib.h>
#include "qnx.h"

/* Control program flow for QNX subsystem. Requests I/O access and maps ports needed.
   Indefinite loop that continually converts the analog signal seen. Prompts if converted voltage
   is out of range and outputs to STM. */
int main(void){
    double convertedAD = 0.0;
    request_access_permission();
    map_ports();
    analog_to_digital_setup();
    while(1){
        convertedAD = analog_to_digital();                  //double digital voltage
        if (convertedAD > 5.0 || convertedAD < -5.0){       //voltage has gone over accepted value
            printf("Voltage has gone over/under +/-5V\n");  //indicate failure in momentics/qnx
        }
        output_to_stm(scale(convertedAD));                   //kick it back to isolated qnx files - output to stm
    }
    return 0;
}
