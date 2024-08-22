#include <eigen3/Eigen/Dense>
#include <atomic>
#include "easylogging++.h"
#include "MathUtils.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include "viper_ui.h"
#include "SharedStruct.h"

enum class MasterConsoleType
{
    Viper           = 0x00,
    Omega           = 0x01,
    DessightMaster  = 0x02,
};


class MasterConsole
{
  
public:
    MasterConsole() = delete;
    explicit MasterConsole(MasterConsoleType masterConsoleType):
                            m_MasterConsoleType(masterConsoleType)
                            // m_FilterCase(FilterCase::FilterOFF)
                            {
                                m_FilterCase.store(static_cast<int>(FilterCase::FilterOFF));
                            };

    void                        updateConsoleDataThread();
    void                        startUpdateConsoleDataThread();

    HandlePose                  returnHandlePose(){return m_handlePose_Cur.load();}
    static void                 setMyFilterCase(const int& filterCase);

private:

    MasterConsoleType           m_MasterConsoleType; //Console Type;
    viper_ui                    m_viper;
    std::thread                 m_updateConsoleDataThread;
    void                        assembleDataFromUSBAndEthernet();

    bool                        m_isViperOk = false;
    bool                        m_is422Ok = false;

    mutable HandlePose          m_handlePose_Pre;
    mutable HandlePose          m_handlePose_PrePre;
    mutable HandlePose          m_handlePoseAF_Pre;
    mutable HandlePose          m_handlePoseAF_PrePre;
    std::atomic<HandlePose>     m_handlePose_Cur;
    std::atomic<std::array<std::array<double,viperDataNumPerSensor>,2>> m_sensorData_Cur;

    std::atomic<int>            m_FilterCase; //0: No Filter; 1: IR Filter; 2: IIR Filter
    const std::vector<double>   m_IRnum{0.0337, 0.0337};  // Numerator coefficients
    const std::vector<double>   m_IRden{1.0, -0.9325};
    const std::vector<double>   m_IIRnum{0.001160721940461, 0.002321443880922, 0.001160721940461};  // Numerator coefficients ()
    const std::vector<double>   m_IIRden{1.0, -1.901343793847116, 0.905986681608960};// (b)
    mutable std::array<std::array<double,viperDataNumPerSensor>,2> m_poseData_Cur, m_poseData_Pre, m_poseData_PrePre, m_poseDataAF_Pre, m_poseDataAF_PrePre; 
    std::array<std::array<double,viperDataNumPerSensor>,2>        returnIRFilteredData(const std::array<std::array<double,7>,2>& poseData_Cur);
    std::array<std::array<double,viperDataNumPerSensor>,2>        returnIIRFilteredData(const std::array<std::array<double,7>,2>& poseData_Cur);

    static MasterConsole        *m_selfPointer;

};