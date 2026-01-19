#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <chrono>
#include <time.h>
#include <unistd.h>
#include <cmath>
#include <vector>
#include <array>
#include <iostream>
#include <bits/stdint-uintn.h>
#include <queue>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/LU>
#include <eigen3/Eigen/Geometry>
#include <deque>
using JointCoords = Eigen::Vector3d;
using CartesianCoords = Eigen::Vector3d;
using JacobianType = Eigen::Matrix3d;
using InvJacobianType = Eigen::Matrix3d;
using JointVel = Eigen::Vector3d;
using DigitalInput = std::array<int, 8>;

#define T_NOERROR ((int32_t)1)
#define T_ERROR ((int32_t)0)
using Mat3 = Eigen::Matrix3d;
using Vec3 = Eigen::Vector3d;

constexpr double PI = 3.14159265358979323846;

/*check if any key is pressed*/
static int kbhit()
{
  struct termios oldt, newt;
  int    ch;
  int    oldf;
  int    iRet = 0;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    iRet = 1;
  }
  return iRet;
}

inline short int hex2Int8(unsigned char byte){
    short int value = static_cast<short int>(byte);
    return value;
}

inline uint16_t hex2Uint16(unsigned char byte_1, unsigned char byte_2){
    uint16_t value = 0;
    value |= static_cast<uint16_t>(byte_1);
    value |= static_cast<uint16_t>(byte_2) << 8;
    return value;
}

