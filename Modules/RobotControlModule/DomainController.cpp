#include "DomainController.h"

DomainController::DomainController(uint32_t ID):
    m_forceSensorFilter_IR(m_forceSensorIR, k_forceSensor_sample_rate, k_forceSensor_cutoff_rate),
    m_forceSensorFilter_IIR(m_forceSensorIIR, k_forceSensor_sample_rate, k_forceSensor_cutoff_rate)/* init lowpassfilter */
{
    this -> m_ID = ID;
    this -> type = DEV_DOMAINCONTROLLER;

    this -> Qhash_Cmd_Classify.insert("READWRITEDATA", DOMAINCONTROLLER_READ_WRITE_DATA);
    this -> Qhash_Cmd_Classify.insert("RESET", DOMAINCONTROLLER_RESET);

    openSerialPort(921600);
}

/* TOCHECK: what is the difference between these two constructors */
DomainController::DomainController(QString ip, quint16 port, uint32_t ID) : Peripheral_Device(ip, port),
    m_forceSensorFilter_IR(m_forceSensorIR, k_forceSensor_sample_rate, k_forceSensor_cutoff_rate),
    m_forceSensorFilter_IIR(m_forceSensorIIR, k_forceSensor_sample_rate, k_forceSensor_cutoff_rate)/* init lowpassfilter */
{
    this -> m_ID = ID;
    this -> type = DEV_DOMAINCONTROLLER;

    this -> Qhash_Cmd_Classify.insert("READWRITEDATA", DOMAINCONTROLLER_READ_WRITE_DATA);
    this -> Qhash_Cmd_Classify.insert("RESET", DOMAINCONTROLLER_RESET);

    connect(this, &DomainController::NewDataIn, this, &DomainController::readHandleOtherData);
}

/* the pointer shall be deleted; TODO: apply the destructor somewhere */
DomainController::~DomainController() {
    if (m_serial_422_domain_controller) {
        m_serial_422_domain_controller -> close();
        delete m_serial_422_domain_controller;
    }
}

void DomainController::openSerialPort(qint32 baudrate)
{
    m_serial_422_domain_controller = new QSerialPort();

    QString name = "/dev/ttyXR1";

    m_serial_422_domain_controller -> setPortName(name);
    m_serial_422_domain_controller -> setBaudRate(baudrate);
    m_serial_422_domain_controller -> setDataBits(QSerialPort::Data8);
    m_serial_422_domain_controller -> setParity(QSerialPort::NoParity);
    m_serial_422_domain_controller -> setStopBits(QSerialPort::OneStop);
    m_serial_422_domain_controller -> setFlowControl(QSerialPort::NoFlowControl);
    if (m_serial_422_domain_controller -> open(QIODevice::ReadWrite)) {
        LOG(INFO) << "RS422 port for domain-controller open successful.";
    } else {
        LOG(ERROR) << "RS422 port for domain-controller open failed.";
    }
    connect(m_serial_422_domain_controller, &QSerialPort::readyRead, this, &DomainController::On422DataIn);
}

void DomainController::closeSerialPort()
{
    LOG(INFO) << "RS422 port for domain-controller closed.";
    m_serial_422_domain_controller -> close();
}

int DomainController::findFrameHead(QByteArray &data)
{
    /* TOCHECK: check frame head */
    uint8_t fh0 = 0;
    uint8_t fh1 = 0;
    uint8_t fh2 = 0;
    uint8_t fh3 = 0;

    int len = data.length();

    if(len < 4){
        return -1;
    }

    for(int i = 0; i < len - 4; i++)
    {

        fh0 = data.at(i);
        fh1 = data.at(i + 1);
        fh2 = data.at(i + 2);
        fh3 = data.at(i + 3);
        /* TOCHECK: 0x4B,0x57,0x57,0x58?*/
        if(fh0 == 75 && fh1 == 87 && fh2 == 87 && fh3 == 88){//is frame head  ok?
            return i;
        }
    }
    return -1;
}

