#ifndef ROBOTARM_H
#define ROBOTARM_H
extern bool UI_FORCE_CONTROL;
void *robotarmThread(void* p);

void print_abSendData();
void print_abRecvData();
void print_tSendSDOData();
void print_tRecvSDOData();
unsigned int get_error_code(int joint);
long int get_position(int joint);
unsigned int get_status_word(int joint);
unsigned long int get_digital_input(int joint);
long int get_velocity(int joint);
int get_current(int joint);
short int get_mode(int joint);
int get_torque(int joint);
int init_torque(int joint);
int torque_mode(int joint);

int enable_joint(int joint);
int disable_joint(int joint);
int init_joint(int joint);
int update_speed(int joint,long int target_velocity);
int update_control_mode(int joint,short int mode);
int update_torque(int joint,long int target_torque);
void sync_target_position(int joint);
int toggle_servo(int joint);
int toggle_control_mode(int joint);

int set_SDO_6087(int joint,int val);
int set_brake_4602(int joint,int val);
int read_SDO_6087(int joint);
int read_brake_4602(int joint);
#endif // ROBOTARM_H
