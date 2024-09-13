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
#include "MathUtils.h"
#include "easylogging++.h"


#define CIFX_DEV "PROFIBUS"
#define JOINT_NUM 1
#define MAX_PROFILE_VEL ((uint32_t) 314572)   // 314572 counts/s
#define PROFILE_ACC ((uint32_t) 3048575)  // 1048575 counts/s^2
#define PROFILE_DEC ((uint32_t) 3048575)  // 1048575 counts/s^2
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
#define MAXPROFILEVEL      "MaxProfileVel"
#define POSTRQLIMIT        "PosTrqLimit"
#define NEGTRQLIMIT        "NegTrqLimit"
#define DUMMYBYTE          "DummyByte"
#define STATUSWORD         "StatusWord"
#define MODEDISPLAY        "ModeDisplay"
#define ERRCODE            "ErrorCode"
#define ACTPOS             "ActPos"
#define ACTVEL             "ActVel"
#define ACTTRQ             "ActTrq"
#define ACTCUR             "ActCur"
#define DIGITALINPUT       "DigitalInputs"
#define FOLLOWINGPOSERR    "FollowingPosErr"
#define HOMEMETHOD         "HomeMethod"
#define HOMEOFFSET         "HomeOffset"
#define HOMEVEL            "HomeVel"
#define HOMEACC            "HomeAcc"

#define PROCESSID 1;

constexpr int zeroErr_sizeRecvData = 26;
constexpr int zeroErr_sizeSendData = 44;
constexpr int moons_sizeRecvData   = 21;
constexpr int moons_sizeSendData   = 36;
constexpr int maxon_sizeRecvData   = 27;
constexpr int maxon_sizeSendData   = 39;

constexpr int lengthSDODataHeader  = 18;
constexpr int timeOutOpenBus       = 10000;               // timeout value for open bus
constexpr int timeOutCyclicIO      = 10;                  // timeout value for cyclic data transfer
constexpr uint ulTimeout           = 1000;                // timeout value for SDO, ms
constexpr uint timeWaitMailBoxFree = 10;                  // time in ms to wait for the mailbox to get free
constexpr uint timeWaitMessage     = 10;                  // time in ms to wait for the message
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
    MAX_VEL_ERR   = 117964   // counts
};

enum class MotorType {
    ZERO_ERR = 0x00,
    MOONS    = 0x01,
    MAXON    = 0x02,
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
    COMMAND_TRIGGER = 0x1F,
    CLEAR_ERROR     = 0x80
};

struct PDOConfig {
    struct Variable {
        std::string type;
        int offset;
    };

    struct Layer {
        std::unordered_map<std::string, Variable> variables;
    };

    Layer RxPDO;
    Layer TxPDO;
};


class MotorDriver{
public:
    MotorDriver(){};
    explicit MotorDriver(std::string pathMappingPDO, int endMotorNum, int motorNumber, int slaveNumber):
        m_mappingPath(pathMappingPDO),
        m_endMotorNum(endMotorNum),
        m_motorNum(motorNumber),
        m_slaveNumber(slaveNumber),
        m_flagThreadTerminated(false){
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

        m_jointEnabled = new bool[endMotorNum + motorNumber];
        for(int i = 0; i < endMotorNum + motorNumber; i++){
            m_jointEnabled[i] = false;
        }
        m_counter = 0;
    };

    void loadPDOMapping();  
    void dumpPacket(CIFX_PACKET* ptPacket);  
    void dumpData(unsigned char* data, unsigned long dataLength);  

    // parse information from unsigned char array m_abRecvData, which belongs to TxPDO
    uint16_t getErrorCode(const MotorType& type, const int& index);
    uint16_t getStatusWord(const MotorType& type, const int& index);
    int16_t getOperationMode(const MotorType& type, const int& index);
    int32_t getActualPos(const MotorType& motorType, const int& index);
    int32_t getActualVel(const MotorType& type, const int& index);
    int16_t getActualTrq(const MotorType& type, const int& index);
    int16_t getActualCur(const MotorType& type, const int& index);
    std::array<int, 8> getDigitalInputs(const MotorType& type, const int& index);
    int32_t getFollowingPosErr(const MotorType& type, const int& index);