inline int16_t hex2Int16(unsigned char byte_1, unsigned char byte_2) {

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

inline int32_t hex2Int32(unsigned char byte_1, unsigned char byte_2, unsigned char byte_3, unsigned char byte_4) {

    int32_t value = 0;
    value |= static_cast<int32_t>(byte_1);
    value |= static_cast<int32_t>(byte_2) << 8;
    value |= static_cast<int32_t>(byte_3) << 16;
    value |= static_cast<int32_t>(byte_4) << 24;
    return value;
}


inline void int32ToBytes(const int32_t& value, unsigned char* bytes) {
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



inline void uint32ToBytes(const uint32_t& value, unsigned char* bytes){

    bytes[0] = static_cast<unsigned char>(value & 0xFF);
    bytes[1] = static_cast<unsigned char>((value >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((value >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((value >> 24) & 0xFF);
}


inline void int16ToBytes(const int16_t& value, unsigned char* bytes) {

    int tmpValue = value;
    if (value < 0) {
        tmpValue =  ~(- tmpValue) + 1;
    }
    bytes[0] = static_cast<unsigned char>(tmpValue & 0xFF);
    bytes[1] = static_cast<unsigned char>((tmpValue >> 8) & 0xFF);

}


inline void uint16ToBytes(const uint16_t& value, unsigned char* bytes){
    bytes[0] = static_cast<unsigned char>(value & 0xFF);
    bytes[1] = static_cast<unsigned char>((value >> 8) & 0xFF);
}

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

// inline double sigmoid(const double& xVal, const double& scaleFactor, const double& widthFactor){
//     double yVal = (1/(1 + exp(-xVal * widthFactor)) - 0.5) * 2 *scaleFactor;
//     return yVal;
// }

//template <typename T>
//T sigmoid(const T& xVal, const double& scaleFactor, const double& widthFactor){
//    T y_val = (1/(1 + exp(-xVal * widthFactor)) - 0.5) * 2 * scaleFactor;
//    return y_val;
//}

inline Eigen::Vector3d sigmoid(const Eigen::Vector3d& tmpVal, const double& scaleFactor, const double& widthFactor){
    Eigen::Vector3d val(tmpVal);
    for(size_t i = 0; i < 3; i++){
        val[i] =  (1/(1 + exp(-tmpVal[i] * widthFactor)) - 0.5) * 2 * scaleFactor;
    }
    return val;
}

inline Eigen::Vector3d wipeOffSmallVal(const Eigen::Vector3d& tmpVal, const double& threshold){
    Eigen::Vector3d val(tmpVal);
    for(size_t i = 0; i < 3; i++){        
        if (std::fabs(tmpVal[i]) <= threshold){
            val[i] = 0. ;
        } 
        else if(tmpVal[i] > threshold){
            val[i] = tmpVal[i] - threshold;
        }
        else{
            val[i] = tmpVal[i] + threshold;
        }
    }
    return val;
}

inline double smoothTransition(const double& upperLimit, const double& lowerLimit, const double& leftRange, const double& rightRange, const double& input){
    const auto midRange = (leftRange + rightRange)/2;
    const auto output = (upperLimit + lowerLimit)/2 - (upperLimit-lowerLimit)/2 * std::sin(M_PI * (input - midRange)/(rightRange - midRange) - M_PI/2);
    return output;
}

template <typename T, size_t N>
std::array<T, N> operator*(const std::array<T, N>& arr, const T& scalar) {
    std::array<T, N> result;
    for (size_t i = 0; i < N; i++) {
        result[i] = arr[i] * scalar;
    }
    return result;
}

template <typename T, size_t N>
std::array<T, N> operator*(const T& scalar, const std::array<T, N>& arr) {
    return arr * scalar;  
}

template <typename T, size_t N>
Eigen::VectorXd operator*(const Eigen::Matrix<T, N, N>& matrix, const std::array<T, N> arr){
    Eigen::VectorXd tmpVector(N);
    for(size_t i = 0; i < N; i++){
        tmpVector[i] = arr[i];
    }
    return matrix * arr; 
}

template <typename T, size_t N>
std::array<T, N> operator+(const std::array<T, N>& arr1, const std::array<T, N>& arr2) {
    std::array<T, N> result;
    for (size_t i = 0; i < N; i++) {
        result[i] = arr1[i] + arr2[i];
    }
    return result;
}

template <typename T, size_t N>
std::array<T, N> operator-(const std::array<T, N>& arr1, const std::array<T, N>& arr2) {
    std::array<T, N> result;
    for (size_t i = 0; i < N; i++) {
        result[i] = arr1[i] - arr2[i];
    }
    return result;
}

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& arr) {
    os << "[";
    for (size_t i = 0; i < N; i++) {
        os << arr[i];
        if (i < N - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

template<typename T>
T rad2deg(T rad){
    return rad/M_PI * 180;
}

template<typename T>
T deg2rad(T degree){
    return degree/180 * M_PI;
}

template<typename T, std::size_t N>
void arrayThreshold(std::array<T, N>& arr, const T& threshold) {
    for (auto& element : arr) {
        if (element > threshold) {
            element = threshold;
        }
        else if(element < -threshold){
            element = -threshold;
        }
    }
}

// // functions for rotation
// Eigen::Matrix3d RotX_deg(double deg){
//     double rad = deg2rad(deg);
//     return Eigen::AngleAxisd(rad, Eigen::Vector3d::UnitX()).toRotationMatrix();
// }
// Eigen::Matrix3d RotY_deg(double deg){
//     double rad = deg2rad(deg);
//     return Eigen::AngleAxisd(rad, Eigen::Vector3d::UnitY()).toRotationMatrix();
// }
// Eigen::Matrix3d RotZ_deg(double deg){
//     double rad = deg2rad(deg);
//     return Eigen::AngleAxisd(rad, Eigen::Vector3d::UnitZ()).toRotationMatrix();
// }

// // --------- 3D vector rotation ---------
// Eigen::Vector3d rotAroundX(const Eigen::Vector3d& v, double angle_deg){
//     return RotX_deg(angle_deg) * v;
// }
// Eigen::Vector3d rotAroundY(const Eigen::Vector3d& v, double angle_deg){
//     return RotY_deg(angle_deg) * v;
// }
// Eigen::Vector3d rotAroundZ(const Eigen::Vector3d& v, double angle_deg){
//     return RotZ_deg(angle_deg) * v;
// }

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

template <typename T>
class ButterWorthIIR {

public:
    explicit ButterWorthIIR(const T& init) : m_yPre(init), m_yPrePre(init), m_xPre(init), m_xPrePre(init) {};
    T filter(const T& x) const {
        const auto y = (x * num[0] + m_xPre * num[1] + m_xPrePre * num[2]  - m_yPre * den[1] - m_yPrePre * den[2]);
        m_yPrePre = m_yPre;
        m_yPre = y;
        m_xPrePre = m_xPre;
        m_xPre = x;
        return y;
    };

private:
    //const std::vector<double> num{ 0.000241, 0.000482, 0.000241 };  // Numerator coefficients
    //const std::vector<double> den{ 1.0, -1.9556, 0.9566 };  // Denominator coefficients
    const std::vector<double> num{ 0.000943, 0.0019, 0.000943 };  // Numerator coefficients
    const std::vector<double> den{ 1.0, -1.9113, 0.9150 };  // Denominator coefficients
    mutable T m_yPre;
    mutable T m_yPrePre;
    mutable T m_xPre;
    mutable T m_xPrePre;
};


inline std::string getCurrentTimeAsString() {
    std::time_t currentTime;
    std::time(&currentTime);

    struct tm localTime;
    localtime_r(&currentTime, &localTime);

    char buffer[80];
    std::strftime(buffer, sizeof (buffer), "%Y%m%d_%H%M%S", &localTime);
    return buffer;
}

//split函数
inline std::vector<std::string> split(std::string str,std::string separator){
    std::vector<std::string> result;
    int cutAt;
    while((cutAt = str.find_first_of(separator))!=str.npos){
        if(cutAt>0){
            result.push_back(str.substr(0,cutAt));
        }
        str=str.substr(cutAt+1);
    }
    if(str.length()>0){
        result.push_back(str);
    }
    return result;
}

template <typename T>
class MedianFilter{
public:
    MedianFilter(int windowSize) :windowSize_(windowSize) {}

    T getMedian(){
        std::deque<T> process_buffer = buffer_;
        std::sort(process_buffer.begin(),process_buffer.end());
        if(process_buffer.size()<windowSize_){
            return process_buffer[process_buffer.size() / 2];
        }
        return process_buffer[windowSize_ / 2];
    }
    void add(T newValue){
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
    std::deque<T> buffer_;
    int windowSize_;
};

template <typename T, std::size_t N>
class MoveMeanArray {
public:
    explicit MoveMeanArray(int windowSize)
        : windowSize_(windowSize), sum_{} {}

    // 添加一个新值
    void add(const std::array<T, N>& newValue) {
        if (buffer_.size() < windowSize_) {
            buffer_.push_back(newValue);
            for (std::size_t i = 0; i < N; ++i)
                sum_[i] += newValue[i];
        } else {
            // 移除最老的
            const auto& old = buffer_.front();
            for (std::size_t i = 0; i < N; ++i)
                sum_[i] -= old[i];
            buffer_.pop_front();

            // 加入新的
            buffer_.push_back(newValue);
            for (std::size_t i = 0; i < N; ++i)
                sum_[i] += newValue[i];
        }
    }

    // 获取当前窗口的平均
    std::array<T, N> getMean() const {
        std::array<T, N> mean{};
        if (buffer_.empty()) return mean;

        double denom = static_cast<double>(buffer_.size());
        for (std::size_t i = 0; i < N; ++i)
            mean[i] = static_cast<T>(sum_[i] / denom);

        return mean;
    }

    int size() const { return buffer_.size(); }
    int capacity() const { return windowSize_; }

private:
    std::deque<std::array<T, N>> buffer_;
    int windowSize_;
    std::array<long double, N> sum_;  // 累计和，避免精度损失
};



#endif 
