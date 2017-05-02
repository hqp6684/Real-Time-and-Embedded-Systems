/*

0 voltage = position 3
- voltage = positions 4 and 5
+ voltage = position 0, 1, and 2 

VOLTAGE   -5   -4   -3   -2   -1      0    1    2    3    4    5 
POSITION   5           4              3       2        1       0
CCRx       21          16            13       10       7       4  

*/

prevVoltage = fetchVOLTAGEFromPIN(); //this stays the same and isnt updated
currentVoltage = prevVoltage;

while 1:
	if (currentVoltage == -5){  //lower bound - negative voltage leftmost position
		RED_LED_OFF(); //valid voltage
		CCRx = 21 //position5;
	}
	else if (currentVoltage == 0){ //middle defined - 0 voltage middle position
		RED_LED_OFF(); //valid voltage
		CCRx = 13 //position3;
	}
	else if (currentVoltage == 5){ //upper bound - positive voltage rightmost position
		RED_LED_OFF(); //valid voltage
		CCRx = 4 //position0;
	}
	else if (currentVoltage > 5){ //voltage has gone over accepted value
		RED_LED_ON(); //indicate failure on STM/servo
		printf("Voltage has gone over +5V\n"); //indicate failure in momentics/qnx
	}
	else if (currentVoltage < -5){ //voltage has gone lower accepted value
		RED_LED_ON(); //indicate failure on STM/servo
		printf("Voltage has gone under -5V\n"); //indicate failure in momentics/qnx
	}
	else{ //in the middle - lets do some math for the compare cap reg
		RED_LED_OFF(); //valid voltage
		if (currentVoltage < prevVoltage){ //voltage is less going negative - increase CCR val to move servo more right
			CCRx++;
		}
		else if (currentVoltage > prevVoltage){ //voltage is more going positive - decrease CCR val to move servo more left
			CCRx--;
		}
		else{ //voltage has not changed - hold motor at current spot 
			CCRx = CCRx;
		}

	}
	prevVoltage = currentVoltage;
	currentVoltage = fetchVOLTAGEFromPIN();