    //write data to m_abSendData, which belongs to RxPDO, return 1 if succeed, else return 0
    int setControlWord(const MotorType& type, const int& index, const ControlCommand& cmd);
    int setOperationMode(const MotorType& type, const int& index, const OperationMode& mode);
    int setTargetPos(const MotorType& type, const int& index, const int32_t& targetPos);
    int setTargetVel(const MotorType& type, const int& index, const int32_t& targetVel);
    int setTargetTrq(const MotorType& type, const int& index, const int16_t& targetTrq);
    int setVelOffset(const MotorType& type, const int& index, const int32_t& velOffset);
    int setTrqOffset(const MotorType& type, const int& index, const int16_t& trqOffset);
    int setDigitalOutputs(const MotorType& type, const int& index, const uint32_t& digitalOutputs);
    int setProfileVel(const MotorType& type, const int& index, const uint32_t& profileVel);
    int setProfileAcc(const MotorType& type, const int& index, const uint32_t& profileAcc);
    int setProfileDec(const MotorType& type, const int& index, const uint32_t& profileDec);
    int setMaxProfileVel(const MotorType& type, const int& index, const uint32_t& maxProfileVel);
    int setTrqPosLimit(const MotorType& type, const int& index, const uint16_t& trqPosLimit);
    int setTrqNegLimit(const MotorType& type, const int& index, const uint16_t& trqNegLimit);

    int setHomeMethod(const MotorType& type, const int& index, const int& homeMethod);
    int setHomeAcc(const MotorType& type, const int& index, const int32_t& homeAcc);
    int setHomeOffset(const MotorType& type, const int& index, const int32_t& homeOffset);
    int setHomeVel(const MotorType& type, const int& index, const int32_t& homeVel);

    //write SDO data, write specific value to a specific object index
    /*Format of the packet for SDO writing, Packet head information, refer to the definition of CIFX_PACKET*/
    int setBrake(const int& jointIndex, const SDO_COMMAND& sdoCmd);
    int setMaxVelErr(const int& jointIndex, const SDO_COMMAND& sdoCmd);
    int setMaxPosErr(const int& jointIndex, const SDO_COMMAND& sdoCmd);
    int setMotorLock(const MotorType& type,const MotorServoObjectIndex& index);
    void motorLockOpen(const MotorType& type, const int& index, const MotorServoObjectIndex& sdo_index);
    void motorLockClosed(const MotorType& type, const int& index, const MotorServoObjectIndex& sdo_index);

    // set and get master state and slave state.
    int setECatMasterState(const MasterState& targetState);
    int getECatMasterState();
    int setECatSlaveState(const SlaveState& targetState, const int& slaveIdx);
    int getECatSlaveState(const int& slaveIdx);

    int motorDriverInit();
    int mailboxPacketTransfer();
    int openBusConnection();
    int closeBusConnection();
    int cyclicDataTransfer();

    //Different Modes, CSP/CSV/CST, change of modes are only possible in enabled still state.
    void enableMotor(const MotorType& type, const int& index, const bool& flagPPMode);  // before starting motor/after release brake
    void operationCSP(const MotorType& type, const int& index);
    void operationCSV(const MotorType& motorType, const int& index);
    void operationCST(const MotorType& type, const int& index);
    void operationPP(const MotorType& type, const int& index);
    void operationHOME(const MotorType& type, const int& index);
    void motorDriverExit();
    static void motorDriverThread(std::promise<bool> &promiseCommunication);
    static MotorDriver* getInstance(const std::string pathMappingPDO, const int& jointNumber, const int& motorNumber, const int& slaveNumber);
    void startThread(std::promise<bool> &promiseCommunication);
    bool isMotorDriverThreadTerminated();

private:

    CIFX_PACKET m_tSendPkt = {{0}};
    CIFX_PACKET m_tRecvPkt = {{0}};
    unsigned char m_abSendData[39] = {0};
    unsigned char m_abRecvData[27] = {0};
    std::string m_mappingPath;
    PDOConfig m_config[3] = {};
    struct CIFX_LINUX_INIT m_init;
    CIFXHANDLE m_hDriver = NULL;
    CIFXHANDLE m_hChannel = NULL;
    
    int m_slaveNumber; 
    int m_counter = 0;
    int m_endMotorNum;
    int m_motorNum;
    bool m_flagSDO;
    bool *m_jointEnabled = nullptr;
    static MotorDriver *m_selfPointer;
    std::atomic<bool> m_flagThreadTerminated;
    std::mutex m_myMutex;
    std::thread m_etherCatThread;
};

#endif
