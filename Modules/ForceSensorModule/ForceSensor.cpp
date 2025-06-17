#include "ForceSensor.h"

ForceSensor::ForceSensor(QObject* parent):
    F_x(0),F_y(0),F_z(0),M_x(0),M_y(0),M_z(0){

    m_serial_forceSensor = new QSerialPort();
    connect(m_serial_forceSensor, &QSerialPort::readyRead, this, &ForceSensor::handleReadyRead);
}

ForceSensor::~ForceSensor(){
    running = false;
    // pthread_join(threadId, nullptr);
    stopForceSensor();
    if(m_serial_forceSensor->isOpen()){
        m_serial_forceSensor->close();
    }
}

void ForceSensor::initDevice(){
    if(openSerialPort(460800) == true){
        LOG(INFO)<<"===force sensor serial open successful";
    }else{
        LOG(ERROR)<<"===force sensor serial open failed";
    }
    usleep(5 * 1000);

}

bool ForceSensor::openSerialPort(qint32 baudrate)
{
    QString name = "/dev/ttyUSB0";
    m_serial_forceSensor -> setPortName(name);
    m_serial_forceSensor -> setBaudRate(baudrate);
    m_serial_forceSensor -> setDataBits(QSerialPort::Data8);
    m_serial_forceSensor -> setParity(QSerialPort::NoParity);
    m_serial_forceSensor -> setStopBits(QSerialPort::OneStop);
    m_serial_forceSensor -> setFlowControl(QSerialPort::NoFlowControl);
    if (m_serial_forceSensor->open(QIODevice::ReadWrite))
    {
        LOG(INFO)<<"+++Open Force Sensor Port Successful";
        startForceSensor();
        return true;
    } else {
        LOG(INFO)<<"+++Failed to Open Port of Force Sensor";
        return false;
    }
}

void ForceSensor::startForceSensor(){
    LOG(INFO)<<"+++Send Start Force Sensor Command";
    running = true;
    QByteArray cmd = QByteArray::fromHex("48AA0D0A");
    m_serial_forceSensor -> write(cmd);
    m_serial_forceSensor -> flush();
}

void ForceSensor::stopForceSensor(){
    LOG(INFO)<<"Send Stop Force Sensor Command";
    running = false;
    QByteArray cmd = QByteArray::fromHex("43AA0D0A");
    m_serial_forceSensor->write(cmd);
    m_serial_forceSensor->flush();
}

void ForceSensor::handleReadyRead() {
    QByteArray newData = m_serial_forceSensor->readAll();
    m_buffer.append(newData);
    LOG(INFO) << "+++in handle ready read function";
    processBuffer();
}

void ForceSensor::processBuffer() {
    while (m_buffer.size() >= 28) {
        LOG(INFO) << "in process buffer function";
        int idx = m_buffer.indexOf(char(0xAA));
        if (idx < 1) {
            m_buffer.remove(0, 1);
            continue;
        }
        if (m_buffer.size() < idx + 27) break;

        QByteArray frame = m_buffer.mid(idx - 1, 28);

        if ((uint8_t)frame[26] == 0x0D && (uint8_t)frame[27] == 0x0A) {
            if (parseFrame(frame)) {
                m_buffer.remove(0, idx - 1 + 28);
            } else {
                m_buffer.remove(0, idx);
            }
        } else {
            m_buffer.remove(0, idx);
        }
    }
}
// void* ForceSensor::readForceSensorCyclic(void* arg){
//     ForceSensor* sensor = static_cast<ForceSensor*>(arg);

//     while (sensor->running) {
//         QByteArray newData = sensor->m_serial_forceSensor->readAll();
//         if (!newData.isEmpty()) {
//             sensor->m_buffer.append(newData);

//             // 处理缓冲区中的多帧数据
//             while (sensor->m_buffer.size() >= 28) {
//                 int idx = sensor->m_buffer.indexOf(char(0xAA));
//                 if (idx < 1) {
//                     sensor->m_buffer.remove(0, 1);
//                     continue;
//                 }
//                 if (sensor->m_buffer.size() < idx + 27) break;

//                 QByteArray frame = sensor->m_buffer.mid(idx - 1, 28);

//                 LOG(INFO) << "Raw Frame: " << frame.toHex(' ').toStdString();
//                 if ((uint8_t)frame[26] == 0x0D && (uint8_t)frame[27] == 0x0A) {
//                     if (sensor->parseFrame(frame)) {
//                         sensor->m_buffer.remove(0, idx - 1 + 28);
//                     } else {
//                         sensor->m_buffer.remove(0, idx);
//                     }
//                 } else {
//                     sensor->m_buffer.remove(0, idx);
//                 }
//             }
//         }

//         system("clear");
//         LOG(INFO) << "reading force sensor ";

//         // LOG(INFO) << std::fixed << std::setprecision(5)
//         //           << "FX: " << std::setw(10) << sensor->F_x << " "
//         //           << "FY: " << std::setw(10) << sensor->F_y << " "
//         //           << "FZ: " << std::setw(10) << sensor->F_z;
//         usleep(1000);  /* 1kHz */

