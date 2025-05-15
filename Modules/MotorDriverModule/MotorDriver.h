#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <utility>
#include <thread>
#include <future>
#include <toml++/toml.h>
#include "cifx/cifxlinux.h"
#include "cifx/cifXEndianess.h"
#include "cifx/Hil_Packet.h"
#include "cifx/Hil_SystemCmd.h"
#include "Modules/MathModule/MathUtils.h"
#include "Modules/LoggerModule/easylogging++.h"
#include "Modules/SystemUtilsModule/SystemUtils.h"
#include "Modules/MsgModule/messagequeue.h"


#define CIFX_DEV "PROFIBUS"
#define JOINT_NUM 1
#define MAX_PROFILE_VEL ((uint32_t) 514572)   // 314572 counts/s zeroerr
#define PROFILE_ACC ((uint32_t) 425125)  // 325125 counts/s^2
#define PROFILE_DEC ((uint32_t) 425125)  // 325125 counts/s^2
#define PROFILE_VEL_MOONS ((uint32_t) 50000) // 2000000 counts/s
#define PROFILE_ACC_MOONS ((uint32_t) 50000) // 2000000 counts/s
#define PROFILE_DEC_MOONS ((uint32_t) 50000) // 2000000 counts/s
#define PROFILE_VEL_MOONS_CSP ((uint32_t) 1000000)
#define PROFILE_ACC_MOONS_CSP ((uint32_t) 1000000)
#define PROFILE_DEC_MOONS_CSP ((uint32_t) 1000000)
//((int32_t)0x00000000L)

/*Define the name of each keyword*/
#define CONTROLWORD        "ControlWord"
#define OPMODE             "OperationMode"
#define TARGETPOS          "TargetPos"
#define TARGETVEL          "TargetVel"
#define TARGETTRQ          "TargetTrq"
#define VELOFFSET          "VelOffset"
#define TRQOFFSET          "TrqOffset"
#define DIGITALOUT         "DigitalOutputs"
#define PROFILEVEL         "ProfileVel"
#define PROFILEACC         "ProfileAcc"
#define PROFILEDEC         "ProfileDec"
#define POSTRQLIMIT        "PosTrqLimit"
#define NEGTRQLIMIT        "NegTrqLimit"
#define FOLLOWERRWINDOW    "FollowErrWindow"
#define DUMMYBYTE          "DummyByte"
#define STATUSWORD         "StatusWord"
#define SEARCHFORZEROVEL   "SearchForZeroVel"
#define MODEDISPLAY        "ModeDisplay"
#define MAXPROFILEVEL      "MaxProfileVel"
#define ERRCODE            "ErrorCode"
#define ACTPOS             "ActPos"
#define ACTVEL             "ActVel"
#define ACTTRQ             "ActTrq"
#define ACTCUR             "ActCur"
#define DIGITALINPUT       "DigitalInputs"
#define FOLLOWINGPOSERR    "FollowingPosErr"
#define INTERPOLATIONTIME  "InterpolationTime"
#define HOMEMETHOD         "HomeMethod"
#define HOMEOFFSET         "HomeOffset"
#define HOMEVEL            "HomeVel"
#define HOMEACC            "HomeAcc"

#define CONTROLWORD_1      "Control_1"
#define CONTROLWORD_2      "Control_2"
#define PROCESSID           1;

constexpr int interpolationTime_maxonMotor = 4;
constexpr int zeroErr_sizeRecvData = 26;
constexpr int zeroErr_sizeSendData = 44;
constexpr int moons_sizeRecvData   = 23;
constexpr int moons_sizeSendData   = 27;
constexpr int maxon_sizeRecvData   = 29;
constexpr int maxon_sizeSendData   = 35;

constexpr int endGimbalMotor_sizeRecvData = 23;
constexpr int endGimbalMotor_sizeSendData = 27;
constexpr int endJointMotor_sizeRecvData = 26;
constexpr int endJointMotor_sizeSendData = 44;
constexpr int guidingJointMotor_sizeRecvData = 26;          /* receive from motor */
constexpr int guidingJointMotor_sizeSendData = 44;          /* send to motor */
constexpr int endInstrumentMotor_sizeRecvData = 29;
constexpr int endInstrumentMotor_sizeSendData = 35;

