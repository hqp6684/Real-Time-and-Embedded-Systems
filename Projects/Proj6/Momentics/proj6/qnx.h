#ifndef QNX_H_
#define QNX_H_

void request_access_permission(void);
void map_ports(void);
void analog_to_digital_setup(void);
double analog_to_digital(void);
void output_to_stm(double convertedAD);
double scale(double volts);
