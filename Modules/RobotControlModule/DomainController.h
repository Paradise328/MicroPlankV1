#ifndef DOMAIN_CONTROLLER_H
#define DOMAIN_CONTROLLER_H

#include <atomic>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <QSerialPort>
#include <eigen3/Eigen/Dense>

#include "../MathModule/lowpass_filter.h"
#include "../PeripheralDeviceModule/peripheral_device.h"
#include "../SystemUtilsModule/SystemUtils.h"

struct ForceSensorData{
    double              ForceSensor_Fx;
    double              ForceSensor_Fy;
    double              ForceSensor_Fz;
    double              ForceSensor_Mx;
    double              ForceSensor_My;
    double              ForceSensor_Mz;
    bool                EnableButton;
    bool                Button2;
    bool                Button3;
};

struct DomainControllerData{
    ForceSensorData     ForceSensor;
    uint32_t            MagneticScale_Counter = 0;
    uint8_t             DigitalInputs;
    uint8_t             DigitalOutputs;
    bool                BtnInstrumentCalibration;
    bool                BtnInstrumentInstalled;
    uint8_t             LightStatus;
    uint8_t             TestByte=31;
};

class DomainController : public Peripheral_Device
{
public:
    enum LightModel_e
    {
        LightModel_Off=0,
        LightModel_Blink,
        LightModel_On
    };

    enum LightColor_e
    {
        LightColor_None=0,
        LightColor_Red,
        LightColor_Green,
        LightColor_Yellow
    };

    enum ForceSensor_e
    {
        Not_Calibrated=0,
        Calibrating,
        Calibrated
    };

    DomainController() = delete;
    DomainController(uint32_t ID = 0);
    DomainController(QString ip, quint16 port, uint32_t ID = 0);
    ~DomainController();

    void                                            startThread();
    void                                            VCMD(QString cmd, int arg1=-1, int arg2=-1, int arg3=-1);
    void                                            SendTestFrame(void);
    void                                            StartContinus();
    void                                            reset();
    void                                            read_Write_Data();
    float                                           Uint8ArrToFloat(uint8_t *arr, unsigned char StartIndex);
    bool                                            isSelfCheck(){ return this -> m_selfCheckOK;}
    void                                            openSerialPort(qint32 baud);
    void                                            closeSerialPort();
    int                                             findFrameHead(QByteArray &data);
    void                                            On422DataIn(void);
    eSendReturn                                     Send_Frame_By_422(COMMU_FRAME cftemp);

    void                                            readHandleOtherData(QByteArray qba);
    bool                                            isDomainControllerConnected();

    std::array<double, 3>                           getForce();
    std::array<double, 3>                           getForce(double tilt_angle);
    std::array<double, 3>                           getMomentum();
    std::array<double, 3>                           getMomentum(double tilt_angle);

    bool                                            isForceSensorZeroFound();
    bool                                            getEnableButton();
    bool                                            getResetButton();
    uint8_t                                         getDigitalInput();
    DomainControllerData                            m_domainControllerData;     /* struct for data read by domain controller */
    void                                            set_Light_Color_Model(LightColor_e color,LightModel_e model);

    int                                             getMagneticScale(){return m_endGimbalMagneticCounter.load();}
private:
    uint32_t                                        m_ID = 0;                     /* ID for the domain controller board */
    QSerialPort                                    *m_serial_422_domain_controller = nullptr;
    bool                                            m_selfCheckOK = false;
    QByteArray                                      m_422ReceiveBuffer;
    QHash<QString,eDomainController_Actions>        Qhash_Cmd_Classify;
    std::atomic<bool>                               isSystemTerminated;

    std::atomic<int>                                m_endGimbalMagneticCounter;

    std::array<double, 6>                           m_forceSensorRaw = {0} ;
    std::array<double, 6>                           m_forceSensorInit = {0};
    std::deque<std::array<double,6>>                m_forceZeroDetectBuffer;
    std::array<double, 6>                           m_forceSensorZeroCompensated = {0};

    const double                                    k_forceSensor_sample_rate = 1000.0;
    const double                                    k_forceSensor_cutoff_rate = 20.0;
    bool                                            m_forceZeroFound = false;
    const unsigned int                              m_forceZeroWindowSize = 1000;
    const double                                    m_forceZeroThreshold = 1; // 单位: N 或 Nm，可根据实际调

    std::array<double, 6>                           m_forceSensorIR = {0} ;
    LowpassFilter1stOrder<std::array<double, 6>>    m_forceSensorFilter_IR;     /* IR for force sensor data */

    std::array<double, 6>                           m_forceSensorIIR = {0} ;
    LowpassFilter2ndOrder<std::array<double, 6>>    m_forceSensorFilter_IIR;    /* IIR for force sensor data */

    std::atomic<uint8_t>                            m_LightCmd;

    bool                                            findForceSensorZero(std::array<double,6> new_sample);
};

#endif // DOMAIN_CONTROLlER_H
