/**
 * @file lowpass_filter.cpp
 * @brief 
 * @author Hao,Sibo; Chen,Xinbo
 * @date 2025-01-16
 */

#pragma once


/**
 * @file lowpass_filter.h
 * Contains functions for filtering signals with low-pass filter.
 * this file contains two filters: 1st-order lp filter and 2nd-order lp filter
 * 
 */

 #include <array>
 #include <cmath>
 #include "MathUtils.h"
 
/**
 * Default cutoff frequency
 * TODO
 */
constexpr double kDefaulCutoffFrequency = 100.0;

/**
 * Maximum cutoff frequency
 * TODO
 */
constexpr double kMaximumCutoffFrequency = 100.0;


template <typename T>
class LowpassFilter2ndOrder{
public:
    struct Coefficients{
        double a0;
        double a1;
        double a2;
        double b0;
        double b1;
        double b2;
    };
    
    Coefficients m_filterCoefficient;
    
    double m_sampleRate = 0.0;
    double m_cutoffRate = 0.0;

    mutable T m_x_pre;
    mutable T m_x_prepre;
    mutable T m_y_pre;
    mutable T m_y_prepre;
    
    LowpassFilter2ndOrder(){}

    /**
    * Instantiate a first-order low-pass filter
    *
    * @param[in] sample_time sample time[s]
    * @param[in] cutoff_rate cut-off rate of the low-pass filter[Hz]
    *
    * @return Filtered value.
    */
    LowpassFilter2ndOrder(const T& init, double sample_rate, double cutoff_rate):
                                                            m_x_pre(init),
                                                            m_x_prepre(init),
                                                            m_y_pre(init),
                                                            m_y_prepre(init)
    {
        computeCoefficients(sample_rate, cutoff_rate);
    }

    /*
     *  calculate filter coefficients according to the sample rate and cutoff rate
    */
    int computeCoefficients(double sample_rate, double cutoff_rate) {
        double fr = sample_rate / cutoff_rate;
        double ohm = tan(M_PI / fr);
        double c = 1.0 + 2.0*cos(M_PI / 4.0) * ohm + ohm * ohm;

        m_filterCoefficient.b0 = ohm * ohm / c;
        m_filterCoefficient.b1 = 2.0 * m_filterCoefficient.b0;
        m_filterCoefficient.b2 = m_filterCoefficient.b0;
        m_filterCoefficient.a0 = 1.0;
        m_filterCoefficient.a1 = 2.0 * (ohm * ohm-1.0)/c;
        m_filterCoefficient.a2 = (1.0 - 2.0 * cos(M_PI / 4.0) * ohm + ohm * ohm)/c;
        
        //TODO: return update Status
        return 1;
    }
    
    T update(T x){
        T y = m_filterCoefficient.b0 * x +
            m_filterCoefficient.b1 * m_x_pre +
            m_filterCoefficient.b2 * m_x_prepre -
            m_filterCoefficient.a1 * m_y_pre -
            m_filterCoefficient.a2 * m_y_prepre;

        m_y_prepre = m_y_pre;
        m_y_pre = y;
        m_x_prepre = m_x_pre;
        m_x_pre = x;

        return y;
    }
    
    Coefficients getCoefficients() const{
        return m_filterCoefficient;
    }

};


template <typename T>
class LowpassFilter1stOrder{
public:
    struct Coefficients{
        double a1;
        double b0;
    };

    Coefficients m_filterCoefficient;

    double m_sample_rate = 0.0;
    double m_cutoff_rate = 0.0;

    mutable T m_y_pre;
    mutable T m_x_pre;

    LowpassFilter1stOrder(){}
    LowpassFilter1stOrder(const T& init, double sample_rate,double cutoff_rate):
                                                                m_y_pre(init),
                                                                m_x_pre(init)
    {
        computeCoefficients(sample_rate,cutoff_rate);
    }
    
    int computeCoefficients(double sample_rate,double cutoff_rate){
        double alpha = 2.0 * M_PI * cutoff_rate / 
                        (2.0 * M_PI * cutoff_rate + sample_rate);

        m_filterCoefficient.b0 = alpha;
        m_filterCoefficient.a1 = 1.0 - alpha;

        return 1;
    }
    
    T update(T x){
        T y = m_filterCoefficient.b0 * x +
          m_filterCoefficient.a1 * m_y_pre;
        
        m_y_pre = y;

        return y;
    }

    Coefficients getCoefficients() const{
        return m_filterCoefficient;
    }
};
