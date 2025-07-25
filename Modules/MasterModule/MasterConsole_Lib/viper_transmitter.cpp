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

    HandlePose handlePoseTmp, handlePoseCur;

    eDepackReturn edr = this->Depack_Frame(qba,cftemp);

    if(edr == Depack_SUCCESS)
    {
        std::array<std::array<double, 7>, 2> viperDataTmp = {0};
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < 6; j++)
            {
                viperDataTmp[i][j] = Uint8ArrToFloat(cftemp.payload.args,(28 * i + 4 * j + 10));
            }
        }
        int stepPedal = cftemp.payload.args[9];
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
            handlePoseTmp = motionMapping(viperDataTmp, openAngle, stepPedal);
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

HandlePose Viper_Transmitter::motionMapping(const std::array<std::array<double,viperDataNumPerSensor>,2>& viperData, const std::array<double,2>& openAngle, const int& stepPedal)
{

    double arzimuth_Cur_R   = viperData[1][3] *  M_PI / 180;
    double Elevation_Cur_R  = viperData[1][4]  *  M_PI / 180;
    double Roll_Cur_R       = viperData[1][5]  *  M_PI / 180;

    double arzimuth_Cur_L   = viperData[0][3]  *  M_PI / 180;
    double Elevation_Cur_L  = viperData[0][4] *  M_PI / 180;
    double Roll_Cur_L       = viperData[0][5] *  M_PI / 180;

    //Define the Euler rotation Matrix
    Eigen::Matrix3d rotAroundZ_R, rotAroundY_R, rotAroundX_R;
    Eigen::Matrix3d rotAroundZ_L, rotAroundY_L, rotAroundX_L;
    Eigen::Matrix3d rotAroundX_Init_R, rotAroundY_Init_R;
    Eigen::Matrix3d rotAroundX_Init_L, rotAroundY_Init_L;
    Eigen::Matrix3d mappingMatrix;
    mappingMatrix << 0, 0, 1,
        0, 1, 0,
        -1, 0, 0;

    rotAroundZ_R = Eigen::AngleAxisd(arzimuth_Cur_R, Eigen::Vector3d::UnitZ());
    rotAroundY_R = Eigen::AngleAxisd(Elevation_Cur_R, Eigen::Vector3d::UnitY());
    rotAroundX_R = Eigen::AngleAxisd(Roll_Cur_R, Eigen::Vector3d::UnitX());

    rotAroundZ_L = Eigen::AngleAxisd(arzimuth_Cur_L, Eigen::Vector3d::UnitZ());
    rotAroundY_L = Eigen::AngleAxisd(Elevation_Cur_L, Eigen::Vector3d::UnitY());
    rotAroundX_L = Eigen::AngleAxisd(Roll_Cur_L, Eigen::Vector3d::UnitX());

    //Initial two rotations
    rotAroundX_Init_R = Eigen::AngleAxisd(-m_armAnglePerSide / 180 * M_PI, Eigen::Vector3d::UnitX());//armAnglePerSide=M_PI / 6    M_PI / 12
    rotAroundY_Init_R = Eigen::AngleAxisd(-M_PI / 3, Eigen::Vector3d::UnitY());

    rotAroundX_Init_L = Eigen::AngleAxisd( m_armAnglePerSide / 180 * M_PI, Eigen::Vector3d::UnitX());// M_PI / 6
    rotAroundY_Init_L = Eigen::AngleAxisd(-M_PI / 3, Eigen::Vector3d::UnitY());

    Eigen::Matrix3d rotMatrix_R = rotAroundX_Init_R * rotAroundY_Init_R * rotAroundZ_R * rotAroundY_R * rotAroundX_R * mappingMatrix;
    Eigen::Matrix3d rotMatrix_L = rotAroundX_Init_L * rotAroundY_Init_L * rotAroundZ_L * rotAroundY_L * rotAroundX_L * mappingMatrix;

    double alpha_R = atan2(rotMatrix_R(1, 0), rotMatrix_R(0, 0));
    double beta_R = atan2(-rotMatrix_R(2,0), sqrt(rotMatrix_R(2,1) * rotMatrix_R(2,1) + rotMatrix_R(2,2) * rotMatrix_R(2,2)));
    double gamma_R = atan2(rotMatrix_R(2,1), rotMatrix_R(2,2));

    double alpha_L = atan2(rotMatrix_L(1, 0), rotMatrix_L(0, 0));
    double beta_L = atan2(-rotMatrix_L(2,0), sqrt(rotMatrix_L(2,1) * rotMatrix_L(2,1) + rotMatrix_L(2,2) * rotMatrix_L(2,2)));
    double gamma_L = atan2(rotMatrix_L(2,1), rotMatrix_L(2,2));

    if(alpha_R > 160 * M_PI / 180)
    {
        alpha_R  = 160 * M_PI / 180;
    }else if(alpha_R * 180 / M_PI < -160)
    {
        alpha_R  = -160 * M_PI / 180;
    }

    if(beta_R >  80 * M_PI / 180)
    {
        beta_R = 80 * M_PI / 180;
    }else if(beta_R < -80 * M_PI / 180)
    {
        beta_R = -80 * M_PI / 180;
    }

    if(gamma_R > 160 * M_PI / 180)
    {
        gamma_R = 160 * M_PI / 180;
    }else if(gamma_R < -160 * M_PI / 180)
    {
        gamma_R = -160 * M_PI / 180;
    }

    if(alpha_L > 160 * M_PI / 180)
    {
        alpha_L = 160 * M_PI / 180;
    }else if(alpha_L < -160 * M_PI / 180)
    {
        alpha_L = -160 * M_PI / 180;
    }

    if(beta_L > 80 * M_PI / 180)
    {
        beta_L = 80 * M_PI / 180;
    }else if(beta_L < -80 * M_PI / 180)
    {
        beta_L = -80 * M_PI / 180;
    }

    if(gamma_L > 160 * M_PI / 180)
    {
        gamma_L = 160 * M_PI / 180;
    }else if(gamma_L < -160 * M_PI / 180)
    {
        gamma_L = -160 * M_PI / 180;
    }

    HandlePose poseDataCurInSlaveFrame;

    poseDataCurInSlaveFrame.handlePoseR_Roll = alpha_R;
    poseDataCurInSlaveFrame.handlePoseR_Elevation = beta_R;
    poseDataCurInSlaveFrame.handlePoseR_Arzimuth = gamma_R;

    poseDataCurInSlaveFrame.handlePoseL_Roll = alpha_L;
    poseDataCurInSlaveFrame.handlePoseL_Elevation = beta_L;
    poseDataCurInSlaveFrame.handlePoseL_Arzimuth = gamma_L;

    Eigen::Vector3d  masterPositionViaSensor;
    masterPositionViaSensor << -2.0,
                                0,
                               2.0;

    Eigen::Vector3d  sensorPosition_L, sensorPosition_R;
    sensorPosition_L << viperData[0][0],
                        viperData[0][1],
                        viperData[0][2];

    sensorPosition_R << viperData[1][0],
                        viperData[1][1],
                        viperData[1][2];

    Eigen::Matrix3d rotAroundWorldY;
    rotAroundWorldY = Eigen::AngleAxisd(-M_PI / 6, Eigen::Vector3d::UnitY());

    Eigen::Vector3d  endPosition_L, endPosition_R;
    endPosition_L = rotAroundWorldY * (sensorPosition_L + rotAroundZ_L * rotAroundY_L * rotAroundX_L * masterPositionViaSensor);
    endPosition_R = rotAroundWorldY * (sensorPosition_R + rotAroundZ_R * rotAroundY_R * rotAroundX_R * masterPositionViaSensor);

    poseDataCurInSlaveFrame.handlePoseL_X = endPosition_L[0];
    poseDataCurInSlaveFrame.handlePoseL_Y = endPosition_L[1];
    poseDataCurInSlaveFrame.handlePoseL_Z = endPosition_L[2];

    poseDataCurInSlaveFrame.handlePoseR_X= endPosition_R[0];
    poseDataCurInSlaveFrame.handlePoseR_Y = endPosition_R[1];
    poseDataCurInSlaveFrame.handlePoseR_Z = endPosition_R[2];

    poseDataCurInSlaveFrame.handlePoseL_OpenAngle = openAngle[0];
    poseDataCurInSlaveFrame.handlePoseR_OpenAngle = openAngle[1];

    poseDataCurInSlaveFrame.stepPedal = stepPedal;

    return poseDataCurInSlaveFrame;
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
    std::array<double, 2> openAngle = {0,0};
    double adcValueL_Tmp = adcValueL;
    double adcValueR_Tmp = adcValueR;

    if(adcValueL_Tmp > adcValueOpen_L) {adcValueL_Tmp = adcValueOpen_L;}
    if(adcValueL_Tmp < adcValueClose_L) {adcValueL_Tmp = adcValueClose_L;}
    openAngle[0] = (adcValueL_Tmp >= adcValueGrasp_L)? (adcValueL_Tmp - adcValueGrasp_L)/(adcValueOpen_L - adcValueGrasp_L) * 20.0 : (adcValueL_Tmp - adcValueGrasp_L)/(adcValueGrasp_L - adcValueClose_L) * 10.0;

    if(adcValueR_Tmp > adcValueOpen_R) {adcValueR_Tmp = adcValueOpen_R;}
    if(adcValueR_Tmp < adcValueClose_R) {adcValueR_Tmp = adcValueClose_R;}
    openAngle[1] = (adcValueR_Tmp >= adcValueGrasp_R)? (adcValueR_Tmp - adcValueGrasp_R)/(adcValueOpen_R - adcValueGrasp_R) * 20.0 : (adcValueR_Tmp - adcValueGrasp_R)/(adcValueGrasp_R - adcValueClose_R) * 10.0;
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
