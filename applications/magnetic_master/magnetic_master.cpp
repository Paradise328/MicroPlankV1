#include <QApplication>
#include <unistd.h>
#include <chrono>
#include <pthread.h>
#include <QMutex>
#include "viper_transmitter.h"
#include "magnetic_master.h"
auto time_start=std::chrono::steady_clock::now();
auto time_end=std::chrono::steady_clock::now();
QMutex viper_mutex;

class MasterVal{
    public:
        float l_x_input=0,r_x_input=0;
        float l_y_input=0,r_y_input=0;
        float l_z_input=0,r_z_input=0;
        float l_a_input=0,r_a_input=0;
        float l_b_input=0,r_b_input=0;
        float l_r_input=0,r_r_input=0;

        uint16_t foot_switch=0;
        uint16_t l_angle_input=0,r_angle_input=0;
        uint16_t l_switch_input=0,r_switch_input=0;

};

MasterVal mv;

class MagneticMasterThread:public QThread
{
public:
    uint8_t keyvalold=0;
    VIPER_Transmitter *viper_transmitter=nullptr;
    MagneticMasterThread()
    {


    }
    
    void RecvData(QByteArray qba)
    {
//        QByteArray qba=viper_transmitter->DataRecin;
//        viper_transmitter->DataRecin.clear();
        COMMU_FRAME cftemp;
        uint32_t ORTemp=0;
        uint32_t HandleVAL1=0;
        uint32_t HandleVAL2=0;
        uint16_t ADCVAL1;
        uint16_t ADCVAL2;
        uint16_t Keyval1;
        uint16_t Keyval2;

        uint32_t IOVAL=0;
        eDepackReturn edr=viper_transmitter->Depack_Frame(qba,cftemp);
        if(edr==Depack_SUCCESS)
        {
            if(cftemp.payload.args[0]==Dev_Sta_OK)
            {
                ORTemp=cftemp.payload.args[1];
                HandleVAL1|=ORTemp;
                ORTemp=cftemp.payload.args[2];
                HandleVAL1|=(ORTemp<<8);
                ORTemp=cftemp.payload.args[3];
                HandleVAL1|=(ORTemp<<16);
                ORTemp=cftemp.payload.args[4];
                HandleVAL1|=(ORTemp<<24);

                ORTemp=cftemp.payload.args[5];
                HandleVAL2|=ORTemp;
                ORTemp=cftemp.payload.args[6];
                HandleVAL2|=(ORTemp<<8);
                ORTemp=cftemp.payload.args[7];
                HandleVAL2|=(ORTemp<<16);
                ORTemp=cftemp.payload.args[8];
                HandleVAL2|=(ORTemp<<24);

                IOVAL=cftemp.payload.args[9];
                ADCVAL1=HandleVAL1;
                Keyval1=HandleVAL1>>16;
                ADCVAL2=HandleVAL2;
                Keyval2=HandleVAL2>>16;

                viper_mutex.lock();

                mv.foot_switch = IOVAL;
                mv.r_angle_input = ADCVAL1;
                mv.l_angle_input = ADCVAL2;
                mv.r_switch_input = Keyval1;
                mv.l_switch_input = Keyval2;

                viper_mutex.unlock();
                
                /*
                system("clear");
                qDebug()<<"ADCVAL1="<<QString::number(ADCVAL1,10)
                <<"\nKeyval1="<<QString::number(Keyval1,16)
                <<"\nADCVAL2="<<QString::number(ADCVAL2,10)
                <<"\nKeyval2="<<QString::number(Keyval2,16)
                <<"\nIOVAL="<<QString::number(IOVAL,16);
                */

                time_end=std::chrono::steady_clock::now();
               if( std::chrono::duration_cast<std::chrono::microseconds>(time_end-time_start).count()>3000)
               {
               }
               // qDebug()<<"read data gap time"<<std::chrono::duration_cast<std::chrono::microseconds>(time_end-time_start).count()<<"us";

            }
            else if(cftemp.payload.args[0]==Dev_Sta_LEFTHANDLE_ERROR)
            {
                 qDebug()<<"LEFTHANDLE_ERROR!";
            }
            else if(cftemp.payload.args[0]==Dev_Sta_RIGHTHANDLE_ERROR)
            {
                 qDebug()<<"RIGHTHANDLE_ERROR!";
            }
            else
            {
                qDebug()<<"UNKNOW_ERROR!";
            }
            qba.clear();

        }
        else
        {
            qDebug()<<"Depack_Frame Fail";
        }
    }
   void run()
   {
        viper_transmitter=new VIPER_Transmitter("192.168.42.100",10080);
        connect(viper_transmitter,&VIPER_Transmitter::NewDataIn,this,&MagneticMasterThread::RecvData);
       while(1)
       {
            time_start=std::chrono::steady_clock::now();
            viper_transmitter->VCMD("GETSINGLE");

            auto Qtime_next=QTime::currentTime().addMSecs(2);
            while(QTime::currentTime()<Qtime_next)
            QCoreApplication::processEvents(QEventLoop::AllEvents,2);
       }
   }

};

class ControlThread:public QThread{
public:
    ControlThread(){};
    void run(){
       // std::chrono::duration<double> time_loop_passed;
        const double kTimePerLoop=10.0;

        while(1){
            system("clear");
            auto time_loop_start = std::chrono::steady_clock::now();

            viper_mutex.lock();
            printf("reading inputs:\n");
            qDebug()<<"ADCVAL1="<<QString::number(mv.l_angle_input,10)
            <<"\nKeyval1="<<QString::number(mv.l_switch_input,16)
            <<"\nADCVAL2="<<QString::number(mv.r_angle_input,10)
            <<"\nKeyval2="<<QString::number(mv.r_switch_input,16)
            <<"\nIOVAL="<<QString::number(mv.foot_switch,16);
            viper_mutex.unlock();


            auto time_loop_end = std::chrono::steady_clock::now();
            std::chrono::duration<double> time_loop_passed = time_loop_end - time_loop_start;

            printf("time of the loop: %f ms\n",time_loop_passed.count()*1000);
            if(kTimePerLoop - time_loop_passed.count() * 1000 > 0){
                usleep((kTimePerLoop - time_loop_passed.count() * 1000.0) * 1000.0);
            }else{
                //TODO: what to do after timeout??
                printf("TIMEOUT\n");
            }
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    usleep(2000*1000);
    
    MagneticMasterThread *MagnThr = new MagneticMasterThread();
    MagnThr->start();

    ControlThread *CtrlThr = new ControlThread();
    CtrlThr->start();

    // main_thead:mainly for printing
  //  pthread_t ctrl_thread_id;
  //  pthread_create(&ctrl_thread_id, NULL, ControlThread, NULL);

    return a.exec();
}