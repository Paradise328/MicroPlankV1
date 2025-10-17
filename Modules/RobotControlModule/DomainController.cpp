#include "DomainController.h"

DomainController::DomainController():
    m_forceSensorFilter_IR_l(m_forceSensorIR_l, k_forceSensor_sample_rate, k_forceSensor_cutoff_rate),
    m_forceSensorFilter_IIR_l(m_forceSensorIIR_l, k_forceSensor_sample_rate, k_forceSensor_cutoff_rate),
    m_forceSensorFilter_IR_r(m_forceSensorIR_r, k_forceSensor_sample_rate, k_forceSensor_cutoff_rate),
    m_forceSensorFilter_IIR_r(m_forceSensorIIR_r, k_forceSensor_sample_rate, k_forceSensor_cutoff_rate),
    isSystemTerminated(false)/* init lowpassfilter */
{
    LOG(INFO) << " in default contructor ";
    this -> type = DEV_DOMAINCONTROLLER;

    this -> Qhash_Cmd_Classify.insert("READWRITEDATA", DOMAINCONTROLLER_READ_WRITE_DATA);
    this -> Qhash_Cmd_Classify.insert("RESET", DOMAINCONTROLLER_RESET);

    m_LightCmd.store(0);

    set_Light_Color_Model(LightColor_None, LightModel_Off);
    openSerialPort(921600);
}