constexpr int lengthSDODataHeader  = 18;
constexpr int timeOutOpenBus       = 10000;               // timeout value for open bus
constexpr int timeOutCyclicIO      = 40;                  // timeout value for cyclic data transfer
constexpr uint ulTimeout           = 1000;                // timeout value for SDO, ms
constexpr uint timeWaitMailBoxFree = 40;                  // time in ms to wait for the mailbox to get free
constexpr uint timeWaitMessage     = 40;   //0               // time in ms to wait for the message
constexpr uint countsPerDeg        = 1456;                // ZeroDriver 524288/round

constexpr int zeroVel = 0;

/*Define commands for EtherCAT mailbox handling*/
enum class UlCmd{
    ECM_IF_CMD_COE_SDO_DOWNLOAD_REQ         = 0x9A00,
    ECM_IF_CMD_COE_SDO_DOWNLOAD_CNF         = 0x9A01,
    ECM_IF_CMD_COE_SDO_UPLOAD_REQ           = 0x9A02,
    ECM_IF_CMD_COE_SDO_UPLOAD_CNF           = 0x9A03,
    ECM_IF_CMD_SET_MASTER_TARGET_STATE_REQ  = 0x9E00,
    ECM_IF_CMD_SET_MASTER_TARGET_STATE_CNF  = 0x9E01,
    ECM_IF_CMD_GET_MASTER_CURRENT_STATE_REQ = 0x9E02,
    ECM_IF_CMD_GET_MASTER_CURRENT_STATE_CNF = 0x9E03,
    ECM_IF_CMD_SET_SLAVE_TARGET_STATE_REQ   = 0x9E04,
    ECM_IF_CMD_SET_SLAVE_TARGET_STATE_CNF   = 0x9E05,
    ECM_IF_CMD_GET_SLAVE_CURRENT_STATE_REQ  = 0x9E06,
    ECM_IF_CMD_GET_SLAVE_CURRENT_STATE_CNF  = 0x9E07
};

enum class MasterState{
    ECM_IF_STATE_INIT     = 0x01,
    ECM_IF_STATE_PREOP    = 0x02,
    ECM_IF_STATE_SAFEOP   = 0x04,
    ECM_IF_STATE_OP       = 0x08,
    ECM_IF_STATE_BUSOFF   = 0x00,
    ECM_IF_STATE_LEAVE_OP = 0x18,
    ECM_IF_STATE_BUSSCAN_COMPLETE_NO_PREOP = 0x1D,
    ECM_IF_STATE_BUSSCAN  = 0x1E,
    ECM_IF_STATE_BUSSCAN_COMPLETE =0x1F
};

enum class SlaveState{
    ECM_IF_STATE_INIT       = 0x01,
    ECM_IF_STATE_PREOP      = 0x02,
    ECM_IF_STATE_BOOT       = 0x03,
    ECM_IF_STATE_SAFEOP     = 0x04,
    ECM_IF_STATE_OP         = 0x08,
    ECM_IF_STATE_BUSOFF     = 0x00,
    ECM_IF_STATE_INIT_ERR   = 0x11,
    ECM_IF_STATE_PREOP_ERR  = 0x12,
    ECM_IF_STATE_BOOT_ERR   = 0x13,
    ECM_IF_STATE_SAFEOP_ERR = 0x14
};

