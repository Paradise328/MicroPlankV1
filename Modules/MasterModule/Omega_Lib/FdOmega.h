#ifndef FDOMEGA_H
#define FDOMEGA_H

#include <future>
#include <atomic>
#include <thread>
#include <eigen3/Eigen/Dense>
#include "Modules/MathModule/MathUtils.h"
#include "Modules/LoggerModule/easylogging++.h"
#include "dhdc.h"
#include "drdc.h"

/* To run the haptic device, need to give the permission
   ls -l /dev/bus/usb/00*
   sudo chmod o+w /dev/bus/usb/003/008  (use the real port entry)
*/
constexpr int omegaDataNumPerSensor = 7;

class FdOmega{
public:
    explicit FdOmega():
        m_threadTerminated(false)
        {
            m_isOmegaOK.store(false);
        };

    int initDevice();
    void turnOffDevice();
    void startThread();
    void masterHandleThread();
    bool returnOmegaStatus(){return m_isOmegaOK.load();}
    std::array<double,omegaDataNumPerSensor> return_omegaData(){return m_omegaPNOData.load();}


private:
    int                m_deviceID;
    std::atomic<bool>  m_isOmegaOK;

    std::atomic<bool>  m_threadTerminated;
    std::atomic<std::array<double,omegaDataNumPerSensor>> m_omegaPNOData;
};

#endif
