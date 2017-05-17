/*********************************************************** 
* Date: 2016-06-30 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 计时器 
* 
* Brief: 
* 
* Note: 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef	__MY_TIMER_HPP_BY_MOUYUN_2014_09_28__
#define	__MY_TIMER_HPP_BY_MOUYUN_2014_09_28__

#include <chrono>

namespace my_module_space
{
    class timer
    {
    public:
        timer() : m_begin(std::chrono::high_resolution_clock::now()) {}
        ~timer() {}

    public:
        void reset()
        {
            m_begin = std::chrono::high_resolution_clock::now();
        }

        double tell_s() const
        {
            return std::chrono::duration_cast<std::chrono::duration<double> >(std::chrono::high_resolution_clock::now() - m_begin).count();
        }

        long long tell_ms() const
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
        }

        long long tell_us() const
        {
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
        }

        long long tell_ns() const
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
    };


}

#endif