/*Define the object index of motor servo*/
enum class MotorServoObjectIndex{
    STO                      = 0x4602,   // RW  4 Bytes  UINT32  (safety torque off, Brake)
    POS_DEMAND_VAL           = 0x6062,   // R   4 Bytes  INT32
    POS_FOLLOWING_ERR_WINDOW = 0x6065,   // RW  4 Bytes  UINT32
    POS_WINDOW               = 0x6067,   // RW  4 Bytes  UINT32
    POS_WINDOW_TIME          = 0x6068,   // RW  2 Bytes  UINT16
    VEL_FOLLOWING_ERR_WINDOW = 0x3B60,
    MAX_TORQUE               = 0x6072,   // RW  2 Bytes  UINT16
    MAX_CURRENT              = 0x6073,   // RW  2 Bytes  UINT16
    TORQUE_DEMAND_VAL        = 0x6074,   // RO  2 Bytes  INT16
    MOTOR_RATED_CURRENT      = 0x6075,   // RW  4 Bytes  UINT32
    MOTOR_RATED_TORQUE       = 0x6076,   // RW  4 Bytes  UINT32
    SOFT_POS_LIMIT           = 0x607D,   // RW  4 Bytes  INT32,  subindex: 01h --> Min Limit, 02h --> Max Limit
    TORQUE_SLOP              = 0x6087,   // RW  4 Bytes  UINT32
    MOONS_X_BITMASK          = 0x60FE,
    MOONS_Y_BITMASK          = 0x68FE,
    MOONS_Z_BITMASK          = 0X70FE
};

enum class SDO_COMMAND {
    BRAKE         = 0x00,
    RELEASE_BRAKE = 0x01,
    MOONS_BRAKE   = 0x02,
    MAX_VEL_ERR   = 1179640,    // countsetMaxPosErrs zeroerr
    MAX_POS_ERR   = 117964
};

enum class MotorType {
    MOONS    = 0,
    ZERO_ERR = 1,
    MAXON    = 2,
    KUNWEI   = 3,
    ATI   = 4
};

/*Define the operation mode of servo.
 Notice: 1. Support switching in the operation enabled state (still stand).
         2. Before switching to PP/CSP, assign actual position to target position.*/
enum class OperationMode {
    PP    = 0x01,            
    PV    = 0x03,            
    PT    = 0x04,            
    HOME  = 0x06,          // Home mode (Moons Motor only)
    CSP   = 0x08,           
    CSV   = 0x09,                 
    CST   = 0x0A                 
};

// this is for 0x6040, set control word
enum class ControlCommand{
    DISABLE         = 0x00,
    DISABLE1        = 0x02,
    SHUT_DOWN       = 0x06,
    SWITCH_ON       = 0x07,
    ENABLE          = 0x0F,
    MOTION_START_HOMING   = 0x1F,
    ENABLE_PP       = 0x2F,//使能进入pp模式，准备接受位置命令
    MOTION_START_PP = 0x3F,//targetpose发送绝对位置
    CLEAR_ERROR     = 0x80,
    NEW_SET_POINT_MOONS = 0x5F,//targetpose发送相对位置
    QUICKSTOP       = 0x0B,
    HALTHOMING      = 0x11F,

};

struct PDOConfig {
    struct Variable {
        std::string type;
        int offset;
    };

    struct Layer {
        std::unordered_map<std::string, Variable> variables;
    };

    Layer RxPDO;//zero 44
    Layer TxPDO;//zero 26
};

class  MotorDriver: public QObject{

