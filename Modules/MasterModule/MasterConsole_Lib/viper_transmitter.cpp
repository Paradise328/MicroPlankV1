#include "viper_transmitter.h"

void Viper_Transmitter::initDevice()
{

    if(openSerialPort(576000) == true)
    {
        LOG(INFO)<<"422 open successful";
        m_is422Ok = true;
    }else
    {
        LOG(ERROR)<<"422 open fail";
    }

    usleep(5 * 1000);

    m_statusMonitor = std::thread(&Viper_Transmitter::statusMonitor, this);
    m_statusMonitor.detach();
}

bool Viper_Transmitter::openSerialPort(qint32 baud)
{

    system("sudo rm -f /var/lock/LCK..tty*");

    m_serial_422 = new QSerialPort();
    QString name = "/dev/ttyXR0";
    m_serial_422->setPortName(name);
    m_serial_422->setBaudRate(baud);
    m_serial_422->setDataBits(QSerialPort::Data8);
    m_serial_422->setParity(QSerialPort::NoParity);
    m_serial_422->setStopBits(QSerialPort::OneStop);
    m_serial_422->setFlowControl(QSerialPort::NoFlowControl);
    if (m_serial_422->open(QIODevice::ReadWrite))
    {
        connect(m_serial_422, &QSerialPort::readyRead, this, &Viper_Transmitter::On422DataIn);
        LOG(INFO)<<"Connect Successful";
        return true;
    }else
    {
        LOG(INFO)<<"Connect Failed";
        return false;
    }
}

void Viper_Transmitter::closeSerialPort()
{
    m_serial_422->close();
}

int Viper_Transmitter::findFrameHead(QByteArray &data)
{
    uint8_t fh0=0;
    uint8_t fh1=0;
    uint8_t fh2=0;
    uint8_t fh3=0;
    int len=data.length();
    if(len<4){
         qDebug()<<"frame head err  3";
        return -1;
    }
    for(int i=0;i<len-4;i++)
    {

        fh0=data.at(i);
        fh1=data.at(i+1);
        fh2=data.at(i+2);
        fh3=data.at(i+3);
        if(fh0==75 && fh1==87&&fh2==87&&fh3==88){//is frame head  ok?
            return i;
        }
    }
     qDebug()<<"frame head err            4";
    qDebug()<<"len=                "<<len;
     if(len>400)
    {
        for(int i=0;i<400;i++){
             uint8_t u8temp=data.at(i);
            // qDebug()<<"u8temp="<<u8temp;
        }
     }
    return -1;
}

void Viper_Transmitter::On422DataIn(void)
{
    if(m_serial_422->canReadLine())
    {

        Data422Recvin+=m_serial_422->readAll();
        int len=this->Data422Recvin.length();
        int headindex=findFrameHead(Data422Recvin);
        if(headindex<0)
        {
            qDebug()<<"frame head err  1";
            return;
        }
        else if(headindex>0)
        {
            Data422Recvin.remove(0,headindex);
              qDebug()<<"frame head err  2";
            return;
        }


        while(len>=86){
            if((this->Data422Recvin.at(84)==0x0D)&&((this->Data422Recvin.at(85)==0x0a)))
            {
                QByteArray datatemp=Data422Recvin.left(86);
                readHandleData(datatemp);
            }
            Data422Recvin.remove(0,86);
            len=this->Data422Recvin.length();
        }
    }
}

// void Viper_Transmitter::On422DataIn(void)
// {
//     if(m_serial_422->canReadLine())
//     {
//         Data422Recvin += m_serial_422->readAll();
//         int len = this->Data422Recvin.length();
//         if(len >= 4)
//         {
//             uint8_t fh0 = Data422Recvin.at(0);
//             uint8_t fh1 = Data422Recvin.at(1);
//             uint8_t fh2 = Data422Recvin.at(2);
//             uint8_t fh3 = Data422Recvin.at(3);
//             if(fh0!=75||fh1!=87||fh2!=87||fh3!=88){//is frame head  ok?
//                 Data422Recvin.clear();
//                 qDebug()<<"frame head err";
//                 return;
//             }
//             while(len >= 86){
//                 m_communicateTemp.fetch_add(1);
//                 if(m_communicateTemp >= 65536 * 65536 -1)
//                 {
//                     m_communicateTemp = 0;
//                 }
//                 if((this->Data422Recvin.at(84)==0x0D)&&((this->Data422Recvin.at(85)==0x0a)))
//                 {
//                     QByteArray datatemp=Data422Recvin.left(86);
//                     readHandleData(datatemp);
//                 }
//                 else
//                 {
//                     qDebug()<<"frame end err";
//                 }
//                 Data422Recvin.remove(0,86);
//                 len=this->Data422Recvin.length();
//             }
//         }
//         else
//         {
//             qDebug()<<"len < 4````";
//         }
//     }
//     else{
//         qDebug()<<"no LF";
//     }
// }

