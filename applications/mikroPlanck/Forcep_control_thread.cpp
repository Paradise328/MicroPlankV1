#include "Forcep_control_thread.h"
#include "../../lib/GalilControl/GalilControl.h"
#include "../../lib/master_handle_data/Handle.h"
#include "../../lib/Viper_Kinematics/Handle_Control_Value.h"
#include "Read_Handle_Data.h"
#include "main.h"
extern Posture ControlPos_Data;
extern SensorData ControlS_Data;
//auto time_point_cl_start = std::chrono::steady_clock::now();
//auto time_point_cl_end = std::chrono::steady_clock::now();
//std::chrono::duration<double> time_loop_control;
extern double time_loop_control;
extern QMutex ReadData_lock;
extern int target_p_s[17];
extern double motor_pos_out[8];
Forcep_Control_Thread::Forcep_Control_Thread(QObject *parent) : QThread(parent)
{

}
void Forcep_Control_Thread::run()
{
    Posture Handle_pos;
    SensorData S_Data;
    GReturn rc = GALIL_EXAMPLE_OK;
    char buf[G_SMALL_BUFFER];

    //var used to refer to a unique connection. A valid connection is nonzero.
    GCon g_l = 0;
    GCon g_r = 0;
    char* address = "192.168.42.30";  //Retrieve address from command line
    sprintf(buf, "%s --subscribe MG", address);
    galil(GOpen(buf, &g_l));
    char* address1 = "192.168.42.40";  //Retrieve address from command line
    sprintf(buf, "%s --subscribe MG", address1);
    galil(GOpen(buf, &g_r)); //Opens a connection at the provided address
    //Demonstrates how to receive messages from the controller
    //and detect differences in Trace and crashed code.
    initGali(g_l);
    initGali(g_r);
    std::ofstream outfile("str_test0001.txt",ios::trunc);
    static uint32_t counter=0;
    while(1){
//        time_point_cl_start = std::chrono::steady_clock::now();
        emit get_Pos();
        ReadData_lock.lock();
        Handle_pos = ControlPos_Data;
        S_Data = ControlS_Data;
        ReadData_lock.unlock();
        SteadyDelay(10);
       // Handle::Handle_wait(10);
//        qDebug()<<"Thread";
//        for(int i=4;i<7;i++)
//        {
//            qDebug()<<"  "<<S_Data.sensor1_pos[i-4];
//        }
//        Forcep_position_1step_control(g_l,g_r,Handle_pos,S_Data);

  //      Forcep_position_control(g_l,g_r,Handle_pos,S_Data);
//        outfile<<S_Data.sensor1_pos[0]<<" "<<S_Data.sensor1_pos[1]<<" "<<S_Data.sensor1_pos[2]<<" "<<S_Data.sensor2_pos[0]<<" "<<S_Data.sensor2_pos[1]<<" "<<S_Data.sensor2_pos[2]<<" "<<target_p_s[12]<<" "<<target_p_s[13]<<" "<<target_p_s[14]<<" "<<motor_pos_out[4]<<" "<<motor_pos_out[5]<<" "<<motor_pos_out[6]<<"\n";
//        time_point_cl_end = std::chrono::steady_clock::now();
//        time_loop_control = time_point_cl_end-time_point_cl_start;
 //       qDebug()<<"Single control loop time:"<<time_loop_control*1000<<"ms";

    };
}