void DomainController::On422DataIn(void)
{
    if(m_serial_422_domain_controller -> canReadLine())
    {

        m_422ReceiveBuffer += m_serial_422_domain_controller -> readAll();
        int len = this -> m_422ReceiveBuffer.length();
        int headindex = findFrameHead(m_422ReceiveBuffer);
        if(headindex < 0)
        {
            return;
        }
        else if(headindex > 0)
        {
            m_422ReceiveBuffer.remove(0, headindex);
            return;
        }

        while(len >= 50){
            if((this -> m_422ReceiveBuffer.at(48) == 0x0D)&&((this -> m_422ReceiveBuffer.at(49) == 0x0a)))
            {
                QByteArray datatemp = m_422ReceiveBuffer.left(50);
                readHandleOtherData(datatemp);

            }
            m_422ReceiveBuffer.remove(0, 50);
            len=this -> m_422ReceiveBuffer.length();
        }
    }
}

eSendReturn DomainController::Send_Frame_By_422(COMMU_FRAME cftemp)
{
    QByteArray DataQBA;
    this -> Packet_Frame(DataQBA, cftemp);

    m_serial_422_domain_controller -> write(DataQBA.data(), DataQBA.length());

    return SEND_SUCCESS;
}



float DomainController::Uint8ArrToFloat(uint8_t *arr, unsigned char StartIndex)
{
    unsigned char floatData[4] = {0};
    float f = 0;
    floatData[0] = arr[StartIndex];
    floatData[1] = arr[StartIndex+1];
    floatData[2] = arr[StartIndex+2];
    floatData[3] = arr[StartIndex+3];
    memcpy(&f, floatData, 4);
    return f;
}

/* TOCHECK: what is VCMD */
void DomainController::VCMD(QString cmd, int arg1, int arg2, int arg3)
{

    cmd=cmd.toUpper();
    QStringList slist = cmd.split("=");

    COMMU_FRAME cf;
    memset(&cf, 0, sizeof (COMMU_FRAME));
    uint8_t action1[2] = {0};
    int actionLen = 2;
    action1[0] = this -> Qhash_Cmd_Classify[slist[0]];

    switch (this -> Qhash_Cmd_Classify[slist[0]])
    {
        case DOMAINCONTROLLER_READ_WRITE_DATA:
        {
            action1[1] = 0;
            actionLen = 1;
            cf.payload.type = CMD_READ_WRITE;
            break;
        }
        case DOMAINCONTROLLER_RESET:
        {
            action1[1] = 0;
            actionLen = 1;
            cf.payload.type = CMD_CTRL_WRITE;
            break;
        }
        default: break;
    }

    if(arg1 != -1)    actionLen++;
    if(arg2 != -1)    actionLen++;
    if(arg3 != -1)    actionLen++;

    uint8_t payloadsize = 3 + actionLen;
    uint8_t framelength = 17 + payloadsize;

    cf.preamble = PREAMBLE;
    cf.ID = this -> GetRandNum();
    cf.size = framelength;
    cf.sender = DEV_HOST;

    cf.recever = this -> type;

    cf.payload.size = payloadsize;
    cf.payload.argscounter = actionLen;

    unsigned int j = 0;
    for( j = 0; j < sizeof(action1); j++){
        cf.payload.args[j] = action1[j];
    }

    if(arg1 != -1){ cf.payload.args[j] = (uint8_t)arg1; j++; }
    if(arg2 != -1){ cf.payload.args[j] = (uint8_t)arg2; j++; }
    if(arg3 != -1){ cf.payload.args[j] = (uint8_t)arg3; j++; }

    Send_Frame_By_422(cf);
}

void DomainController::reset()
{
    this -> VCMD("RESET");
}