//     }
//     return nullptr;
// }

bool ForceSensor::parseFrame(const QByteArray& frame) {
    /* 简单校验固定标志 */
    if ((uint8_t)frame[1] != 0xAA) return false;

    float tmpFx = readFloatLE(frame.constData() + 2);
    float tmpFy = readFloatLE(frame.constData() + 6);
    float tmpFz = readFloatLE(frame.constData() + 10);
    float tmpMx = readFloatLE(frame.constData() + 14);
    float tmpMy = readFloatLE(frame.constData() + 18);
    float tmpMz = readFloatLE(frame.constData() + 22);

    {
        std::lock_guard<std::mutex> lock(m_dataMutex);
        F_x = tmpFx;
        F_y = tmpFy;
        F_z = tmpFz;
        M_x = tmpMx;
        M_y = tmpMy;
        M_z = tmpMz;
    }

    LOG(INFO) << "ForceSensor:" << F_x << F_y << F_z << M_x << M_y << M_z;
    return true;
}

float ForceSensor::readFloatLE(const char* ptr) {
    // 直接拷贝4字节，假设系统float为IEEE754小端格式
    float value;
    std::uint8_t buf[4] = {
        static_cast<std::uint8_t>(ptr[0]),
        static_cast<std::uint8_t>(ptr[1]),
        static_cast<std::uint8_t>(ptr[2]),
        static_cast<std::uint8_t>(ptr[3])
    };
    std::memcpy(&value, buf, sizeof(float));
    return value;
}

std::array<double, 6> ForceSensor::getForceSensor() {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    return {F_x, F_y, F_z, M_x, M_y, M_z};
}

// void RobotControl::closeTorqueSensor()
// {
//     m_Torque_Sensor_Serial_422->close();
//     m_Torque_Sensor_Serial_422->deleteLater();
// }
/*
void RobotControl::onTorqueSensorDataIn()
{
    if(m_Torque_Sensor_Serial_422->canReadLine())
    {

        m_Data_Torque_Sensor_Serial_Receved += m_Torque_Sensor_Serial_422->readAll();
        int len=this->m_Data_Torque_Sensor_Serial_Receved.length();
        if(len>=2){
            uint8_t fh0=m_Data_Torque_Sensor_Serial_Receved.at(0);
            uint8_t fh1=m_Data_Torque_Sensor_Serial_Receved.at(1);
            if(fh0!=0x55||fh1!=0xaa){//is frame head  ok?
                m_Data_Torque_Sensor_Serial_Receved.clear();
                return;//如果贞头不是55,aa说明数据贞无效。
            }

            while(len>=23){
                if((this->m_Data_Torque_Sensor_Serial_Receved.at(21)==0x0D)&&((this->m_Data_Torque_Sensor_Serial_Receved.at(22)==0x0a)))//判断尾帧
                {
                    QByteArray datatemp=m_Data_Torque_Sensor_Serial_Receved.left(23);
                    uint32_t Crctemp= CalcCrc16((uint8_t *)(datatemp.data()), 19);

                    uint8_t crchigh=datatemp.at(19);
                    uint8_t crclow=datatemp.at(20);

                    if(Crctemp==(crchigh*256+crclow))//crc校验
                    {

                        //                       qDebug()<<"CRC OK";
                        uint8_t errorcode=datatemp.at(2);
                        TorqueSensorData torqueSensorData_Left_Temp;
                        torqueSensorData_Left_Temp.ErrorCode=errorcode;
                        if(errorcode==0x00)//解析数据
                        {
                            torqueSensorData_Left_Temp.TorqueValueSensor1=covertQbytearrayToFloat(m_Data_Torque_Sensor_Serial_Receved,3);
                            torqueSensorData_Left_Temp.TorqueValueSensor2=covertQbytearrayToFloat(m_Data_Torque_Sensor_Serial_Receved,7);
                            torqueSensorData_Left_Temp.TorqueValueSensor3=covertQbytearrayToFloat(m_Data_Torque_Sensor_Serial_Receved,11);
                            torqueSensorData_Left_Temp.TorqueValueSensor4=covertQbytearrayToFloat(m_Data_Torque_Sensor_Serial_Receved,15);
                        }
                        else
                        {
                            torqueSensorData_Left_Temp.TorqueValueSensor1=0;
                            torqueSensorData_Left_Temp.TorqueValueSensor2=0;
                            torqueSensorData_Left_Temp.TorqueValueSensor3=0;
                            torqueSensorData_Left_Temp.TorqueValueSensor4=0;
                        }
                        m_torqueSensorData_Right.store(torqueSensorData_Left_Temp);

                    }

                    //readHandleOtherData(datatemp);
                }
                m_Data_Torque_Sensor_Serial_Receved.remove(0,23);
                len=this->m_Data_Torque_Sensor_Serial_Receved.length();
            }
        }

    }
}
*/