    Q_OBJECT

public:
//    MotorDriver(MessageQueue&  messagePool):m_messagePool(messagePool){};
    explicit MotorDriver(const MotorDriverParameter& t_motorDriverParameter, MessageQueue&  messagePool):
        m_motorDriverparameter(t_motorDriverParameter),
        m_messagePool(messagePool),
        m_threadTerminated(false),
        m_isMotorDriverOk(false),
        m_endInstrumentMotorNum(t_motorDriverParameter.endInstrumentMotorNum),
        m_endInstrumentMotorNumPerArm(t_motorDriverParameter.endInstrumentMotorNumPerArm),
        m_endJointMotorNum(t_motorDriverParameter.endJointMotorNum),
        m_endJointMotorNumPerArm(t_motorDriverParameter.endJointMotorNumPerArm),
        m_endGimbalMotorNum(t_motorDriverParameter.endGimbalMotorNum),
        m_endGimbalMotorNumPerArm(t_motorDriverParameter.endGimbalMotorNumPerArm),
        m_guidingJointMotorNum(t_motorDriverParameter.guidingJointMotorNum),
        m_forceSensorNumPerArm(t_motorDriverParameter.forceSensorNumPerArm),
        m_motorNum(t_motorDriverParameter.motorNum),
        m_slaveNum(t_motorDriverParameter.slaveNum)
        {

            LOG(INFO) << "Start motorDriver construction! ";

            loadPDOMapping();
            m_init.init_options = CIFX_DRIVER_INIT_AUTOSCAN;

            m_init.iCardNumber = 0;
            m_init.fEnableCardLocking = 0;
            m_init.base_dir = NULL;
            m_init.poll_interval = 0;
            m_init.poll_StackSize = 0;
            m_init.trace_level = 255;
            m_init.user_card_cnt = 0;
            m_init.user_cards = NULL;

            m_abSendDataByteNum = m_motorDriverparameter.guidingJointMotorNum * guidingJointMotor_sizeSendData +
                                  (m_motorDriverparameter.endJointMotorNumPerArm * endJointMotor_sizeSendData +
                                   m_motorDriverparameter.endGimbalMotorNumPerArm * endGimbalMotor_sizeSendData +
                                   m_motorDriverparameter.endInstrumentMotorNumPerArm * endInstrumentMotor_sizeSendData
                                   ) * m_motorDriverparameter.armNum;

            m_abRecvDataByteNum = m_motorDriverparameter.guidingJointMotorNum * guidingJointMotor_sizeRecvData +
                                  (m_motorDriverparameter.endJointMotorNumPerArm * endJointMotor_sizeRecvData +
                                   m_motorDriverparameter.endGimbalMotorNumPerArm * endGimbalMotor_sizeRecvData +
                                   m_motorDriverparameter.endInstrumentMotorNumPerArm * endInstrumentMotor_sizeRecvData
                                   ) * m_motorDriverparameter.armNum;

            m_abRecvDataLengthGuiding = m_motorDriverparameter.guidingJointMotorNum * guidingJointMotor_sizeRecvData;

            m_abSendDataLengthGuiding = m_motorDriverparameter.guidingJointMotorNum * guidingJointMotor_sizeSendData;
            LOG(INFO) << "m_abRecvDataLengthGuiding: " << m_abRecvDataLengthGuiding;
            LOG(INFO) << "m_abSendDataLengthGuiding: " << m_abSendDataLengthGuiding;
            m_abRecvDataLengthPerArm = m_motorDriverparameter.endJointMotorNumPerArm * endJointMotor_sizeRecvData +
                                       m_motorDriverparameter.endGimbalMotorNumPerArm * endGimbalMotor_sizeRecvData +
                                       m_motorDriverparameter.endInstrumentMotorNumPerArm * endInstrumentMotor_sizeRecvData;

            m_abSendDataLengthPerArm = m_motorDriverparameter.endJointMotorNumPerArm * endJointMotor_sizeSendData +
                                       m_motorDriverparameter.endGimbalMotorNumPerArm * endGimbalMotor_sizeSendData +
                                       m_motorDriverparameter.endInstrumentMotorNumPerArm * endInstrumentMotor_sizeSendData;

            m_jointEnabled = new bool[t_motorDriverParameter.motorNum];
            for(int i = 0; i < t_motorDriverParameter.motorNum; i++){
                m_jointEnabled[i] = false;
            }

            connect(this, &MotorDriver::DealMsgSignal, this, &MotorDriver::dealWithMsg);
        }

    friend class RobotControl;

    void loadPDOMapping();
    void initMyData();
    void dumpPacket(CIFX_PACKET* ptPacket);  
    void dumpData(unsigned char* data, unsigned long dataLength);

    /* parse information from unsigned char array m_abRecvData, which belongs to TxPDO */
    uint16_t getErrorCode(const MotorType& type, const int& index, const int& armNum);
    uint16_t getStatusWord(const MotorType& type, const int& index, const int& armNum);
    int16_t getOperationMode(const MotorType& type, const int& index, const int& armNum);
    int32_t getActualPos(const MotorType& motorType, const int& index, const int& armNum);
    int32_t getActualVel(const MotorType& type, const int& index, const int& armNum);
    int16_t getActualTrq(const MotorType& type, const int& index, const int& armNum);
    int16_t getActualCur(const MotorType& type, const int& index, const int& armNum);
    std::array<int, 8> getDigitalInputs(const MotorType& type, const int& index, const int& armNum);
    int32_t getFollowingPosErr(const MotorType& type, const int& index, const int& armNum);

