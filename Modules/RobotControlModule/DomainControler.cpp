#include "DomainControler.h"

auto time_point_rp_start_pd = std::chrono::steady_clock::now();
auto time_point_rp_end_pd = std::chrono::steady_clock::now();
std::chrono::duration<double> time_loop_rp_pd;

DomainControler::DomainControler(uint32_t ID)
{
    this->m_ID=ID;
    this->type=DEV_DOMAINCONTROLER;
    this->Qhash_Cmd_Classify.insert("READWRITEDATA",DOMAINCONTROLER_READ_WRITE_DATA);
    this->Qhash_Cmd_Classify.insert("RESET",DOMAINCONTROLER_RESET);

    openSerialPort(921600);
}


DomainControler::DomainControler(QString ip,quint16 port,uint32_t ID):Peripheral_Device(ip,port)
{
    this->m_ID=ID;
    this->type=DEV_DOMAINCONTROLER;

    this->Qhash_Cmd_Classify.insert("READWRITEDATA",DOMAINCONTROLER_READ_WRITE_DATA);
    this->Qhash_Cmd_Classify.insert("RESET",DOMAINCONTROLER_RESET);

    connect(this,&DomainControler::NewDataIn,this,&DomainControler::readHandleOtherData);
}

void DomainControler::openSerialPort(qint32 baud)
{
     m_serial_422=new QSerialPort();
     QString name="/dev/ttyXR1";
     m_serial_422->setPortName(name);
     m_serial_422->setBaudRate(baud);
     m_serial_422->setDataBits(QSerialPort::Data8);
     m_serial_422->setParity(QSerialPort::NoParity);
     m_serial_422->setStopBits(QSerialPort::OneStop);
     m_serial_422->setFlowControl(QSerialPort::NoFlowControl);
     if (m_serial_422->open(QIODevice::ReadWrite)) {
        //LOG(INFO)
         qDebug()<<"422 open successful";
     } else {
        //LOG(ERROR)
         qDebug()<<"422 open fail";
     }
    connect(m_serial_422, &QSerialPort::readyRead, this, &DomainControler::On422DataIn);
}

void DomainControler::closeSerialPort()
{
    m_serial_422->close();
}