void DomainController::read_Write_Data()
{
    this -> VCMD("READWRITEDATA");
}
bool DomainController::findForceSensorZero(std::array<double,6> new_sample){
    /* push new value to zero-point dectect buffer */
    m_forceZeroDetectBuffer.push_back(new_sample);

    if (m_forceZeroDetectBuffer.size() > m_forceZeroWindowSize){
        m_forceZeroDetectBuffer.pop_front();
    }

    /* judge when window size is enough */
    if (m_forceZeroDetectBuffer.size() == m_forceZeroWindowSize) {
        std::array<double,6> minVal = m_forceZeroDetectBuffer[0];
        std::array<double,6> maxVal = m_forceZeroDetectBuffer[0];
        std::array<double,6> sumVal = {0};

        for (const auto& sample : m_forceZeroDetectBuffer) {
            for (int i = 0; i < 6; ++i) {
                if (sample[i] < minVal[i]) minVal[i] = sample[i];
                if (sample[i] > maxVal[i]) maxVal[i] = sample[i];
                sumVal[i] += sample[i];
            }
        }

        bool foundStableZero = true;
        for (int i = 0; i < 6; ++i) {
            if ((maxVal[i] - minVal[i]) > m_forceZeroThreshold) {
                foundStableZero = false;
                break;
            }
        }

        if (foundStableZero) {
            for (int i = 0; i < 6; ++i)
                m_forceSensorInit[i] = sumVal[i] / m_forceZeroWindowSize;

            m_forceZeroFound = true;
        }
    }

    return m_forceZeroFound;
}

void DomainController::readHandleOtherData(QByteArray qba)
{
    COMMU_FRAME cftemp;

    eDepackReturn edr = this -> Depack_Frame(qba, cftemp);
    if(edr == Depack_SUCCESS)
    {
        this -> m_selfCheckOK = true;

        if(cftemp.payload.args[0] == Dev_Sta_OK)
        {
            /* parse data for magnetic-scale */
            m_domainControllerData.MagneticScale_Counter=0;
            m_domainControllerData.MagneticScale_Counter|=cftemp.payload.args[4];
            m_domainControllerData.MagneticScale_Counter<<=8;
            m_domainControllerData.MagneticScale_Counter|=cftemp.payload.args[3];
            m_domainControllerData.MagneticScale_Counter<<=8;
            m_domainControllerData.MagneticScale_Counter|=cftemp.payload.args[2];
            m_domainControllerData.MagneticScale_Counter<<=8;
            m_domainControllerData.MagneticScale_Counter|=cftemp.payload.args[1];

            /* parse data for forceSensor */
            m_domainControllerData.ForceSensor.ForceSensor_Fx = Uint8ArrToFloat(cftemp.payload.args,5);
            m_domainControllerData.ForceSensor.ForceSensor_Fy = Uint8ArrToFloat(cftemp.payload.args,9);
            m_domainControllerData.ForceSensor.ForceSensor_Fz = Uint8ArrToFloat(cftemp.payload.args,13);
            m_domainControllerData.ForceSensor.ForceSensor_Mx = Uint8ArrToFloat(cftemp.payload.args,17);
            m_domainControllerData.ForceSensor.ForceSensor_My = Uint8ArrToFloat(cftemp.payload.args,21);
            m_domainControllerData.ForceSensor.ForceSensor_Mz = Uint8ArrToFloat(cftemp.payload.args,25);

            m_forceSensorRaw = {m_domainControllerData.ForceSensor.ForceSensor_Fx / 10000.0,
                                m_domainControllerData.ForceSensor.ForceSensor_Fy / 10000.0,
                                m_domainControllerData.ForceSensor.ForceSensor_Fz / 10000.0,
                                m_domainControllerData.ForceSensor.ForceSensor_Mx / 10000.0,
                                m_domainControllerData.ForceSensor.ForceSensor_My / 10000.0,
                                m_domainControllerData.ForceSensor.ForceSensor_Mz / 10000.0 };

            if (!m_forceZeroFound) {
                findForceSensorZero(m_forceSensorRaw);
            }

            m_forceSensorZeroCompensated = m_forceSensorRaw;

            if (m_forceZeroFound) {
                for (int i = 0; i < 6; ++i)
                    m_forceSensorZeroCompensated[i] -= m_forceSensorInit[i];
            }

            /* feed to filter */
            m_forceSensorIR = m_forceSensorFilter_IR.update(m_forceSensorZeroCompensated);
            m_forceSensorIIR = m_forceSensorFilter_IIR.update(m_forceSensorZeroCompensated);

            /* depackage data for digital inputs */
            m_domainControllerData.DigitalInputs = cftemp.payload.args[29];
        }
        else if(cftemp.payload.args[0] == Dev_Sta_LEFTHANDLE_ERROR)
        {
            //qDebug()<<"LEFTHANDLE_ERROR!";
        }
        else if(cftemp.payload.args[0] == Dev_Sta_RIGHTHANDLE_ERROR)
        {
            //qDebug()<<"RIGHTHANDLE_ERROR!";
        }
        else if(cftemp.payload.args[0] == (Dev_Sta_LEFTHANDLE_ERROR+Dev_Sta_RIGHTHANDLE_ERROR))
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
        // uint8_t byteemp=0;
            //LOG(INFO)<<"qba.length()="<<qba.length();
        //        for(int i=0;i<qba.length();i++){
        //            byteemp=qba.at(i);
        //            LOG(INFO)<<"qba["<<i<<"]="<<QString::number(byteemp,16).toStdString();
        //        }
    }
}

