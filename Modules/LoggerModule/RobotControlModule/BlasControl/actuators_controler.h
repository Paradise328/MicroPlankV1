#ifndef ACTUATORS_CONTROLER_H
#define ACTUATORS_CONTROLER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include "communication.h"
#include "string.h"
#include <iostream>

typedef  struct{
uint8_t Frame[20];
uint8_t len;
}Actuator_Action_t;

#define MAX422SENDLEN 80

#define ADD_PC_FRAME_HEAD 1

class Actuators_Controler:public QObject
{
    Q_OBJECT
public:
    Actuators_Controler();
    void openSerialPort(qint32 baud);
    void On422DataIn(void);
    int parseDataFrom422(QByteArray data);
    Actuator_Action_t SetPosition(uint16_t pos) ;
    Actuator_Action_t SetSpeed(uint16_t speed) ;
    Actuator_Action_t SetMode(uint8_t mode) ;
    int Actuators_Set_Action(Actuator_Action_t Actuator0_Action,
                             Actuator_Action_t Actuator1_Action,
                             Actuator_Action_t Actuator2_Action,
                             Actuator_Action_t Actuator3_Action,
                             Actuator_Action_t Actuator4_Action,
                             Actuator_Action_t Actuator5_Action);
    Actuator_Status_TypeDef m_Actuator0_State;
    Actuator_Status_TypeDef m_Actuator1_State;
    Actuator_Status_TypeDef m_Actuator2_State;
    Actuator_Status_TypeDef m_Actuator3_State;
    Actuator_Status_TypeDef m_Actuator4_State;
    Actuator_Status_TypeDef m_Actuator5_State;
private:
        QSerialPort *m_serial_422 = nullptr;
        QByteArray  Data422Received;


        uint8_t m_422_SendBuffer[MAX422SENDLEN];
};

#endif // ACTUATORS_CONTROLER_H
