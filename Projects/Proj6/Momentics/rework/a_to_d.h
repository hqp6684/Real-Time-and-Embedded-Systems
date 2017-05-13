void request_io_access_permission(void);
void map_io_ports(void);
void a_d_init(void);
double analog_to_digital(void);
int scale_converted_signal(double voltage_post_conversion);
void output_to_stm(int scaled_signal);