eSendReturn Viper_Transmitter::Send_Frame_By_422(COMMU_FRAME cftemp)
{
    QByteArray DataQBA;
    this->Packet_Frame(DataQBA,cftemp);
    m_serial_422->write(DataQBA.data(),DataQBA.length());
    return SEND_SUCCESS;
}


float Viper_Transmitter::Uint8ArrToFloat(uint8_t *arr,unsigned char StartIndex)
{
    unsigned char floatdat[4]={0};
    float f=0;
    floatdat[0]=arr[StartIndex];
    floatdat[1]=arr[StartIndex+1];
    floatdat[2]=arr[StartIndex+2];
    floatdat[3]=arr[StartIndex+3];
    memcpy(&f,floatdat,4);
    return f;
}

void Viper_Transmitter::VCMD(QString cmd,int arg1,int arg2,int arg3)
{

    cmd=cmd.toUpper();
    QStringList slist=cmd.split("=");

    COMMU_FRAME cf;
    memset(&cf,0,sizeof (COMMU_FRAME));
    uint8_t action1[2]={0};
    int actionLen=2;
    action1[0]=this->Qhash_Cmd_Classify[slist[0]];

    switch (this->Qhash_Cmd_Classify[slist[0]])
    {
        case VIPER_TRANSMITTER_GET_SINGLE_DATA:
        {
            action1[1]=0;
            actionLen=1;
            cf.payload.type=CMD_READ_DATA;
            break;
        }
        case VIPER_TRANSMITTER_RESET_VIPER:
        {
            action1[1]=0;
            actionLen=1;
            cf.payload.type=CMD_CTRL_WRITE;
            break;
        }
        case VIPER_TRANSMITTER_RESET_HANDLE:
        {
            action1[1]=0;
            actionLen=1;
            cf.payload.type=CMD_CTRL_WRITE;
            break;
        }
        case VIPER_TRANSMITTER_START_CONTINUS:
        {
            action1[1]=0;
            actionLen=1;
            cf.payload.type=CMD_CTRL_WRITE;
            break;
        }
        default:break;
    }

    if(arg1!=-1)actionLen++;
    if(arg2!=-1)actionLen++;
    if(arg3!=-1)actionLen++;

    uint8_t payloadsize=3+actionLen;
    uint8_t framelen=17+payloadsize;

    cf.preamble=PREAMBLE;
    cf.ID=this->GetRandNum();
    cf.size=framelen;
    cf.sender=DEV_HOST;

    cf.recever=this->type;

    cf.payload.size=payloadsize;
    cf.payload.argscounter=actionLen;
    unsigned int j=0;
    for(j=0;j<sizeof(action1);j++)
    {cf.payload.args[j]=action1[j];}

    if(arg1!=-1){cf.payload.args[j]=(uint8_t)arg1;j++;}
    if(arg2!=-1){cf.payload.args[j]=(uint8_t)arg2;j++;}
    if(arg3!=-1){cf.payload.args[j]=(uint8_t)arg3;j++;}

    // Send_Frame(cf);
    Send_Frame_By_422(cf);
}

void Viper_Transmitter::Reset_Viper()
{
    this->VCMD("RESETVIPER");
}

void Viper_Transmitter::StartContinus()
{
    this->VCMD("STARTCONTINUS");
}