/* if degree input is void, getForce() returns Force value under self coordinate */
std::array<double,3> DomainController::getForce(){
    // return {m_forceSensorZeroCompensated[0], m_forceSensorZeroCompensated[1], m_forceSensorZeroCompensated[2]};
    return {m_forceSensorIIR[0], m_forceSensorIIR[1], m_forceSensorIIR[2]};
}

/* if degree input is void, getMomentum() returns Momentum value under self coordinate */
std::array<double,3> DomainController::getMomentum(){
    return {m_forceSensorIIR[3], m_forceSensorIIR[4], m_forceSensorIIR[5]};
}

std::array<double,3> DomainController::getForce(double tilt_angle_deg) {

    double install_angle_deg = 90.0; /* around Z axis */
    double total_z_deg = install_angle_deg - tilt_angle_deg; /* around Z axis */

    /* to arc */
    double theta_z = total_z_deg * M_PI / 180.0; /* around Z axis */
    double theta_y = 90.0 * M_PI / 180.0;  /* around Y axis */

    /* rotation matrix */
    Eigen::Matrix3d Rz = Eigen::AngleAxisd(theta_z, Eigen::Vector3d::UnitZ()).toRotationMatrix();
    Eigen::Matrix3d Ry = Eigen::AngleAxisd(theta_y, Eigen::Vector3d::UnitY()).toRotationMatrix();
    Eigen::Matrix3d R = Rz * Ry;

    std::array<double, 3> force = getForce();
    Eigen::Vector3d Force_sensor(force[0], force[1], force[2]);

    Eigen::Vector3d Force_world = R.transpose() * Force_sensor;

    return {static_cast<double>(Force_world.x()), static_cast<double>(Force_world.y()), static_cast<double>(Force_world.z())};
}

std::array<double,3> DomainController::getMomentum(double tilt_angle_deg){

    double install_angle_deg = 90.0 ; /* around Z axis */
    double total_z_deg = install_angle_deg - tilt_angle_deg; /* around Z axis */

    /* to arc */
    double theta_z = tilt_angle_deg * M_PI / 180.0; /* around Z axis */
    double theta_y = 90.0 * M_PI / 180.0;  /* around Y axis */

    /* rotation matrix */
    Eigen::Matrix3d Rz = Eigen::AngleAxisd(theta_z, Eigen::Vector3d::UnitZ()).toRotationMatrix();
    Eigen::Matrix3d Ry = Eigen::AngleAxisd(theta_y, Eigen::Vector3d::UnitY()).toRotationMatrix();
    Eigen::Matrix3d R = Rz * Ry;

    std::array<double, 3> momentum = getMomentum();
    Eigen::Vector3d Momentum_sensor(momentum[0], momentum[1], momentum[2]);

    Eigen::Vector3d Momentum_world = R * Momentum_sensor;

    return {static_cast<double>(Momentum_world.x()), static_cast<double>(Momentum_world.y()), static_cast<double>(Momentum_world.z())};
}

bool DomainController::isDomainControllerConnected(){
    return true;
}
bool DomainController::isForceSensorZeroFound(){
    return m_forceZeroFound;
}

uint8_t DomainController::getDigitalInput(){
    return m_domainControllerData.DigitalInputs;
}

/* TODO: test */
bool DomainController::getEnableButton(){
    /* the 0th bit of uint_8 */
    return (m_domainControllerData.DigitalInputs & 0x01) != 0;
}

/* TODO: test */
bool DomainController::getResetButton(){
    /* the 1st bit of uint_8 */
    return ((m_domainControllerData.DigitalInputs >> 1) & 0x01) != 0;
}
