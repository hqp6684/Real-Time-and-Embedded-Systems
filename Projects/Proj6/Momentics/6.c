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
