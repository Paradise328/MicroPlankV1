#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <cmath>
#include <array>
#include <iostream>
#include <bits/stdint-uintn.h>
#include <queue>
#include <deque>
#include <algorithm>
#include <QCoreApplication>
#include <QEventLoop>

#define T_NOERROR ((int32_t)1)
#define T_ERROR ((int32_t)0)


//#define SteadyDelay(time_ms)\
//{\
//    auto Delay_start = std::chrono::steady_clock::now();\
//    auto Delay_end = Delay_start;\
//    std::chrono::duration<double> Delay_time;\
//      do{\
//           Delay_end = std::chrono::steady_clock::now();\
//           Delay_time =Delay_end-Delay_start;\
//           if((Delay_time.count()*1000)>=time_ms)\
//           {\
//               break;\
//           }\
//            QCoreApplication::processEvents(QEventLoop::AllEvents,2);\
//      }while(1);\
//}

//#define SteadyDelay(time_ms)std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));

//#define SteadyDelay(time_ms){\
//    int time_ms_temp=time_ms;\
//    while(time_ms_temp>0)\
//    {    QCoreApplication::processEvents(QEventLoop::AllEvents,1);\
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));\
//         QCoreApplication::processEvents(QEventLoop::AllEvents,1);\
//         time_ms_temp--;\
//    }\
//    }\

#define SteadyDelay(time_ms)\
{\
    auto Delay_start = std::chrono::steady_clock::now();\
    auto Delay_end = Delay_start;\
    std::chrono::duration<double> Delay_time;\
      do{\
           Delay_end = std::chrono::steady_clock::now();\
           Delay_time =Delay_end-Delay_start;\
           if((Delay_time.count()*1000)>=time_ms)\
           {\
               break;\
           }\
           QCoreApplication::processEvents(QEventLoop::AllEvents,1);\
           std::this_thread::sleep_for(std::chrono::microseconds(500));\
      }while(1);\
}


//#define SteadyDelay(time_ms)\
//{\
//    QEventLoop eventloop;\
//    QTimer tim;\
//    t.setSingleShot(true);\
//    connect(&t,&QTimer::timeout,&eventloop,&QEventLoop::quit);\
//    t.start(time_ms);\
//    eventloop.exec();\
//}

/*check if any key is pressed*/


/*Convert hex data to decimal depends on the variable type*/
//short int is int16, there is no int8 (which is unsigned/signed char in C++)
inline short int hex2Int8(unsigned char byte){
    short int value = static_cast<short int>(byte);
    return value;
}

/*Convert hex data to decimal depends on the variable type
  byte_1: low byte, byte2: high byte*/
inline uint16_t hex2Uint16(unsigned char byte_1, unsigned char byte_2){
    uint16_t value = 0;
    value |= static_cast<uint16_t>(byte_1);
    value |= static_cast<uint16_t>(byte_2) << 8;
    return value;
}

inline int16_t hex2Int16(unsigned char byte_1, unsigned char byte_2){
    /*bool flagBit = (byte_2 & 0x80) != 0;*/
    int16_t value = 0;
    value |= static_cast<int16_t>(byte_1);
    value |= static_cast<int16_t>(byte_2) << 8;
    return value;
}


inline uint32_t hex2Uint32(unsigned char byte_1, unsigned char byte_2, unsigned char byte_3, unsigned char byte_4){
    uint32_t value = 0;

    value |= static_cast<uint32_t>(byte_1);
    value |= static_cast<uint32_t>(byte_2) << 8;
    value |= static_cast<uint32_t>(byte_3) << 16;
    value |= static_cast<uint32_t>(byte_4) << 24;
    return value;

}

inline int32_t hex2Int32(unsigned char byte_1, unsigned char byte_2, unsigned char byte_3, unsigned char byte_4){
    /*bool flagBit = (byte_4 & 0x80) != 0;*/
    int32_t value = 0;
    value |= static_cast<int32_t>(byte_1);
    value |= static_cast<int32_t>(byte_2) << 8;
    value |= static_cast<int32_t>(byte_3) << 16;
    value |= static_cast<int32_t>(byte_4) << 24;
    return value;
}

// convert int32 to bytes
inline void int32ToBytes(const int32_t& value, unsigned char* bytes){
    // convert to a pos. integer for postprocessing
    int tmpValue = value;
    if (value < 0) {
        tmpValue = ~(- tmpValue) + 1;
    }

    bytes[0] = static_cast<unsigned char>(tmpValue & 0xFF);
    bytes[1] = static_cast<unsigned char>((tmpValue >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((tmpValue >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((tmpValue >> 24) & 0xFF);
}

// convert uint32 to bytes
inline void uint32ToBytes(const uint32_t& value, unsigned char* bytes){
  // convert to a pos. integer for postprocessing
  bytes[0] = static_cast<unsigned char>(value & 0xFF);
  bytes[1] = static_cast<unsigned char>((value >> 8) & 0xFF);
  bytes[2] = static_cast<unsigned char>((value >> 16) & 0xFF);
  bytes[3] = static_cast<unsigned char>((value >> 24) & 0xFF);
}

// convert int16 to bytes
inline void int16ToBytes(const int16_t& value, unsigned char* bytes){
    // convert to a pos. integer for postprocessing
    int tmpValue = value;
    if (value < 0) {
        tmpValue =  ~(- tmpValue) + 1;
    }
    bytes[0] = static_cast<unsigned char>(tmpValue & 0xFF);
    bytes[1] = static_cast<unsigned char>((tmpValue >> 8) & 0xFF);

}

// convert uint16 to bytes
inline void uint16ToBytes(const uint16_t& value, unsigned char* bytes){
  bytes[0] = static_cast<unsigned char>(value & 0xFF);
  bytes[1] = static_cast<unsigned char>((value >> 8) & 0xFF);
}

//
inline void shortIntToBytes(short int& value, unsigned char* bytes){
  int tmpValue = value;
  if (value < 0){
    tmpValue *= -1;
  }
  bytes[0] = static_cast<unsigned char> (value & 0xFF);
  if (value < 0){
    bytes[1] |= 0x80;
  }
}

inline double sigmoid(const double& xVal, const double& scaleFactor, const double& widthFactor){
    double yVal = (1/(1 + exp(-xVal * widthFactor)) - 0.5) * 2 *scaleFactor;
    return yVal;
}


class AverageFilter {
private:
    std::queue<float> m_que;
    int m_size;
    float m_sum;
public:
    /** Initialize your data structure here. */
    AverageFilter(int size){
        m_size = size;
        m_sum = 0;
        while(m_que.size()) m_que.pop();
    }
    float next(float val){
        m_sum += val;
        m_que.push(val);
        while(m_que.size() > m_size){
            m_sum -= m_que.front();
            m_que.pop();
        }
        return m_sum / (float)m_que.size();
    }
};

class MedianFilter {
public:
    MedianFilter(int windowSize) :windowSize_(windowSize) {}

    int32_t getMedian(){
        std::deque<int32_t>process_buffer=buffer_;
        std::sort(process_buffer.begin(),process_buffer.end());
        if(process_buffer.size()<windowSize_){
            return process_buffer[process_buffer.size() / 2];
        }
        return process_buffer[windowSize_ / 2];
    }
    void add(int32_t newValue){
        if(buffer_.size()< windowSize_){
            buffer_.push_back(newValue);
        }else{
            buffer_.pop_front();
            buffer_.push_back(newValue);
        }

    }
    int size(){
        return windowSize_;
    }
private:
    std::deque<int32_t> buffer_;
    int windowSize_;
};

#endif // UTILS_H
