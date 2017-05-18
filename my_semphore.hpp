/*********************************************************** 
* Date: 2016-06-23 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 信号量 
* 
* Brief: 
* 
* Note: 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef __MY_SEMPHORE_HPP_BY_MOUYUN_2016_06_23__
#define __MY_SEMPHORE_HPP_BY_MOUYUN_2016_06_23__

#include <chrono>
#include <mutex>
#include <condition_variable>

namespace my_module_space
{
    enum 
    {
        SEMPHORE_FAIL = (-1),
        SEMPHORE_SUCCESS = 0,
        SEMPHORE_TIME_OUT = 1
    };

    class Semphore
    {
    public:
        Semphore(const int sigs = 0) : m_signals(sigs), m_blocked(0) {}
        ~Semphore() {}

    private:
        Semphore(const Semphore&) = delete;
        Semphore(Semphore&&) = delete;
        Semphore operator=(const Semphore&) = delete;

    private:
        std::mutex m_mtx;
        std::condition_variable m_cv;
        int m_signals;
        int m_blocked;

    public:
        int wait(const int time_out_ms = (-1))
        {
            std::unique_lock<std::mutex> ul(m_mtx);
            if (m_signals > 0)
            {
                --m_signals;
                return SEMPHORE_SUCCESS;
            }
            else
            {
                ++m_blocked;
            }

            if (time_out_ms >= 0)
            {
                std::chrono::milliseconds wait_time_ms(time_out_ms);
                auto result = m_cv.wait_for(ul, wait_time_ms, [&]{ return m_signals > 0; });
                --m_blocked;
                if (result)
                {
                    --m_signals;
                    return SEMPHORE_SUCCESS;
                }
                else
                {
                    return SEMPHORE_TIME_OUT;
                }
            }
            else
            {
                m_cv.wait(ul, [&]{ return m_signals > 0; });
                --m_blocked;
                --m_signals;
                return SEMPHORE_SUCCESS;
            }
        }

        inline void signal(const int count = 1)
        {
            std::lock_guard<std::mutex> lg(m_mtx);
            m_signals += count;
            if (m_blocked > 0)
            {
                m_cv.notify_one();
            }
        }
    };
}

#endif