int DomainControler::findFrameHead(QByteArray &data)
{
    uint8_t fh0=0;
    uint8_t fh1=0;
    uint8_t fh2=0;
    uint8_t fh3=0;
    int len=data.length();
    if(len<4){
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
    return -1;
}


void DomainControler::On422DataIn(void)
{
    if(m_serial_422->canReadLine())
    {

        Data422Recvin+=m_serial_422->readAll();
        int len=this->Data422Recvin.length();
        int headindex=findFrameHead(Data422Recvin);
        if(headindex<0)
        {
                return;
        }
        else if(headindex>0)
        {
            Data422Recvin.remove(0,headindex);
            return;
        }


        while(len>=50){
            if((this->Data422Recvin.at(48)==0x0D)&&((this->Data422Recvin.at(49)==0x0a)))
            {
                QByteArray datatemp=Data422Recvin.left(50);
                readHandleOtherData(datatemp);

            }
            Data422Recvin.remove(0,50);
            len=this->Data422Recvin.length();
        }
    }
}

eSendReturn DomainControler::Send_Frame_By_422(COMMU_FRAME cftemp)
{
    QByteArray DataQBA;
    this->Packet_Frame(DataQBA,cftemp);
//    qDebug()<<"length="<<DataQBA.length();
//    qDebug() << DataQBA.toHex(' ').toUpper();
    m_serial_422->write(DataQBA.data(),DataQBA.length());
    return SEND_SUCCESS;
}



float DomainControler::Uint8ArrToFloat(uint8_t *arr,unsigned char StartIndex)
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

void DomainControler::VCMD(QString cmd,int arg1,int arg2,int arg3)
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
        case DOMAINCONTROLER_READ_WRITE_DATA:
        {
            action1[1]=0;
            actionLen=1;
            cf.payload.type=CMD_READ_WRITE;
            break;
        }
        case DOMAINCONTROLER_RESET:
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



void DomainControler::reset()
{
    this->VCMD("RESET");
}


void DomainControler::read_Write_Data()
{
    this->VCMD("READWRITEDATA");
}

void DomainControler::readHandleOtherData(QByteArray qba)
{
    COMMU_FRAME cftemp;

    uint16_t Handle_Angle_LEFT=0;
    uint16_t Handle_Angle_RIGHT=0;
    uint16_t Handle_Key_LEFT=0;
    uint16_t Handle_Key_RIGHT=0;
    uint8_t  FootPad=0;
    uint32_t ORTemp=0;
//    PosDataFromViper  EthernetDataTmp;
//    handleOtherData handleOtherDataTmp;

    eDepackReturn edr=this->Depack_Frame(qba,cftemp);
    if(edr==Depack_SUCCESS)
    {

        this->SelfCheckOK = true;
//        EthernetDataTmp.viperDataL.viperData_X = Uint8ArrToFloat(cftemp.payload.args,10);
//        EthernetDataTmp.viperDataL.viperData_Y = Uint8ArrToFloat(cftemp.payload.args,14);
//        EthernetDataTmp.viperDataL.viperData_Z = Uint8ArrToFloat(cftemp.payload.args,18);
//        EthernetDataTmp.viperDataL.viperData_Arzimuth = Uint8ArrToFloat(cftemp.payload.args,22);
//        EthernetDataTmp.viperDataL.viperData_Elevation = Uint8ArrToFloat(cftemp.payload.args,26);
//        EthernetDataTmp.viperDataL.viperData_Roll = Uint8ArrToFloat(cftemp.payload.args,30);

//        EthernetDataTmp.viperDataR.viperData_X = Uint8ArrToFloat(cftemp.payload.args,38);
//        EthernetDataTmp.viperDataR.viperData_Y = Uint8ArrToFloat(cftemp.payload.args,42);
//        EthernetDataTmp.viperDataR.viperData_Z = Uint8ArrToFloat(cftemp.payload.args,46);
//        EthernetDataTmp.viperDataR.viperData_Arzimuth = Uint8ArrToFloat(cftemp.payload.args,50);
//        EthernetDataTmp.viperDataR.viperData_Elevation = Uint8ArrToFloat(cftemp.payload.args,54);
//        EthernetDataTmp.viperDataR.viperData_Roll = Uint8ArrToFloat(cftemp.payload.args,58);

//        qDebug() << "EthernetDataTmp.viperDataL.viperData_X: " <<EthernetDataTmp.viperDataL.viperData_X;
       // FootPad=cftemp.payload.args[9];
//        handleOtherDataTmp.pedalValueIO = FootPad;
//        qDebug() << "handleOtherDataTmp.pedalValueIO: " <<handleOtherDataTmp.pedalValueIO;
        if(cftemp.payload.args[0]==Dev_Sta_OK)
        {

            // MagneticScale_Counter|=cftemp.payload.args[1];
            // MagneticScale_Counter<<=8;
            // MagneticScale_Counter|=cftemp.payload.args[2];
            // MagneticScale_Counter<<=8;
            // MagneticScale_Counter|=cftemp.payload.args[3];
            // MagneticScale_Counter<<=8;
            // MagneticScale_Counter|=cftemp.payload.args[4];

            MagneticScale_Counter|=cftemp.payload.args[4];
            MagneticScale_Counter<<=8;
            MagneticScale_Counter|=cftemp.payload.args[3];
            MagneticScale_Counter<<=8;
            MagneticScale_Counter|=cftemp.payload.args[2];
            MagneticScale_Counter<<=8;
            MagneticScale_Counter|=cftemp.payload.args[1];

         std::ofstream outfile("ForceSensor.txt",std::ios::app);//记录力传感器数据
         outfile<<MagneticScale_Counter<<"\n";
         outfile.close();
            ForceSensor_Fx=Uint8ArrToFloat(cftemp.payload.args,5);
            ForceSensor_Fy=Uint8ArrToFloat(cftemp.payload.args,9);
            ForceSensor_Fz=Uint8ArrToFloat(cftemp.payload.args,13);
            ForceSensor_Mx=Uint8ArrToFloat(cftemp.payload.args,17);
            ForceSensor_My=Uint8ArrToFloat(cftemp.payload.args,21);
            ForceSensor_Mz=Uint8ArrToFloat(cftemp.payload.args,25);

            DI_val=cftemp.payload.args[29];

            // qDebug()<<"MagneticScale_Counter="<<MagneticScale_Counter;

            // qDebug()<<"ForceSensor_Fx="<<ForceSensor_Fx;
            // qDebug()<<"ForceSensor_Fy="<<ForceSensor_Fy;
            // qDebug()<<"ForceSensor_Fz="<<ForceSensor_Fz;
            // qDebug()<<"ForceSensor_Mx="<<ForceSensor_Mx;
            // qDebug()<<"ForceSensor_My="<<ForceSensor_My;
            // qDebug()<<"ForceSensor_Mz="<<ForceSensor_Mz;
            // qDebug()<<" ";
            // qDebug()<<" ";
//            Handle_Angle_LEFT=cftemp.payload.args[2];
//            Handle_Angle_LEFT<<=8;
//            Handle_Angle_LEFT+=cftemp.payload.args[1];
//            Handle_Key_LEFT=cftemp.payload.args[4];
//            Handle_Key_LEFT<<=8;
//            Handle_Key_LEFT+=cftemp.payload.args[3];

//            handleOtherDataTmp.adcValueL = Handle_Angle_LEFT;
//            handleOtherDataTmp.buttonValueL = Handle_Key_LEFT;

//            Handle_Angle_RIGHT=cftemp.payload.args[6];
//            Handle_Angle_RIGHT<<=8;
//            Handle_Angle_RIGHT+=cftemp.payload.args[5];
//            Handle_Key_RIGHT=cftemp.payload.args[8];
//            Handle_Key_RIGHT<<=8;
//            Handle_Key_RIGHT+=cftemp.payload.args[7];


//            std::array<double,2> HandAngle;
//            HandAngle[0] = (Handle_Angle_LEFT-1527.0)/263.0*60.0-13;
//            HandAngle[1] = (Handle_Angle_RIGHT-1527.0)/263.0*60.0;

//            m_Openangle.store(HandAngle);

//            qDebug() << "Handle_Angle_LEFT: " <<Handle_Angle_LEFT;
//            qDebug() << "Handle_Angle_RIGHT: " <<HandAngle[1];
//            handleOtherDataTmp.adcValueR = Handle_Angle_RIGHT;
//            handleOtherDataTmp.buttonValueR = Handle_Key_RIGHT;

        }
        else if(cftemp.payload.args[0]==Dev_Sta_LEFTHANDLE_ERROR)
        {
             //qDebug()<<"LEFTHANDLE_ERROR!";
        }
        else if(cftemp.payload.args[0]==Dev_Sta_RIGHTHANDLE_ERROR)
        {
             //qDebug()<<"RIGHTHANDLE_ERROR!";
        }
        else if(cftemp.payload.args[0]==(Dev_Sta_LEFTHANDLE_ERROR+Dev_Sta_RIGHTHANDLE_ERROR))
        {
           // qDebug()<<"ALLHANDLE_ERROR!";
        }
//        m_HandleOtherData.store(handleOtherDataTmp);
//        auto posDataInSlaveFrame = motionMapping(EthernetDataTmp);
//        m_HandleViperData.store(posDataInSlaveFrame);
        qba.clear();
    }
    else
    {
        //LOG(INFO)<<"Viper_Transmitter Depack_Frame Fail";
        //qDebug()<<QString(qba);
        uint8_t byteemp=0;
         //LOG(INFO)<<"qba.length()="<<qba.length();
//        for(int i=0;i<qba.length();i++){
//            byteemp=qba.at(i);
//            LOG(INFO)<<"qba["<<i<<"]="<<QString::number(byteemp,16).toStdString();
//        }
    }
}
std::array<double,2> DomainControler::returnOpenAngle(){
    return m_Openangle.load();
}