void DomainController::startThread()
{
    QThread *thread = QThread::create([this](){
        while(!isSystemTerminated){
            read_Write_Data(1);
            SteadyDelay(5);
            read_Write_Data(2);
            SteadyDelay(5);
        }
    });
    thread->start();
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
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
        LOG(INFO) << "---RS422 port for domain-controller open successful.";
    } else {
        LOG(ERROR) << "---RS422 port for domain-controller open failed.";
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
            len = this -> m_422ReceiveBuffer.length();
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
void DomainController::VCMD(QString cmd, uint8_t id,int arg1, int arg2, int arg3)
{
    cmd=cmd.toUpper();
    QStringList slist=cmd.split("=");

    COMMU_FRAME cf;
    memset(&cf,0,sizeof (COMMU_FRAME));
    uint8_t action1[10]={0};

    int actionIndex=0;
    action1[actionIndex]=this->Qhash_Cmd_Classify[slist[0]];
    actionIndex++;

    switch (this->Qhash_Cmd_Classify[slist[0]])
    {
    case DOMAINCONTROLLER_READ_WRITE_DATA:
    {
        cf.payload.type=CMD_READ_WRITE;
        break;
    }
    case DOMAINCONTROLLER_RESET:
    {
        cf.payload.type=CMD_CTRL_WRITE;
        break;
    }
    default:break;
    }

    if(arg1!=-1){action1[actionIndex]=(uint8_t)arg1;actionIndex++;}
    if(arg2!=-1){action1[actionIndex]=(uint8_t)arg2;actionIndex++;}
    if(arg3!=-1){action1[actionIndex]=(uint8_t)arg3;actionIndex++;}

    for(int j=0;j<actionIndex;j++)  //action length = actionindex here
    {cf.payload.args[j]=action1[j];}

    uint8_t payloadsize=3+actionIndex;//action length = actionindex here
    uint8_t framelen=17+payloadsize;

    cf.preamble=PREAMBLE;
    cf.ID= id;//this->GetRandNum();
   // LOG(INFO) <<  "cf.ID= m_ID: " << cf.ID;
    cf.size=framelen;
    cf.sender=DEV_HOST;

    cf.recever=this->type;

    cf.payload.size=payloadsize;
    cf.payload.argscounter=actionIndex;//action length = actionindex here

    // Send_Frame(cf);
    Send_Frame_By_422(cf);
}

void DomainController::reset(uint8_t id)
{
    this -> VCMD("RESET",id);
}

void DomainController::set_Light_Color_Model(LightColor_e color, LightModel_e model)
{
    uint8_t LightCmdTemp=0;
    LightCmdTemp=(color<<4);
    LightCmdTemp|=(model&0X0F);
    m_LightCmd.store(LightCmdTemp);
}

void DomainController::read_Write_Data(uint8_t id)
{
    // set_Light_Color_Model(LightColor_Green,LightModel_On);
    this->VCMD("READWRITEDATA",id,m_LightCmd.load());
}

bool DomainController::findForceSensorZero(std::array<double,6> new_sample,uint armSide){
    /* push new value to zero-point dectect buffer */
    if(armSide == 0)
    {
        m_forceZeroDetectBuffer_r.push_back(new_sample);

        if (m_forceZeroDetectBuffer_r.size() > k_forceZeroWindowSize){
            m_forceZeroDetectBuffer_r.pop_front();
        }

        /* judge when window size is enough */
        if (m_forceZeroDetectBuffer_r.size() == k_forceZeroWindowSize) {
            std::array<double,6> minVal = m_forceZeroDetectBuffer_r[0];
            std::array<double,6> maxVal = m_forceZeroDetectBuffer_r[0];
            std::array<double,6> sumVal = {0};

            for (const auto& sample : m_forceZeroDetectBuffer_r) {
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
                    m_forceSensorInit_r[i] = sumVal[i] / k_forceZeroWindowSize;

                m_forceZeroFound_r = true;
            }
        }
        return m_forceZeroFound_r;
    }

    if(armSide == 1)
    {
        m_forceZeroDetectBuffer_l.push_back(new_sample);

        if (m_forceZeroDetectBuffer_l.size() > k_forceZeroWindowSize){
            m_forceZeroDetectBuffer_l.pop_front();
        }


        /* judge when window size is enough */
        if (m_forceZeroDetectBuffer_l.size() == k_forceZeroWindowSize) {
            std::array<double,6> minVal = m_forceZeroDetectBuffer_l[0];
            std::array<double,6> maxVal = m_forceZeroDetectBuffer_l[0];
            std::array<double,6> sumVal = {0};

            for (const auto& sample : m_forceZeroDetectBuffer_l) {
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
                    m_forceSensorInit_l[i] = sumVal[i] / k_forceZeroWindowSize;
                m_forceZeroFound_l = true;
            }
        }

        return m_forceZeroFound_l;
    }
}

void DomainController::readHandleOtherData(QByteArray qba)
{
    COMMU_FRAME cftemp;

    eDepackReturn edr = this -> Depack_Frame(qba, cftemp);
    if(edr == Depack_SUCCESS)
    {


        if(cftemp.payload.args[0] == Dev_Sta_OK)
        {
            if(cftemp.ID==1)
            {
                this -> m_selfCheckOK_l = true;
                /* parse data for magnetic-scale */
                DomainControllerData m_domainControllerData_l_tmp;
                m_domainControllerData_l_tmp.MagneticScale_Counter=0;
                m_domainControllerData_l_tmp.MagneticScale_Counter|=cftemp.payload.args[4];
                m_domainControllerData_l_tmp.MagneticScale_Counter<<=8;
                m_domainControllerData_l_tmp.MagneticScale_Counter|=cftemp.payload.args[3];
                m_domainControllerData_l_tmp.MagneticScale_Counter<<=8;
                m_domainControllerData_l_tmp.MagneticScale_Counter|=cftemp.payload.args[2];
                m_domainControllerData_l_tmp.MagneticScale_Counter<<=8;
                m_domainControllerData_l_tmp.MagneticScale_Counter|=cftemp.payload.args[1];
                // std::cout << "magneticEncoder_L: " << m_domainControllerData_l_tmp.MagneticScale_Counter << std::endl;

                /* parse data for forceSensor */
                m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Fx = Uint8ArrToFloat(cftemp.payload.args, 5);
                m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Fy = Uint8ArrToFloat(cftemp.payload.args, 9);
                m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Fz = Uint8ArrToFloat(cftemp.payload.args, 13);
                m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Mx = Uint8ArrToFloat(cftemp.payload.args, 17);
                m_domainControllerData_l_tmp.ForceSensor.ForceSensor_My = Uint8ArrToFloat(cftemp.payload.args, 21);
                m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Mz = Uint8ArrToFloat(cftemp.payload.args, 25);

                m_forceSensorRaw_l = {m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Fx * 9.8,
                                    m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Fy * 9.8,
                                    m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Fz * 9.8,
                                    m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Mx * 9.8,
                                    m_domainControllerData_l_tmp.ForceSensor.ForceSensor_My * 9.8,
                                    m_domainControllerData_l_tmp.ForceSensor.ForceSensor_Mz * 9.8 };

                if (!m_forceZeroFound_l) {
                    findForceSensorZero(m_forceSensorRaw_l, 1);
                }

                m_forceSensorZeroCompensated_l = m_forceSensorRaw_l;

                if (m_forceZeroFound_l) {
                    for (int i = 0; i < 6; ++i)
                        m_forceSensorZeroCompensated_l[i] -= m_forceSensorInit_l[i];
                }

                /* feed to filter */
                m_forceSensorIR_l = m_forceSensorFilter_IR_l.update(m_forceSensorZeroCompensated_l);
                m_forceSensorIIR_l = m_forceSensorFilter_IIR_l.update(m_forceSensorZeroCompensated_l);

                /* depackage data for digital inputs */
                m_domainControllerData_l_tmp.DigitalInputs = cftemp.payload.args[29];
                m_domainControllerData_l.store(m_domainControllerData_l_tmp);

            }

            if(cftemp.ID==2)
            {

                this -> m_selfCheckOK_r = true;
                /* parse data for magnetic-scale */
                DomainControllerData m_domainControllerData_r_tmp;
                m_domainControllerData_r_tmp.MagneticScale_Counter=0;
                m_domainControllerData_r_tmp.MagneticScale_Counter|=cftemp.payload.args[4];
                m_domainControllerData_r_tmp.MagneticScale_Counter<<=8;
                m_domainControllerData_r_tmp.MagneticScale_Counter|=cftemp.payload.args[3];
                m_domainControllerData_r_tmp.MagneticScale_Counter<<=8;
                m_domainControllerData_r_tmp.MagneticScale_Counter|=cftemp.payload.args[2];
                m_domainControllerData_r_tmp.MagneticScale_Counter<<=8;
                m_domainControllerData_r_tmp.MagneticScale_Counter|=cftemp.payload.args[1];
                // std::cout << "magneticEncoder_r: " << m_domainControllerData_r_tmp.MagneticScale_Counter << std::endl;

                /* parse data for forceSensor */
                m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Fx = Uint8ArrToFloat(cftemp.payload.args, 5);
                m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Fy = Uint8ArrToFloat(cftemp.payload.args, 9);
                m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Fz = Uint8ArrToFloat(cftemp.payload.args, 13);
                m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Mx = Uint8ArrToFloat(cftemp.payload.args, 17);
                m_domainControllerData_r_tmp.ForceSensor.ForceSensor_My = Uint8ArrToFloat(cftemp.payload.args, 21);
                m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Mz = Uint8ArrToFloat(cftemp.payload.args, 25);

                m_forceSensorRaw_r = {m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Fx * 9.8,
                                      m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Fy * 9.8,
                                      m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Fz * 9.8,
                                      m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Mx * 9.8,
                                      m_domainControllerData_r_tmp.ForceSensor.ForceSensor_My * 9.8,
                                      m_domainControllerData_r_tmp.ForceSensor.ForceSensor_Mz * 9.8 };

                if (!m_forceZeroFound_r) {
                    findForceSensorZero(m_forceSensorRaw_r, 0);
                }

                m_forceSensorZeroCompensated_r = m_forceSensorRaw_r;

                if (m_forceZeroFound_r) {
                    for (int i = 0; i < 6; ++i)
                        m_forceSensorZeroCompensated_r[i] -= m_forceSensorInit_r[i];
                }

                /* feed to filter */
                m_forceSensorIR_r = m_forceSensorFilter_IR_r.update(m_forceSensorZeroCompensated_r);
                m_forceSensorIIR_r = m_forceSensorFilter_IIR_r.update(m_forceSensorZeroCompensated_r);

                /* depackage data for digital inputs */
                m_domainControllerData_r_tmp.DigitalInputs = cftemp.payload.args[29];
                m_domainControllerData_r.store(m_domainControllerData_r_tmp);



            }

            // std::cout << static_cast<int>(m_domainControllerData.DigitalInputs)<< std::endl;
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
std::array<double,3> DomainController::getForce(uint armSide){
    if(armSide == 1)
    {
        return {m_forceSensorIIR_l[0], m_forceSensorIIR_l[1], m_forceSensorIIR_l[2]};
    }
    if(armSide == 0)
    {
        return {m_forceSensorIIR_r[0], m_forceSensorIIR_r[1], m_forceSensorIIR_r[2]};
    }
}

/* if degree input is void, getMomentum() returns Momentum value under self coordinate */
std::array<double,3> DomainController::getMomentum(uint armSide){
    if(armSide == 1)
    {
        return {m_forceSensorIIR_l[3], m_forceSensorIIR_l[4], m_forceSensorIIR_l[5]};
    }
    if(armSide == 0)
    {
        return {m_forceSensorIIR_r[3], m_forceSensorIIR_r[4], m_forceSensorIIR_r[5]};
    }
}

std::array<double,3> DomainController::getForce(uint armSide, double tilt_angle_deg) {
    if(armSide == 0){

        double install_angle_deg = -90.0; /* around Z axis */
        // double total_z_deg = install_angle_deg - tilt_angle_deg; /* around Z axis */
        double total_z_deg = -tilt_angle_deg; // -30.0; /* around Z axis */

        /* to arc */
        double theta_z = total_z_deg * M_PI / 180.0; /* around Z axis */
        double theta_y = install_angle_deg * M_PI / 180.0;  /* around Y axis */

        /* rotation matrix */
        Eigen::Matrix3d Rz = Eigen::AngleAxisd(theta_z, Eigen::Vector3d::UnitZ()).toRotationMatrix();
        Eigen::Matrix3d Ry = Eigen::AngleAxisd(theta_y, Eigen::Vector3d::UnitY()).toRotationMatrix();
        Eigen::Matrix3d R =  Ry * Rz;

        std::array<double, 3> force = getForce(0);
        Eigen::Vector3d Force_sensor(force[0], force[1], force[2]);

        Eigen::Vector3d Force_world = R/*.transpose()*/ * Force_sensor;
        return {static_cast<double>(Force_world.x()), static_cast<double>(Force_world.y()), static_cast<double>(Force_world.z())};

    }


    if(armSide == 1){

        double install_angle_deg = -90.0; /* around Z axis */
        // double total_z_deg = install_angle_deg - tilt_angle_deg; /* around Z axis */
        double total_z_deg = tilt_angle_deg; // 30.0; /* around Z axis */

        /* to arc */
        double theta_z = total_z_deg * M_PI / 180.0; /* around Z axis */
        double theta_y = install_angle_deg * M_PI / 180.0;  /* around Y axis */

        /* rotation matrix */
        Eigen::Matrix3d Rz = Eigen::AngleAxisd(theta_z, Eigen::Vector3d::UnitZ()).toRotationMatrix();
        Eigen::Matrix3d Ry = Eigen::AngleAxisd(theta_y, Eigen::Vector3d::UnitY()).toRotationMatrix();
        Eigen::Matrix3d R =  Ry * Rz;

        std::array<double, 3> force = getForce(1);
        Eigen::Vector3d Force_sensor(force[0], force[1], force[2]);

        Eigen::Vector3d Force_world = R/*.transpose()*/ * Force_sensor;
        return {static_cast<double>(Force_world.x()), static_cast<double>(Force_world.y()), static_cast<double>(Force_world.z())};

    }

}

std::array<double,3> DomainController::getMomentum(uint armSide, double tilt_angle_deg){

    double install_angle_deg = 90.0 ; /* around Z axis */
    double total_z_deg = install_angle_deg - tilt_angle_deg; /* around Z axis */

    /* to arc */
    double theta_z = tilt_angle_deg * M_PI / 180.0; /* around Z axis */
    double theta_y = 90.0 * M_PI / 180.0;  /* around Y axis */

    /* rotation matrix */
    Eigen::Matrix3d Rz = Eigen::AngleAxisd(theta_z, Eigen::Vector3d::UnitZ()).toRotationMatrix();
    Eigen::Matrix3d Ry = Eigen::AngleAxisd(theta_y, Eigen::Vector3d::UnitY()).toRotationMatrix();
    Eigen::Matrix3d R = Rz * Ry;

    std::array<double, 3> momentum = getMomentum(1);
    Eigen::Vector3d Momentum_sensor(momentum[0], momentum[1], momentum[2]);

    Eigen::Vector3d Momentum_world = R * Momentum_sensor;

    return {static_cast<double>(Momentum_world.x()), static_cast<double>(Momentum_world.y()), static_cast<double>(Momentum_world.z())};
}

bool DomainController::isDomainControllerConnected(){
    return true;
}
bool DomainController::isForceSensorZeroFound(uint armSide){
    if(armSide == 1)
    {
        return m_forceZeroFound_l;
    }
    if(armSide == 2)
    {
        return m_forceZeroFound_r;
    }
}

uint32_t DomainController::getMagneticScale(uint armSide){
    if(armSide == 1)
    {
        return m_domainControllerData_l.load().MagneticScale_Counter;
    }
    if(armSide == 2)
    {
        return m_domainControllerData_r.load().MagneticScale_Counter;
    }
}

uint8_t DomainController::getDigitalInput(uint armside){
    if(armside == 1)
        return m_domainControllerData_l.load().DigitalInputs;
    else if(armside == 2)
        return m_domainControllerData_r.load().DigitalInputs;
}

/* TODO: test */
bool DomainController::getEnableButton(uint armside){
    if(armside == 1)
        return (m_domainControllerData_l.load().DigitalInputs & 0x01) != 0;
    else if(armside == 2)
        /* the 0th bit of uint_8 */
        return (m_domainControllerData_r.load().DigitalInputs & 0x01) != 0;
}

/* TODO: test */
bool DomainController::getResetButton(uint armside){
    if(armside == 1)
        return ((m_domainControllerData_l.load().DigitalInputs >> 1) & 0x01) != 0;
    else if(armside == 2)
        /* the 0th bit of uint_8 */
        return ((m_domainControllerData_r.load().DigitalInputs >> 1) & 0x01) != 0;

}
