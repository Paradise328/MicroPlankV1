#include "MikroPlanck.h"
#include "main.h"
#include <QWaitCondition>
#include <QApplication>

bool UI_FORCE_CONTROL;
int force_control_init[3]={0,0,0};
int robotarm_enable_btn=0;

int record_while_speed_control_prev=0;

extern QString filename;
extern int record_while_speed_control;
void robot_arm_thread::run()
   {
       std::chrono::duration<double> time_loop_passed;
       const double kTimePerLoop=1.0;
       long int velocity_prev[3] = {0,0,0};
       long int velocity_now[3] = {0,0,0};
       long int velocity_diff[3] = {0,0,0};
       int torque_prev[3] = {0,0,0};
       int torque_now[3] = {0,0,0};
       int torque_diff[3] = {0,0,0};
       QVector<int> joint_sm = {0,0,0,0,0,0};
//       int joint_sm[6]={0,0,0,0,0,0};
       std::deque<int> torque_joint1;
       std::deque<int> torque_joint2;
       std::deque<int> torque_joint3;
       std::deque<std::deque<int>>output_torque_joint;
       output_torque_joint.push_back(torque_joint1);
       output_torque_joint.push_back(torque_joint2);
       output_torque_joint.push_back(torque_joint3);
        static int robot_arm_enable_btn_static=0;
        int i=0;
        std::ofstream outfile;
       while(1){

           if(record_while_speed_control){
               if(!record_while_speed_control_prev){
                   //open a file
                   outfile.open(filename.toStdString());
                   record_while_speed_control_prev=1;
               }
               //record to file
               std::cout <<"recording\n";
               std::cout <<get_velocity(1)<<" "<<get_torque(1)<<"\n";

               outfile<<get_velocity(1)<<" "<<get_torque(1)<<"\n";
           }else if(!record_while_speed_control){
               if(record_while_speed_control_prev){
                //close file
                   outfile.close();
                   record_while_speed_control_prev=0;
               }
           }
           auto time_loop_start = std::chrono::steady_clock::now();


           system("clear");
           printf("UI_FORCE_CONTROL: %d\n",UI_FORCE_CONTROL);
           /*print_abRecvData();
           print_abSendData();
           print_tRecvSDOData();
           print_tSendSDOData();
           printf("status_word: %x %x %x\n",get_status_word(1),get_status_word(2),get_status_word(3));
           printf("control_mode: %x %x %x\n",get_mode(1),get_mode(2),get_mode(3));*/
           printf("actual_position: %ld %ld %ld\n",get_position(1),get_position(2),get_position(3));
           printf("actual_velocity: %8ld,%8ld,%8ld\n",get_velocity(1),get_velocity(2),get_velocity(3));
           printf("actual_torque: %4d,%4d,%4d\n",get_torque(1),get_torque(2),get_torque(3));
           printf("digital_input: %d %d %d\n",get_digital_input(1),get_digital_input(2),get_digital_input(3));
           printf("torque value when pressed: %d %d %d\n",force_control_init[0],force_control_init[1],force_control_init[2]);
            printf("static: %d\n",robot_arm_enable_btn_static);
        //    printf("current: %d\n",robot_arm_enable_btn_current);
            printf("i: %d\n",i);

           int robot_arm_enable_btn_current=get_digital_input(3);

           if(robot_arm_enable_btn_current!=robot_arm_enable_btn_static){
//               toggle_servo(1);toggle_servo(2);toggle_servo(3);
                toggle_control_mode(1);toggle_control_mode(2);toggle_control_mode(3);
           }
           joint_sm[0]=get_status_word(1);
           joint_sm[1]=get_status_word(2);
           joint_sm[2]=get_status_word(3);
           joint_sm[3]=get_mode(1);
           joint_sm[4]=get_mode(2);
           joint_sm[5]=get_mode(3);

           robot_arm_enable_btn_static=robot_arm_enable_btn_current;
           sync_target_position(1);sync_target_position(2);sync_target_position(3);

           emit setArmEnableButton(robot_arm_enable_btn_current);
           emit setArmStatusMode(joint_sm);

           // loop end, prepare for next loop
           for(int i=0;i<3;i++){
               velocity_prev[i]=velocity_now[i];
               torque_prev[i]=torque_now[i];
               velocity_diff[i]=0;
               torque_diff[i]=0;
           }
         //   usleep(200);
           auto time_loop_end = std::chrono::steady_clock::now();
           std::chrono::duration<double> time_loop_passed = time_loop_end - time_loop_start;

       //    printf("time of the loop: %f ms\n",time_loop_passed.count()*1000);
           if(kTimePerLoop - time_loop_passed.count() * 1000 > 0){
               usleep((kTimePerLoop - time_loop_passed.count() * 1000.0) * 1000.0);
           }else{
               //TODO: what to do after timeout??
               printf("TIMEOUT\n");
           }
       }

   }



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MikroPlanck *mp=new MikroPlanck();
    mp->show();

  //   robot arm thread
    pthread_t robot_arm_thread_id;
    pthread_create(&robot_arm_thread_id, NULL, robotarmThread, NULL);

    robot_arm_thread *armThread=new robot_arm_thread();
    armThread->connect(armThread,&robot_arm_thread::setArmEnableButton,mp->robot_arm_widget,&RobotArmPage::setArmEnableStatus);
    armThread->connect(armThread,&robot_arm_thread::setArmStatusMode,mp->robot_arm_widget,&RobotArmPage::setArmSM);
    armThread->start();
/*
    //read data from magnetic master console
    ReadData *ReadData_Threadp;
    ReadData_Threadp=new ReadData();
    ReadData_Threadp->start();
    Handle::Handle_wait(20*1000);

    Forcep_Control_Thread *Motion_Control;
    Motion_Control = new Forcep_Control_Thread();
    Motion_Control->connect(Motion_Control,&Forcep_Control_Thread::get_Pos,ReadData_Threadp,&ReadData::Data_output);
    Motion_Control->start();
*/

// this->connect(m_sockettemp,&QTcpSocket::readyRead,this,&Peripheral_Device::on_Data_In);

    return a.exec();
}
