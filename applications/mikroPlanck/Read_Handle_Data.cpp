
//#include "viper_transmitter.h"
#include "Read_Handle_Data.h"
#include "../../lib/master_handle_data/Handle.h"
//#include "Forcep_control_thread.h"
#include "main.h"
Posture ControlPos_Data;
SensorData ControlS_Data;
QQueue<Posture> dataqueue;

auto time_point_rv_start = std::chrono::steady_clock::now();
auto time_point_rv_end = std::chrono::steady_clock::now();
std::chrono::duration<double> time_loop_rv;

auto Delay_rv_start = std::chrono::steady_clock::now();
auto Delay_rv_end = std::chrono::steady_clock::now();
std::chrono::duration<double> Delay_rv;
QMutex ReadData_lock;
ReadData::ReadData()
{

}

void ReadData::Save_Data(void)
{
    SensorData SS_Data;
    Posture HD_Pos;
    SS_Data = Forcep->return_PNO();
    HD_Pos=Forcep->calc_H_Post(SS_Data);
    system("clear");
    for (int i=4;i<7;i++)
    {
        qDebug()<<"Forcep->current_Post"<<i<<"=:"<<HD_Pos.Handle1[i-4];
    }
//    Forcep->
    if(ReadFirst)
    {
        ReadFirst=false;
        Forcep->Org_Data = SS_Data;
        Forcep->Org_Angle_L = Forcep->V->org_l;
        Forcep->Org_Angle_R = Forcep->V->org_r;
        Forcep->init_Post = HD_Pos;
    }

//    time_point_rv_end = std::chrono::steady_clock::now();
//    time_loop_rv =time_point_rv_end-time_point_rv_start;
//      system("clear");
//    printf("Baudrate = 1843200 time gap:%f\n ",time_loop_rv .count()*1000);
//    qDebug()<<"Baudrate = 1843200 time gap: " << time_loop_rv .count()*1000 << " ms\n";


}

void ReadData::run()
{
    Forcep= new Handle();
    connect(Forcep, &Handle::Viper_Data_Update, this, &ReadData::Save_Data);
    while(1)
    {
     //   time_point_rv_start = std::chrono::steady_clock::now();


        Forcep->get_PNO();
        Forcep->V->VCMD("GETSINGLE");
//        time_point_rv_start=std::chrono::steady_clock::now();

        SteadyDelay(4);
//         qDebug()<<"Baudrate = 1843200 time gap:"<<time_loop_rv .count()*1000;
    }
}
void ReadData::Data_output()
{

    ReadData_lock.lock();
    ControlPos_Data = Forcep->current_Post;
    ControlS_Data = Forcep->Sensor_Data;
    ReadData_lock.unlock();

//    system("clear");

//    time_point_rv_end = std::chrono::steady_clock::now();

//    time_loop_rv =time_point_rv_end-time_point_rv_start;
//    time_point_rv_start=time_point_rv_end;
//    qDebug()<<"Baudrate = 1843200 time gap: " << time_loop_rv .count()*1000 << " ms\n";
//        for (int i=4;i<7;i++)
//        {
//            qDebug()<<"Forcep->current_Post"<<i<<"=:"<<Forcep->current_Post.Handle1[i-4];
//        }
}
