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

    DomainController();
    ~DomainController();

    void                                            startThread();
    void                                            VCMD(QString cmd,uint8_t id, int arg1=-1, int arg2=-1, int arg3=-1);
    void                                            SendTestFrame(void);
    void                                            StartContinus(uint8_t id);
    void                                            reset(uint8_t id);
    void                                            read_Write_Data(uint8_t id);
    float                                           Uint8ArrToFloat(uint8_t *arr, unsigned char StartIndex);
    bool                                            isSelfCheck(){ return (this -> m_selfCheckOK_l&&this -> m_selfCheckOK_r);}
    void                                            openSerialPort(qint32 baud);
    void                                            closeSerialPort();
    int                                             findFrameHead(QByteArray &data);
    void                                            On422DataIn(void);
    eSendReturn                                     Send_Frame_By_422(COMMU_FRAME cftemp);

    void                                            readHandleOtherData(QByteArray qba);
    bool                                            isDomainControllerConnected();

    std::array<double, 3>                           getForce(uint armSide);
    std::array<double, 3>                           getForce(uint armSide, double tilt_angle);
    std::array<double, 3>                           getMomentum(uint armSide);
    std::array<double, 3>                           getMomentum(uint armSide, double tilt_angle);

    bool                                            isForceSensorZeroFound(uint armSide);
    bool                                            getEnableButton(uint armSide);
    bool                                            getResetButton(uint armSide);
    uint8_t                                         getDigitalInput(uint armSide);

    // DomainControllerData                            m_domainControllerData;     /* struct for data read by domain controller */
    std::atomic<DomainControllerData>               m_domainControllerData_l;     /* struct for data read by domain controller */
    std::atomic<DomainControllerData>               m_domainControllerData_r;     /* struct for data read by domain controller */
    void                                            set_Light_Color_Model(LightColor_e color,LightModel_e model);

    uint32_t                                             getMagneticScale(uint armSide);
private:
    uint32_t                                        m_ID = 0;                     /* ID for the domain controller board */
    QSerialPort                                    *m_serial_422_domain_controller = nullptr;
    QByteArray                                      m_422ReceiveBuffer;
    QHash<QString,eDomainController_Actions>        Qhash_Cmd_Classify;
    std::atomic<bool>                               isSystemTerminated;

    bool                                            m_selfCheckOK_l = false;
    bool                                            m_selfCheckOK_r = false;


    std::array<double, 6>                           m_forceSensorRaw_l = {0} ;
    std::array<double, 6>                           m_forceSensorInit_l = {0};
    std::deque<std::array<double,6>>                m_forceZeroDetectBuffer_l;
    std::array<double, 6>                           m_forceSensorZeroCompensated_l = {0};
    std::array<double, 6>                           m_forceSensorRaw_r = {0} ;
    std::array<double, 6>                           m_forceSensorInit_r = {0};
    std::deque<std::array<double,6>>                m_forceZeroDetectBuffer_r;
    std::array<double, 6>                           m_forceSensorZeroCompensated_r = {0};

    const double                                    k_forceSensor_sample_rate = 1000.0;
    const double                                    k_forceSensor_cutoff_rate = 20.0;
    const unsigned int                              k_forceZeroWindowSize = 1000;
    const double                                    m_forceZeroThreshold = 1.0; // 单位: N 或 Nm，可根据实际调

    bool                                            m_forceZeroFound_l = false;
    std::array<double, 6>                           m_forceSensorIR_l = {0};
    LowpassFilter1stOrder<std::array<double, 6>>    m_forceSensorFilter_IR_l;     /* IR for force sensor data */
    std::array<double, 6>                           m_forceSensorIIR_l = {0};
    LowpassFilter2ndOrder<std::array<double, 6>>    m_forceSensorFilter_IIR_l;    /* IIR for force sensor data */

    bool                                            m_forceZeroFound_r = false;
    std::array<double, 6>                           m_forceSensorIR_r = {0};
    LowpassFilter1stOrder<std::array<double, 6>>    m_forceSensorFilter_IR_r;     /* IR for force sensor data */
    std::array<double, 6>                           m_forceSensorIIR_r = {0};
    LowpassFilter2ndOrder<std::array<double, 6>>    m_forceSensorFilter_IIR_r;    /* IIR for force sensor data */

    std::atomic<uint8_t>                            m_LightCmd;

    bool                                            findForceSensorZero(std::array<double,6> new_sample,uint armSide);
};

#endif // DOMAIN_CONTROLlER_H
