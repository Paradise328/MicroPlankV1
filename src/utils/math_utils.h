#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>
#include <iostream>
#include <chrono>
#include <fcntl.h>
#include <array>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <sstream>
#include <Eigen/Dense>


constexpr int degPerRound = 360;
using Pos = Eigen::Vector3d;
using Mat4 = Eigen::Matrix<double, 4, 4>;

template<typename T>
T rad2deg(T rad){
    return rad/M_PI * 180;
}

template<typename T>
T deg2rad(T degree){
    return degree/180 * M_PI;
}

template<typename T, std::size_t N>
void arrayMaxThreshold(std::array<T, N>& arr, const T& threshold) {
    for (auto& element : arr) {
        if (element > threshold) {
            element = threshold;
        }
        else if(element < -threshold){
            element = -threshold;
        }
    }
}

template<typename T, std::size_t N>
void arrayMinThreshold(std::array<T, N>& arr, const T& threshold) {
    for (auto& element : arr) {
        if (element < threshold) {
            element = threshold;
        }
    }
}

//array operators reload

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
    return arr * scalar;  // 倒序乘法，复用上面的重载函数
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























inline Mat4 frameTransferMatrix(double a11, double a12, double a13, double a14,
                         double a21, double a22, double a23, double a24,
                         double a31, double a32, double a33, double a34,
                         double a41, double a42, double a43, double a44){
    Mat4 matrix;
    matrix << a11, a12, a13, a14,
              a21, a22, a23, a24,
              a31, a32, a33, a34,
              a41, a42, a43, a44;
    return matrix;
}

inline int scanKeyboard()
{
    int input;
    struct termios new_settings;
    struct termios stored_settings;
    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0,&stored_settings);
    new_settings.c_cc[VMIN] = 0;
    tcsetattr(0,TCSANOW,&new_settings);

    input = getchar();

    tcsetattr(0,TCSANOW,&stored_settings);
    return input;
}

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















inline std::string getCurrentTimeAsString() {
    std::time_t currentTime;
    std::time(&currentTime);

    struct tm localTime;
    localtime_r(&currentTime, &localTime);

    char buffer[80];
    std::strftime(buffer, sizeof (buffer), "%Y%m%d_%H%M%S", &localTime);
    return buffer;
}


#endif // MATH_UTILS_H

