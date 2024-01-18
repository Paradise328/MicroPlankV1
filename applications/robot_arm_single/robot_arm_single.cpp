#include "mainwindow.h"
#include <QApplication>
#include <chrono>
#include <ratio>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <linux/input.h>


#include <unistd.h>
#include <queue>
#include <deque>
#include "robot_arm_single.h"

bool UI_FORCE_CONTROL;
int force_control_init[3]={0,0,0};
int robotarm_enable_btn=0;


int avr_torque(std::deque<int> torque_joint)
{
    int sum=0;
    for(unsigned int i=0;i<torque_joint.size();++i){
        sum+=torque_joint.at(i);
    }
    if(sum>=0){
        return sum/(torque_joint.size());
    }else{
        return (-1)*((-sum)/(torque_joint.size()));
    }

}



void main_thread::run()
   {
       std::chrono::duration<double> time_loop_passed;
       const double kTimePerLoop=10.0;
       long int velocity_prev[3] = {0,0,0};
       long int velocity_now[3] = {0,0,0};
       long int velocity_diff[3] = {0,0,0};
       int torque_prev[3] = {0,0,0};
       int torque_now[3] = {0,0,0};
       int torque_diff[3] = {0,0,0};
       std::deque<int> torque_joint1;
       std::deque<int> torque_joint2;
       std::deque<int> torque_joint3;
       std::deque<std::deque<int>>output_torque_joint;
       output_torque_joint.push_back(torque_joint1);
       output_torque_joint.push_back(torque_joint2);
       output_torque_joint.push_back(torque_joint3);
        static int robot_arm_enable_btn_static=0;
        int i=0;
       while(1){
           system("clear");
           auto time_loop_start = std::chrono::steady_clock::now();
           printf("UI_FORCE_CONTROL: %d\n",UI_FORCE_CONTROL);
           print_abRecvData();
           print_abSendData();
           print_tRecvSDOData();
           print_tSendSDOData();
           printf("status_word: %x %x %x\n",get_status_word(1),get_status_word(2),get_status_word(3));
           printf("control_mode: %x %x %x\n",get_mode(1),get_mode(2),get_mode(3));
           printf("actual_position: %ld %ld %ld\n",get_position(1),get_position(2),get_position(3));
           printf("actual_velocity: %ld %ld %ld\n",get_velocity(1),get_velocity(2),get_velocity(3));
           printf("actual_torque: %d %d %d\n",get_torque(1),get_torque(2),get_torque(3));
           printf("digital_input: %d %d %d\n",get_digital_input(1),get_digital_input(2),get_digital_input(3));
           printf("torque value when pressed: %d %d %d\n",force_control_init[0],force_control_init[1],force_control_init[2]);
            int robot_arm_enable_btn_current=get_digital_input(3);
            printf("static: %d\n",robot_arm_enable_btn_static);
            printf("current: %d\n",robot_arm_enable_btn_current);
            printf("i: %d\n",i);
           if(robot_arm_enable_btn_current!=robot_arm_enable_btn_static){
//               toggle_servo(1);toggle_servo(2);toggle_servo(3);
                toggle_control_mode(1);toggle_control_mode(2);toggle_control_mode(3);
           }
           robot_arm_enable_btn_static=robot_arm_enable_btn_current;
           sync_target_position(1);sync_target_position(2);sync_target_position(3);
           emit setArmEnableButton(robot_arm_enable_btn_current);

           // loop end, prepare for next loop
           for(int i=0;i<3;i++){
               velocity_prev[i]=velocity_now[i];
               torque_prev[i]=torque_now[i];
               velocity_diff[i]=0;
               torque_diff[i]=0;
           }

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



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *window=new MainWindow();
    window->show();

    // robot arm thread
    pthread_t robot_arm_thread_id;
    pthread_create(&robot_arm_thread_id, NULL, robotarmThread, NULL);

    main_thread *mainThred=new main_thread();
    mainThred->start();

    mainThred->connect(mainThred,&main_thread::setArmEnableButton,window,&MainWindow::setArmEnableStatus);
// this->connect(m_sockettemp,&QTcpSocket::readyRead,this,&Peripheral_Device::on_Data_In);


    // main_thead:mainly for printing
  //  pthread_t main_thread_id;
  //  pthread_create(&main_thread_id, NULL, mainThread, NULL);

    return a.exec();
}
