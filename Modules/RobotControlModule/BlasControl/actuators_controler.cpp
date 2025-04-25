#include "actuators_controler.h"

Actuators_Controler::Actuators_Controler()
{

}


void Actuators_Controler::openSerialPort(qint32 baud)
{
     m_serial_422=new QSerialPort();

     QString name="/dev/ttyUSB2";
     m_serial_422->setPortName(name);
     m_serial_422->setBaudRate(baud);
     m_serial_422->setDataBits(QSerialPort::Data8);
     m_serial_422->setParity(QSerialPort::NoParity);
     m_serial_422->setStopBits(QSerialPort::OneStop);
     m_serial_422->setFlowControl(QSerialPort::NoFlowControl);
     if (m_serial_422->open(QIODevice::ReadWrite)) {
        qDebug()<<"422 open successful";
     } else {
        qDebug()<<"422 open fail";
     }

    connect(m_serial_422, &QSerialPort::readyRead, this, &Actuators_Controler::On422DataIn);

}


void Actuators_Controler::On422DataIn(void)
{
//    std::cout << "after star qserialPort111" <<std::endl;

    if(m_serial_422->canReadLine())
    {
        Data422Received+=m_serial_422->readAll();
        int len=this->Data422Received.length();
        if((this->Data422Received.at(len-2)==0x0D)&&((this->Data422Received.at(len-1)==0x0a)))
        {
            parseDataFrom422(Data422Received);
            Data422Received.clear();
        }
    }
//    std::cout << "after star qserialPort111" <<std::endl;

}

int Actuators_Controler::parseDataFrom422(QByteArray data)
{
    Com_Frame CF_Temp;

    if(Array_To_Com_Frame(&CF_Temp,(uint8_t*)(data.data()))==-1){
        return -1;
    }
//    for(int i=0;i<12;i++){
//        qDebug()<<CF_Temp.Data_Actuator0[i]<<" ";
//    }
//    qDebug()<<"";
//    qDebug()<<"";
    m_Actuator0_State.Current_Position=CF_Temp.Data_Actuator0[1]*256+CF_Temp.Data_Actuator0[0];
    m_Actuator0_State.Current_Current=CF_Temp.Data_Actuator0[3]*256+CF_Temp.Data_Actuator0[2];
    m_Actuator0_State.Current_Force=CF_Temp.Data_Actuator0[5]*256+CF_Temp.Data_Actuator0[4];
    m_Actuator0_State.Current_Speed=CF_Temp.Data_Actuator0[7]*256+CF_Temp.Data_Actuator0[6];
    m_Actuator0_State.Error_Code=CF_Temp.Data_Actuator0[9]*256+CF_Temp.Data_Actuator0[8];
    m_Actuator0_State.Current_Temperature=CF_Temp.Data_Actuator0[11]*256+CF_Temp.Data_Actuator0[10];

    m_Actuator1_State.Current_Position=CF_Temp.Data_Actuator1[1]*256+CF_Temp.Data_Actuator1[0];
    m_Actuator1_State.Current_Current=CF_Temp.Data_Actuator1[3]*256+CF_Temp.Data_Actuator1[2];
    m_Actuator1_State.Current_Force=CF_Temp.Data_Actuator1[5]*256+CF_Temp.Data_Actuator1[4];
    m_Actuator1_State.Current_Speed=CF_Temp.Data_Actuator1[7]*256+CF_Temp.Data_Actuator1[6];
    m_Actuator1_State.Error_Code=CF_Temp.Data_Actuator1[9]*256+CF_Temp.Data_Actuator1[8];
    m_Actuator1_State.Current_Temperature=CF_Temp.Data_Actuator1[11]*256+CF_Temp.Data_Actuator1[10];

    m_Actuator2_State.Current_Position=CF_Temp.Data_Actuator2[1]*256+CF_Temp.Data_Actuator2[0];
    m_Actuator2_State.Current_Current=CF_Temp.Data_Actuator2[3]*256+CF_Temp.Data_Actuator2[2];
    m_Actuator2_State.Current_Force=CF_Temp.Data_Actuator2[5]*256+CF_Temp.Data_Actuator2[4];
    m_Actuator2_State.Current_Speed=CF_Temp.Data_Actuator2[7]*256+CF_Temp.Data_Actuator2[6];
    m_Actuator2_State.Error_Code=CF_Temp.Data_Actuator2[9]*256+CF_Temp.Data_Actuator2[8];
    m_Actuator2_State.Current_Temperature=CF_Temp.Data_Actuator2[11]*256+CF_Temp.Data_Actuator2[10];

    m_Actuator3_State.Current_Position=CF_Temp.Data_Actuator3[1]*256+CF_Temp.Data_Actuator3[0];
    m_Actuator3_State.Current_Current=CF_Temp.Data_Actuator3[3]*256+CF_Temp.Data_Actuator3[2];
    m_Actuator3_State.Current_Force=CF_Temp.Data_Actuator3[5]*256+CF_Temp.Data_Actuator3[4];
    m_Actuator3_State.Current_Speed=CF_Temp.Data_Actuator3[7]*256+CF_Temp.Data_Actuator3[6];
    m_Actuator3_State.Error_Code=CF_Temp.Data_Actuator3[9]*256+CF_Temp.Data_Actuator3[8];
    m_Actuator3_State.Current_Temperature=CF_Temp.Data_Actuator3[11]*256+CF_Temp.Data_Actuator3[10];

    m_Actuator4_State.Current_Position=CF_Temp.Data_Actuator4[1]*256+CF_Temp.Data_Actuator4[0];
    m_Actuator4_State.Current_Current=CF_Temp.Data_Actuator4[3]*256+CF_Temp.Data_Actuator4[2];
    m_Actuator4_State.Current_Force=CF_Temp.Data_Actuator4[5]*256+CF_Temp.Data_Actuator4[4];
    m_Actuator4_State.Current_Speed=CF_Temp.Data_Actuator4[7]*256+CF_Temp.Data_Actuator4[6];
    m_Actuator4_State.Error_Code=CF_Temp.Data_Actuator4[9]*256+CF_Temp.Data_Actuator4[8];
    m_Actuator4_State.Current_Temperature=CF_Temp.Data_Actuator4[11]*256+CF_Temp.Data_Actuator4[10];

    m_Actuator5_State.Current_Position=CF_Temp.Data_Actuator5[1]*256+CF_Temp.Data_Actuator5[0];
    m_Actuator5_State.Current_Current=CF_Temp.Data_Actuator5[3]*256+CF_Temp.Data_Actuator5[2];
    m_Actuator5_State.Current_Force=CF_Temp.Data_Actuator5[5]*256+CF_Temp.Data_Actuator5[4];
    m_Actuator5_State.Current_Speed=CF_Temp.Data_Actuator5[7]*256+CF_Temp.Data_Actuator5[6];
    m_Actuator5_State.Error_Code=CF_Temp.Data_Actuator5[9]*256+CF_Temp.Data_Actuator5[8];
    m_Actuator5_State.Current_Temperature=CF_Temp.Data_Actuator5[11]*256+CF_Temp.Data_Actuator5[10];

    return sizeof(Com_Frame);


}

