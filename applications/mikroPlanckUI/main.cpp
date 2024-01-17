#include <QQmlApplicationEngine>
#include <QtQml>
#include <qmllanguage.h>
#include <QQmlContext>
#include <UIinterface.h>
#include "MikroPlanck.h"
#include "main.h"
#include <QWaitCondition>
#include <QApplication>

bool UI_FORCE_CONTROL;
int force_control_init[3]={0,0,0};
int robotarm_enable_btn=0;
<<<<<<< HEAD
=======
GCon g_l=0,g_r = 0;
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202


void robot_arm_thread::run()
   {
       std::chrono::duration<double> time_loop_passed;
       const double kTimePerLoop=10.0;
       long int velocity_prev[3] = {0,0,0};
       long int velocity_now[3] = {0,0,0};
       long int velocity_diff[3] = {0,0,0};
       int torque_prev[3] = {0,0,0};
       int torque_now[3] = {0,0,0};
       int torque_diff[3] = {0,0,0};
<<<<<<< HEAD
       QVector<int> joint_sm = {0,0,0,0,0,0};
=======
       QVector<int> joint_status_mode = {0,0,0,0,0,0};
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
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
       while(1){
           auto time_loop_start = std::chrono::steady_clock::now();

           /*
           system("clear");
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
            printf("static: %d\n",robot_arm_enable_btn_static);
            printf("current: %d\n",robot_arm_enable_btn_current);
            printf("i: %d\n",i);
            */
           int robot_arm_enable_btn_current=get_digital_input(3);

           if(robot_arm_enable_btn_current!=robot_arm_enable_btn_static){
<<<<<<< HEAD
//               toggle_servo(1);toggle_servo(2);toggle_servo(3);
                toggle_control_mode(1);toggle_control_mode(2);toggle_control_mode(3);
           }
           joint_sm[0]=get_status_word(1);
           joint_sm[1]=get_status_word(2);
           joint_sm[2]=get_status_word(3);
           joint_sm[3]=get_mode(1);
           joint_sm[4]=get_mode(2);
           joint_sm[5]=get_mode(3);

=======
                toggle_control_mode(1);toggle_control_mode(2);toggle_control_mode(3);
           }

           {
           joint_status_mode[0]=get_status_word(1);
           joint_status_mode[1]=get_status_word(2);
           joint_status_mode[2]=get_status_word(3);
           joint_status_mode[3]=get_mode(1);
           joint_status_mode[4]=get_mode(2);
           joint_status_mode[5]=get_mode(3);
            }
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
           robot_arm_enable_btn_static=robot_arm_enable_btn_current;
           sync_target_position(1);sync_target_position(2);sync_target_position(3);

     //      emit setArmEnableButton(robot_arm_enable_btn_current);
<<<<<<< HEAD
     //      emit setArmStatusMode(joint_sm);
=======
     //      emit setArmStatusMode(joint_status_mode);
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202

           // loop end, prepare for next loop
           for(int i=0;i<3;i++){
               velocity_prev[i]=velocity_now[i];
               torque_prev[i]=torque_now[i];
               velocity_diff[i]=0;
               torque_diff[i]=0;
           }
            usleep(200);
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

Lifting *lp=nullptr;
LightBoard *lb_ee=nullptr;
UIinterface *Uip=nullptr;

int main(int argc, char *argv[])
{

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QmlLanguage qmlLanguage(app, engine);
    engine.rootContext()->setContextProperty("qmlLanguage", &qmlLanguage);

 //   engine.rootContext()->setContextProperty("tt", &tt);

    QThread thread;
    UIthread uiThread;
    uiThread.moveToThread(&thread);


    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

<<<<<<< HEAD
=======
    char buf[G_SMALL_BUFFER];

    //var used to refer to a unique connection. A valid connection is nonzero.
    char* address_l = "192.168.42.30";  //Retrieve address from command line
    sprintf(buf, "%s --subscribe MG", address_l);
    galil(GOpen(buf, &g_l));

    char* address_r = "192.168.42.40";  //Retrieve address from command line
    sprintf(buf, "%s --subscribe MG", address_r);
    galil(GOpen(buf, &g_r)); //Opens a connection at the provided address

    initGali(g_l);
    initGali(g_r);

>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    lp=new Lifting("192.168.42.60",8080);
    lb_ee=new LightBoard("192.168.42.80",8080);
    lb_ee->LCMD("LSC=green");
    lb_ee->LCMD("LSM=on");

    UIinterface uiInterface(engine);
    Uip=&uiInterface;
    uiInterface.SetLifting(lp);
    uiInterface.SetLightBoard(lb_ee);
    engine.rootContext()->setContextProperty("uiInterface", &uiInterface);

    QObject::connect(engine.rootObjects().first()->findChild<QObject*>("kinematics"),SIGNAL(triggerKinematics(int)),&uiThread,SLOT(onKinematics()));

    thread.start();
   // MikroPlanck *mp=new MikroPlanck();
   // mp->show();

  //   robot arm thread
    pthread_t robot_arm_thread_id;
    pthread_create(&robot_arm_thread_id, NULL, robotarmThread, NULL);

    robot_arm_thread *armThread=new robot_arm_thread();
  //  armThread->connect(armThread,&robot_arm_thread::setArmEnableButton,mp->robot_arm_widget,&RobotArmPage::setArmEnableStatus);
 //   armThread->connect(armThread,&robot_arm_thread::setArmStatusMode,mp->robot_arm_widget,&RobotArmPage::setArmSM);
    armThread->start();

    //read data from magnetic master console
    ReadData *ReadData_Threadp;
<<<<<<< HEAD
    ReadData_Threadp=new ReadData();
=======
    ReadData_Threadp =new ReadData();
>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202
    ReadData_Threadp->start();
    Handle::Handle_wait(20*1000);

    Forcep_Control_Thread *Motion_Control;
    Motion_Control = new Forcep_Control_Thread();
    Motion_Control->connect(Motion_Control,&Forcep_Control_Thread::get_Pos,ReadData_Threadp,&ReadData::Data_output);
    Motion_Control->start();
<<<<<<< HEAD
    uiInterface.connect(&uiInterface,&UIinterface::CurrentChanged,Motion_Control,&Forcep_Control_Thread::onPageChange);


// this->connect(m_sockettemp,&QTcpSocket::readyRead,this,&Peripheral_Device::on_Data_In);
=======

    uiInterface.connect(&uiInterface,&UIinterface::CurrentChanged,Motion_Control,&Forcep_Control_Thread::onPageChange);



>>>>>>> 3c45461684e86476062884fa2ceab5fced37c202

    return app.exec();
}
