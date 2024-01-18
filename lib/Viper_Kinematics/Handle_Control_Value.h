#ifndef HANDLE_CONTROL_VALUE_H
#define HANDLE_CONTROL_VALUE_H
#include "gclib.h"
#include "gclibo.h"
#include "../../lib/master_handle_data/Handle.h"
#include "../../lib/GalilControl/GalilControl.h"
/*<<<<<<< HEAD
#include <Eigen/Dense>
=======
#include <eigen3/Eigen/Dense>
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
*/
#include "main.h"

#if  UIINTERFACE_USED
#include "UIinterface.h"
extern UIinterface *Uip;
#endif

void Forcep_position_1step_control(GCon g_l,GCon g_r, Posture Pos, SensorData S_Data);
void Forcep_position_control(GCon g_l,GCon g_r, Posture Pos, SensorData S_Data);
float* motion_mapping_l(Posture forcep_data_prev,Posture forcep_data_cur);
float* motion_mapping_l(Posture forcep_data_init,Posture forcep_data_prev,Posture forcep_data_cur);
double* motion_mapping2_l(Posture forcep_data_init,Posture forcep_data_prev,Posture forcep_data_cur);
float* motion_mapping_r(Posture forcep_data_prev,Posture forcep_data_cur);
float* motion_mapping_r(Posture forcep_data_init,Posture forcep_data_prev,Posture forcep_data_cur);
int* forward_kinematics_l(float control_value_l[],double motor_position_l[]);
int* forward_kinematics_r(float control_value_r[],double motor_position_r[]);
int* forward_kinematics_l(float control_value_l[],float control_value_l_prev[],double motor_position_l[],double motor_position_cur[]);
int* forward_kinematics_r(float control_value_l[],float control_value_l_prev[],double motor_position_l[],double motor_position_cur[]);

#endif // HANDLE_CONTROL_VALUE_H