Actuator_Action_t Actuators_Controler::SetPosition(quint16 pos)
{
    Actuator_Action_t action;
    action.len=5;
    action.Frame[0]=CMD_WR_REGISTER;
    action.Frame[1]=0x24;
    action.Frame[2]=0x00;
    action.Frame[3]=static_cast<uint8_t>(pos);
    action.Frame[4]=static_cast<uint8_t>(pos>>8);
    return action;
}

Actuator_Action_t Actuators_Controler::SetSpeed(quint16 speed)
{
    Actuator_Action_t action;
    action.len=5;
    action.Frame[0]=CMD_WR_REGISTER;
    action.Frame[1]=0x23;
    action.Frame[2]=0x00;
    action.Frame[3]=static_cast<uint8_t>(speed);
    action.Frame[4]=static_cast<uint8_t>(speed>>8);
    return action;
}

Actuator_Action_t Actuators_Controler::SetMode(quint8 mode)
{
    Actuator_Action_t action;
    action.len=5;
    action.Frame[0]=CMD_WR_REGISTER;
    action.Frame[1]=0x20;
    action.Frame[2]=0x00;
    action.Frame[3]=mode;
    action.Frame[4]=0x00;
    return action;
}

int Actuators_Controler::Actuators_Set_Action(Actuator_Action_t Actuator0_Action, Actuator_Action_t Actuator1_Action, Actuator_Action_t Actuator2_Action,
                                              Actuator_Action_t Actuator3_Action, Actuator_Action_t Actuator4_Action, Actuator_Action_t Actuator5_Action)
{
    uint8_t frameHeadOffset=0;

    memset(m_422_SendBuffer,0,MAX422SENDLEN);

#if ADD_PC_FRAME_HEAD//framehead:0x55aaaa55
    frameHeadOffset=4;
    m_422_SendBuffer[0]=0X55;
    m_422_SendBuffer[1]=0XAA;
    m_422_SendBuffer[2]=0XAA;
    m_422_SendBuffer[3]=0X55;

#elif
    frameHeadOffset=0;
#endif

    m_422_SendBuffer[0+frameHeadOffset]=Actuator0_Action.len;
    memcpy(m_422_SendBuffer+frameHeadOffset+1,Actuator0_Action.Frame,Actuator0_Action.len);

    m_422_SendBuffer[12+frameHeadOffset]=Actuator1_Action.len;
    memcpy(m_422_SendBuffer+frameHeadOffset+12+1,Actuator1_Action.Frame,Actuator1_Action.len);

    m_422_SendBuffer[24+frameHeadOffset]=Actuator2_Action.len;
    memcpy(m_422_SendBuffer+frameHeadOffset+12*2+1,Actuator2_Action.Frame,Actuator2_Action.len);

    m_422_SendBuffer[36+frameHeadOffset]=Actuator3_Action.len;
    memcpy(m_422_SendBuffer+frameHeadOffset+12*3+1,Actuator3_Action.Frame,Actuator3_Action.len);

    m_422_SendBuffer[48+frameHeadOffset]=Actuator4_Action.len;
    memcpy(m_422_SendBuffer+frameHeadOffset+12*4+1,Actuator4_Action.Frame,Actuator4_Action.len);

    m_422_SendBuffer[60+frameHeadOffset]=Actuator5_Action.len;
    memcpy(m_422_SendBuffer+frameHeadOffset+12*5+1,Actuator5_Action.Frame,Actuator5_Action.len);


    unsigned int crc=BLS_Get_CRC16(m_422_SendBuffer,MAX422SENDLEN-sizeof(uint16_t)*2);

    m_422_SendBuffer[76]=static_cast<uint8_t>(crc>>8);
    m_422_SendBuffer[77]=static_cast<uint8_t>(crc);
    m_422_SendBuffer[78]=0X0D;
    m_422_SendBuffer[79]=0X0A;

    m_serial_422->write((char*)m_422_SendBuffer,MAX422SENDLEN);

    return  MAX422SENDLEN;
}





