    /* write data to m_abSendData, which belongs to RxPDO, return 1 if succeed, else return 0 */
    int setControlWord(const MotorType& type, const int& index, const ControlCommand& cmd, const int& armNum);
    int setOperationMode(const MotorType& type, const int& index, const OperationMode& mode, const int& armNum);
    int setTargetPos(const MotorType& type, const int& index, const int32_t& targetPos, const int& armNum);
    int setTargetVel(const MotorType& type, const int& index, const int32_t& targetVel, const int& armNum);
    int setTargetTrq(const MotorType& type, const int& index, const int16_t& targetTrq, const int& armNum);
    int setVelOffset(const MotorType& type, const int& index, const int32_t& velOffset, const int& armNum);
    int setTrqOffset(const MotorType& type, const int& index, const int16_t& trqOffset, const int& armNum);
    int setDigitalOutputs(const MotorType& type, const int& index, const uint32_t& digitalOutputs, const int& armNum);
    int setProfileVel(const MotorType& type, const int& index, const uint32_t& profileVel, const int& armNum);
    int setProfileAcc(const MotorType& type, const int& index, const uint32_t& profileAcc, const int& armNum);
    int setProfileDec(const MotorType& type, const int& index, const uint32_t& profileDec, const int& armNum);
    int setMaxProfileVel(const MotorType& type, const int& index, const uint32_t& maxProfileVel, const int& armNum);
    int setTrqPosLimit(const MotorType& type, const int& index, const uint16_t& trqPosLimit, const int& armNum);
    int setTrqNegLimit(const MotorType& type, const int& index, const uint16_t& trqNegLimit, const int& armNum);

    int setHomeMethod(const MotorType& type, const int& index, const int& homeMethod, const int& armNum);
    int setHomeAcc(const MotorType& type, const int& index, const int32_t& homeAcc, const int& armNum);
    int setHomeOffset(const MotorType& type, const int& index, const int32_t& homeOffset, const int& armNum);
    int setHomeVel(const MotorType& type, const int& index, const int32_t& homeVel, const int& armNum);
    int setInterpolationTime(const MotorType& type, const int& index, const int& interpolationTime, const int& armNum);
    int setBias(const int& controlWord, const int& armNum = 0);

    /* Write SDO data, write specific value to a specific object index */
    /* Format of the packet for SDO writing, Packet head information, refer to the definition of CIFX_PACKET */
    int setBrake(const int& jointIndex, const SDO_COMMAND& sdoCmd);
    int setMaxVelErr(const MotorType& type, const int& jointIndex, const SDO_COMMAND& sdoCmd, const int& armNum);
    int setMaxPosErr(const MotorType& type, const int& jointIndex, const SDO_COMMAND& sdoCmd, const int& armNum);
    int readMaxVelErr(const MotorType& type, const int& jointIndex, const int& armNum);
    int readMaxPosErr(const MotorType& type, const int& jointIndex, const int& armNum);
    int setMotorLock(const MotorType& type,const MotorServoObjectIndex& index);
    void motorLockOpen(const MotorType& type, const int& index, const MotorServoObjectIndex& sdo_index);
    void motorLockClosed(const MotorType& type, const int& index, const MotorServoObjectIndex& sdo_index);

    /* set and get master state and slave state. */
    int setECatMasterState(const MasterState& targetState);
    int getECatMasterState();
    int setECatSlaveState(const SlaveState& targetState, const int& slaveIdx);
    int getECatSlaveState(const int& slaveIdx);

    int motorDriverInit();
    int mailboxPacketTransfer();
    int openBusConnection();
    int closeBusConnection();
    int cyclicDataTransfer();