void Viper_Transmitter::readHandleData(QByteArray qba)
{
    COMMU_FRAME cftemp;

    uint16_t Handle_Angle_LEFT = 0;
    uint16_t Handle_Angle_RIGHT = 0;
    uint16_t Handle_Key_LEFT = 0;
    uint16_t Handle_Key_RIGHT = 0;

    HandlePose handlePoseTmp;

    eDepackReturn edr = this->Depack_Frame(qba,cftemp);

    if(edr == Depack_SUCCESS)
    {
        handlePoseTmp.handlePoseL_X = Uint8ArrToFloat(cftemp.payload.args,10);
        handlePoseTmp.handlePoseL_Y = Uint8ArrToFloat(cftemp.payload.args,14);
        handlePoseTmp.handlePoseL_Z = Uint8ArrToFloat(cftemp.payload.args,18);
        handlePoseTmp.handlePoseL_Arzimuth = Uint8ArrToFloat(cftemp.payload.args,22);
        handlePoseTmp.handlePoseL_Elevation = Uint8ArrToFloat(cftemp.payload.args,26);
        handlePoseTmp.handlePoseL_Roll = Uint8ArrToFloat(cftemp.payload.args,30);

        handlePoseTmp.handlePoseR_X = Uint8ArrToFloat(cftemp.payload.args,38);
        handlePoseTmp.handlePoseR_Y = Uint8ArrToFloat(cftemp.payload.args,42);
        handlePoseTmp.handlePoseR_Z = Uint8ArrToFloat(cftemp.payload.args,46);
        handlePoseTmp.handlePoseR_Arzimuth = Uint8ArrToFloat(cftemp.payload.args,50);
        handlePoseTmp.handlePoseR_Elevation = Uint8ArrToFloat(cftemp.payload.args,54);
        handlePoseTmp.handlePoseR_Roll = Uint8ArrToFloat(cftemp.payload.args,58);

        handlePoseTmp.stepPedal = cftemp.payload.args[9];
        // qDebug()<<"cftemp.payload.args[0]: " <<cftemp.payload.args[0] ;
        if(cftemp.payload.args[0] == Dev_Sta_OK)
        {
            Handle_Angle_LEFT = cftemp.payload.args[2];
            Handle_Angle_LEFT<<=8;
            Handle_Angle_LEFT += cftemp.payload.args[1];
            Handle_Key_LEFT = cftemp.payload.args[4];
            Handle_Key_LEFT <<= 8;
            Handle_Key_LEFT += cftemp.payload.args[3];
            Handle_Angle_RIGHT=cftemp.payload.args[6];
            Handle_Angle_RIGHT<<=8;
            Handle_Angle_RIGHT+=cftemp.payload.args[5];
            Handle_Key_RIGHT=cftemp.payload.args[8];
            Handle_Key_RIGHT<<=8;
            Handle_Key_RIGHT+=cftemp.payload.args[7];

            auto openAngle = calculateOpenAngle(Handle_Angle_LEFT, Handle_Angle_RIGHT);
            handlePoseTmp.handlePoseL_OpenAngle = openAngle[0];
            handlePoseTmp.handlePoseR_OpenAngle = openAngle[1];

        }
        else if(cftemp.payload.args[0]== Dev_Sta_LEFTHANDLE_ERROR)
        {
            std::cout <<"LEFTHANDLE_ERROR!" << std::endl;
        }
        else if(cftemp.payload.args[0]==Dev_Sta_RIGHTHANDLE_ERROR)
        {
            std::cout <<"RIGHTHANDLE_ERROR!" << std::endl;
        }
        else if(cftemp.payload.args[0]==(Dev_Sta_LEFTHANDLE_ERROR+Dev_Sta_RIGHTHANDLE_ERROR))
        {
            std::cout<<"ALLHANDLE_ERROR!" << std::endl;
        }
        m_handlePoseData.store(handlePoseTmp);
        qba.clear();
    }
    else
    {
        LOG(INFO)<<"Viper_Transmitter Depack_Frame Fail";
    }
}

void Viper_Transmitter::readHandleData_Quaternion(QByteArray qba)
{
    COMMU_FRAME cftemp;

    uint16_t Handle_Angle_LEFT = 0;
    uint16_t Handle_Angle_RIGHT = 0;
    uint16_t Handle_Key_LEFT = 0;
    uint16_t Handle_Key_RIGHT = 0;

    HandlePose handlePoseTmp;

    eDepackReturn edr = this->Depack_Frame(qba,cftemp);

    if(edr == Depack_SUCCESS)
    {
        handlePoseTmp.handlePoseL_X = Uint8ArrToFloat(cftemp.payload.args,10);
        handlePoseTmp.handlePoseL_Y = Uint8ArrToFloat(cftemp.payload.args,14);
        handlePoseTmp.handlePoseL_Z = Uint8ArrToFloat(cftemp.payload.args,18);
        handlePoseTmp.handlePoseL_Arzimuth = Uint8ArrToFloat(cftemp.payload.args,22);
        handlePoseTmp.handlePoseL_Elevation = Uint8ArrToFloat(cftemp.payload.args,26);
        handlePoseTmp.handlePoseL_Roll = Uint8ArrToFloat(cftemp.payload.args,30);

        handlePoseTmp.handlePoseR_X = Uint8ArrToFloat(cftemp.payload.args,38);
        handlePoseTmp.handlePoseR_Y = Uint8ArrToFloat(cftemp.payload.args,42);
        handlePoseTmp.handlePoseR_Z = Uint8ArrToFloat(cftemp.payload.args,46);
        handlePoseTmp.handlePoseR_Arzimuth = Uint8ArrToFloat(cftemp.payload.args,50);
        handlePoseTmp.handlePoseR_Elevation = Uint8ArrToFloat(cftemp.payload.args,54);
        handlePoseTmp.handlePoseR_Roll = Uint8ArrToFloat(cftemp.payload.args,58);

        handlePoseTmp.stepPedal = cftemp.payload.args[9];

        if(cftemp.payload.args[0] == Dev_Sta_OK)
        {
            Handle_Angle_LEFT = cftemp.payload.args[2];
            Handle_Angle_LEFT<<=8;
            Handle_Angle_LEFT += cftemp.payload.args[1];
            Handle_Key_LEFT = cftemp.payload.args[4];
            Handle_Key_LEFT <<= 8;
            Handle_Key_LEFT += cftemp.payload.args[3];
            Handle_Angle_RIGHT=cftemp.payload.args[6];
            Handle_Angle_RIGHT<<=8;
            Handle_Angle_RIGHT+=cftemp.payload.args[5];
            Handle_Key_RIGHT=cftemp.payload.args[8];
            Handle_Key_RIGHT<<=8;
            Handle_Key_RIGHT+=cftemp.payload.args[7];
            qDebug()<< "Handle_Key_LEFT: " << Handle_Key_LEFT << " Handle_Key_Right" << Handle_Key_RIGHT;

            auto openAngle = calculateOpenAngle(Handle_Angle_LEFT, Handle_Angle_RIGHT);

            handlePoseTmp.handlePoseL_OpenAngle = openAngle[0];
            handlePoseTmp.handlePoseR_OpenAngle = openAngle[1];
        }
        else if(cftemp.payload.args[0] == Dev_Sta_LEFTHANDLE_ERROR)
        {
            std::cout <<"LEFTHANDLE_ERROR!" << std::endl;
        }
        else if(cftemp.payload.args[0] == Dev_Sta_RIGHTHANDLE_ERROR)
        {
            std::cout <<"RIGHTHANDLE_ERROR!" << std::endl;
        }
        else if(cftemp.payload.args[0] == (Dev_Sta_LEFTHANDLE_ERROR+Dev_Sta_RIGHTHANDLE_ERROR))
        {
            std::cout<<"ALLHANDLE_ERROR!" << std::endl;
        }
        m_handlePoseData.store(handlePoseTmp);
        qba.clear();
    }
    else
    {
        LOG(INFO)<<"Viper_Transmitter Depack_Frame Fail";
    }
}

