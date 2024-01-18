 #include "Handle.h"
#include<QTime>
#include<cmath>

auto time_point_rp_start = std::chrono::steady_clock::now();
auto time_point_rp_end = std::chrono::steady_clock::now();

std::chrono::duration<double> time_loop_rp;
#define PI 3.14159265
const uint16_t crctable[256] =
{
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

unsigned char get_Pos[]={0x56,0x50,0x52,0x43,0x18,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x00,
                           0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x5F,0xA1,0x00,0x00};
QByteArray arr_getPos((char*)get_Pos,32);
unsigned char set_Units[]={0x56,0x50,0x52,0x43,0x20,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
                           0x02,0x00,0x00,0x00,0xB5,0xB3,0x00,0x00};
QByteArray arr_setUnits((char*)set_Units,40);


uint32_t myCalcCrc16(uint8_t *b, uint32_t len)
{
  uint32_t crc = 0;
  while (len--)
    crc = crctable[(crc ^ *b++) & 0xff] ^ (crc >> 8);
  return crc;

}

//*****************************************************************************//
//*****************************************************************************//


Handle::Handle()
{
    //Initialization of i_count
    i_counter=0;
    this->V=new VIPER_Transmitter("192.168.42.100",10080);
    this->V->m_socket->waitForConnected();
    Handle_wait(500);
    //Open the PCB thread and reset Viepr
    this->V->Reset_Viper();
    Handle_wait(15*1000);


    //Open the viper thread
    m_serial=new QSerialPort(this);
    openSerialPort(115200);

     //set Baudrate
    set_SerialCfg();
    Handle_wait(100);
    m_serial->close();
    m_serial->deleteLater();
    m_serial=new QSerialPort(this);
    openSerialPort(1843200);
    usleep(100*100);
}


void Handle::get_PNO()
{
    this->Revdata.clear();
    if(i_counter>=9)
    {
    }

    send_CMD(arr_getPos);
}

SensorData Handle::return_PNO()
{
 m_mutex.lock();
 SensorData datatemp=Sensor_Data;
 m_mutex.unlock();

 return datatemp;

}

float Handle::QbyteArrayToFloat(QByteArray arr,unsigned char StartIndex)
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

void Handle::OnRespon()
{

    QByteArray data = m_serial->readAll();
    this->Revdata+=data;
    if(this->Revdata.length()==112)
    {
       if(i_counter<10){i_counter++;}
       else
       {
           m_mutex.lock();
           for (int i = 0;i<3;i++)
           {
               Sensor_Data.sensor1_pos[i]=QbyteArrayToFloat(this->Revdata,48+4*i)*2.54;
               Sensor_Data.sensor2_pos[i]=QbyteArrayToFloat(this->Revdata,80+4*i)*2.54;
    //           Sensor_Data.sensor3_pos[i]=QbyteArrayToFloat(this->Revdata,112+4*i)*2.54;
           }
           for (int i = 3;i<6;i++)
           {
               Sensor_Data.sensor1_pos[i]=QbyteArrayToFloat(this->Revdata,48+4*i);
               Sensor_Data.sensor2_pos[i]=QbyteArrayToFloat(this->Revdata,80+4*i);
    //           Sensor_Data.sensor3_pos[i]=QbyteArrayToFloat(this->Revdata,112+4*i);
           }
//           for(int i =0;i<6;i++)
//           {
//               qDebug()<<"Pos112333"<<i<<"pos = "<<Sensor_Data.sensor1_pos[i];
//           }
           m_mutex.unlock();
           emit Viper_Data_Update();
       }
   }

}


void Handle::openSerialPort(qint32 baud)
{
 QString name="/dev/ttyXR1";
 m_serial->setPortName(name);
 m_serial->setBaudRate(baud);
 m_serial->setDataBits(QSerialPort::Data8);
 m_serial->setParity(QSerialPort::NoParity);
 m_serial->setStopBits(QSerialPort::OneStop);
 m_serial->setFlowControl(QSerialPort::NoFlowControl);
 if (m_serial->open(QIODevice::ReadWrite)) {
    qDebug()<<"open successful";
 } else {
     qDebug()<<"open fail";
 }
 connect(m_serial, &QSerialPort::errorOccurred, this, &Handle::handleError);

 connect(m_serial, &QSerialPort::readyRead, this, &Handle::OnRespon);
}

void Handle::handleError(QSerialPort::SerialPortError error)
{
 if (error == QSerialPort::ResourceError)
 {
     qDebug()<<m_serial->errorString();
     if (m_serial->isOpen())
         m_serial->close();
 }
}


void Handle::set_SerialCfg()
{
    const  uint32_t HDR_END_LOC=sizeof(SEUCMD_HDR);
    uint32_t crc;
    uint32_t cmd_size=sizeof(SEUCMD_HDR)+sizeof(SERIAL_CONFIG)+CRC_SIZE;
    uint8_t* cmd_pkg=new uint8_t[cmd_size];
    memset(cmd_pkg,0,cmd_size);

    SEUCMD_HDR* phdr=(SEUCMD_HDR*)cmd_pkg;

    SERIAL_CONFIG sfg;
    sfg.a.sercfg=0;
    sfg.a.bf.baudrate=E_BR_1843200;
    sfg.a.bf.parity=E_PARITY_NONE;//E_PARITY_EVEN;

    phdr->preamble=VIPER_CMD_PREAMBLE;
    phdr->size=cmd_size-8;  // preamble and size not incl in size

    phdr->seucmd.cmd=CMD_SERIAL_CONFIG;
    phdr->seucmd.action=CMD_ACTION_SET;
    memcpy(cmd_pkg+HDR_END_LOC,&sfg,4);

    crc=myCalcCrc16(cmd_pkg,cmd_size-4);  // remove crc size from length

    memcpy(cmd_pkg+HDR_END_LOC+sizeof(SERIAL_CONFIG),&crc,CRC_SIZE);

    QByteArray arr_serialcfg((char*)cmd_pkg,cmd_size);
    send_CMD(arr_serialcfg);

    delete[] cmd_pkg;
    cmd_pkg=nullptr;
}

void Handle::set_Unit()
{
    send_CMD(arr_setUnits);

}

void Handle::init_OrgData()
{
    while(i_counter<10&&this->V->counter_i<10)
    {
        this->get_PNO();
        Handle_wait(15);
    }
    this->get_PNO();
    Handle_wait(15);
    this->Org_Data=this->return_PNO();
    Org_Angle_L=V->org_l;
    Org_Angle_R=V->org_r;
    H_Speed_Pedal = V->Speed_pedal;
}

SensorData Handle::get_OrgData()
{
    return Org_Data;
}

Posture Handle::calc_H_Post(SensorData data)
{
    Posture Handle_Posture;
    for (int i = 0;i<5;i++)
    {
        Handle_Posture.Handle1[i] = data.sensor1_pos[i];
        Handle_Posture.Handle2[i] = data.sensor2_pos[i];
    }

    Handle_Posture.Handle1[5] = data.sensor1_pos[5];
    Handle_Posture.Handle2[5] = data.sensor2_pos[5];
    Handle_Posture.Handle1[6] = -(V->cur_l-this->Org_Angle_L);
    Handle_Posture.Handle2[6] = -(V->cur_r-this->Org_Angle_R);
    Handle_Posture.Pedal_Speed_Index = V->Speed_pedal;
    Handle_Posture.enable_pedal_l = V->Enable_pedal_l;
    Handle_Posture.enable_pedal_r = V->Enable_pedal_r;
    if (Handle_Posture.Handle1[6]<-45)
    {
        Handle_Posture.grasp_index_l=1;
    }else{Handle_Posture.grasp_index_l=0;}
    if (Handle_Posture.Handle2[6]<-45)
    {
        Handle_Posture.grasp_index_r=1;
    }else{Handle_Posture.grasp_index_r=0;}



    for (int i = 0;i<7;i++)
    {
        this->current_Post.Handle1[i]=Handle_Posture.Handle1[i];
        this->current_Post.Handle2[i]=Handle_Posture.Handle2[i];
    }
    current_Post.Pedal_Speed_Index=Handle_Posture.Pedal_Speed_Index;
    current_Post.enable_pedal_l = Handle_Posture.enable_pedal_l;
    current_Post.enable_pedal_r = Handle_Posture.enable_pedal_r;
    current_Post.grasp_index_l = Handle_Posture.grasp_index_l;
    current_Post.grasp_index_r = Handle_Posture.grasp_index_r;
    return Handle_Posture;
}



void Handle::Handle_wait(int wait_time)
{
    QTime timer=QTime::currentTime().addMSecs(wait_time);
            while(QTime::currentTime()<timer)
    {
            QCoreApplication::processEvents(QEventLoop::AllEvents,10);
    }
}

void Handle::send_CMD(const QByteArray &cmd)
{
 m_serial->write(cmd);

 m_serial->waitForBytesWritten();
}