    /* Different Modes, CSP/CSV/CST, change of modes are only possible in enabled still state. */
    void enableMotor(const MotorType& type, const int& index, const int& armNum);  // before starting motor/after release brake
    void enableMotor_PP(const MotorType& type, const int& index, const int& armNum);
    void enableMotor_Homing(const MotorType &type, const int &index, const int& armNum);
    void operationCSP(const MotorType& type, const int& index, const int& armNum);
    void operationCSV(const MotorType& motorType, const int& index, const int& armNum);
    void operationCST(const MotorType& type, const int& index, const int& armNum);
    void operationPP(const MotorType& type, const int& index, const int& armNum);
    void gotoTargetPos_PPMode(const MotorType& type, const int& index, const int32_t& targetVel, const int32_t& targetPos, const int& armNum);
    void operationHOME(const MotorType& type, const int& index, const int& armNum);
    void motorDriverExit();
    int getAbSendDataByteNum(){return m_abSendDataByteNum;}
    int getAbRecvDataByteNum(){return m_abRecvDataByteNum;}
    static void motorDriverThread(std::promise<bool> &promiseCommunication);//(std::promise<bool> &promiseCommunication)
    static MotorDriver* getInstance(const MotorDriverParameter motorDriverParameter, MessageQueue&  messagePool);

    void startThread(std::promise<bool> &promiseCommunication);//    void startThread(std::promise<bool> &promiseCommunication);
    int  checkECatStationState();
    int  checkMotorState();
    void displayMotorErrCode();

    bool isMotorDriverThreadTerminated();

    bool returnMotorDriverStatus(){return m_isMotorDriverOk.load();}

    /* MessageQueue relative function */
    void GetAmMsg(Message_Inner_T msg);

private:

    CIFX_PACKET m_tSendPkt = {{0}};
    CIFX_PACKET m_tRecvPkt = {{0}};
    int           m_abSendDataByteNum;
    int           m_abRecvDataByteNum;
    unsigned char m_abSendData[870] = {0}; /* with full topology: 9*ZE+2*MOONS+12*MAXON : 9 * 44 + 2 * 27 + 35 * 12*/
    unsigned char m_abRecvData[628] = {0}; /* with full topology: 9*ZE+2*MOONS+12*MAXON : 9 * 26 + 2 * 23 + 29 * 12*/
    std::string m_mappingPath = "/home/a/Desktop/codes/MikroPlanckV1/Config/PDO_mapping.toml";
    PDOConfig m_config[5] = {};
    struct CIFX_LINUX_INIT m_init;
    CIFXHANDLE m_hDriver = NULL;
    CIFXHANDLE m_hChannel = NULL;
    
    MotorDriverParameter m_motorDriverparameter;

    int m_endInstrumentMotorNum;
    int m_endInstrumentMotorNumPerArm;
    int m_endJointMotorNum;
    int m_endJointMotorNumPerArm;
    int m_endGimbalMotorNum;
    int m_endGimbalMotorNumPerArm;
    int m_guidingJointMotorNum;
    int m_forceSensorNum;
    int m_forceSensorNumPerArm;
    int m_motorNum;
    int m_slaveNum;
    int m_abRecvDataLengthPerArm;
    int m_abSendDataLengthPerArm;
    int m_abRecvDataLengthGuiding;
    int m_abSendDataLengthGuiding;

    static MotorDriver *m_selfPointer;
    bool m_flagSDO;
    bool *m_jointEnabled = nullptr;
    std::atomic<bool> m_isMotorDriverOk;
    std::atomic<bool> m_threadTerminated;

    std::mutex m_myMutex;
    std::thread m_etherCatThread;

    //MessageQueue relative function
    MessageQueue            &m_messagePool;
    QQueue<Message_Inner_T> m_MsgGottenQueue;
    QReadWriteLock          m_MsgGottenRWLock;
    void                    SendInnerMsg(Module_Inner_E recever,int Action,QString arg);
    void                    SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist);
    void                    dealWithMsg();


signals:
   void     DealMsgSignal();
   void     SendMsgSignal(const Message_Inner_T msg);
};

#endif