std::array<double, 2> Viper_Transmitter::calculateOpenAngle(const uint16_t& adcValueL, const uint16_t& adcValueR) const
{
    double delt_adcValueL = 0;
    double delt_adcValueR = 0;
    std::array<double, 2> openAngle = {0,0};
    delt_adcValueL = (adcValueL - adcValueClose_L);
    if(delt_adcValueL < 0)
    {
        delt_adcValueL = 0;
    }
    if(delt_adcValueL >= abs(adcValueOpen_L - adcValueClose_L))
    {
        delt_adcValueL = abs(adcValueOpen_L - adcValueClose_L);
    }

    delt_adcValueR = (adcValueR - adcValueClose_R);
    if(delt_adcValueR < 0)
    {
        delt_adcValueR = 0;
    }
    if(delt_adcValueR >= abs(adcValueOpen_R - adcValueClose_R))
    {
        delt_adcValueR = abs(adcValueOpen_R - adcValueClose_R);
    }
    // auto angle_0 = (delt_adcValueL/abs(adcValueOpen_L - adcValueClose_L)) * 30 - 10;
    // auto angle_1 = (delt_adcValueR/abs(adcValueOpen_R - adcValueClose_R)) * 30 - 10;
    // openAngle[0] = (angle_0 < 0) ? 0.008 * pow(angle_0, 3) : pow(angle_0, 3)/400;
    // openAngle[1] = (angle_1 < 0) ? 0.008 * pow(angle_1, 3) : pow(angle_1, 3)/400;
    openAngle[0] = (delt_adcValueL/abs(adcValueOpen_L - adcValueClose_L)) * 30 - 10;
    openAngle[1] = (delt_adcValueR/abs(adcValueOpen_R - adcValueClose_R)) * 30 - 10;
    return openAngle;
}

void Viper_Transmitter::DataIn(QByteArray data)
{
    if(QString(data).contains("VIPER_Transmitter"))
    {
        qDebug()<<QString(data);
        return;
    }
    COMMU_FRAME cftemp;
    eDepackReturn edr=this->Depack_Frame(data,cftemp);
    if(edr==Depack_SUCCESS)
    {
        qDebug()<<"Depack_SUCCESS";
        if(cftemp.payload.type==CMD_TEST&&cftemp.payload.args[0]==0x00)
        {
            disconnect(this,&Viper_Transmitter::NewDataIn,this,&Viper_Transmitter::DataIn);
        }
    }
}

void Viper_Transmitter::statusMonitor()
{
    while(!m_isMonitorTerminated)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if(m_communicateTemp == m_communicateReserve)
        {
            m_is422Ok = false;
        }
        else{
            m_is422Ok = true;
            m_communicateReserve = m_communicateTemp;
        }
    }
}

