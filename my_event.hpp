/*********************************************************** 
* Date: 2016-07-19 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 事件 
* 
* Brief: 
* 
* Note: 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef __MY_EVENT_HPP_BY_MOUYUN_2016_07_19__
#define __MY_EVENT_HPP_BY_MOUYUN_2016_07_19__

#include <chrono>
#include <mutex>
#include <condition_variable>

namespace my_module_space
{
    enum 
    {
        EVENT_FAIL = (-1),
        EVENT_SUCCESS = 0,
        EVENT_TIME_OUT = 1
    };

    class Event
    {
    public:
        Event(bool init_state = false, bool manual_reset = true) : 
            m_signal(init_state), 
            m_manual_reset(manual_reset), 
            m_blocked(0) 
        {
        }

        ~Event() {}

    private:
        Event(const Event&) = delete;
        Event(Event&&) = delete;
        Event& operator=(const Event&) = delete;

    public:
        int wait(const int time_out_ms = (-1))
        {
            std::unique_lock<std::mutex> ul(m_mtx);

            if (m_signal)
            {
                if (!m_manual_reset)
                {
                    m_signal = false;
                }

                return EVENT_SUCCESS;
            }
            else
            {
                ++m_blocked;
            }

            if (time_out_ms >= 0)
            {
                std::chrono::milliseconds wait_time_ms(time_out_ms);
                auto result = m_cv.wait_for(ul, wait_time_ms, [&]{ return m_signal; });
                --m_blocked;
                if (result)
                {
                    if (!m_manual_reset)
                    {
                        m_signal = false;
                    }
                    return EVENT_SUCCESS;
                }
                else
                {
                    return EVENT_TIME_OUT;
                }
            }
            else
            {
                m_cv.wait(ul, [&]{return m_signal;});
                --m_blocked;
                if (!m_manual_reset)
                {
                    m_signal = false;
                }
                return EVENT_SUCCESS;
            }
        }

        void set()
        {
            std::lock_guard<std::mutex> lg(m_mtx);

            if (m_signal)
            {
                return;
            }

            m_signal = true;

            if (m_blocked > 0)
            {
                if (m_manual_reset)
                {
                    m_cv.notify_all();
                }
                else
                {
                    m_cv.notify_one();
                }
            }
        }

        void reset()
        {
            m_mtx.lock();
            m_signal = false;
            m_mtx.unlock();
        }

    private:
        std::mutex m_mtx;
        std::condition_variable m_cv;
        bool m_signal;
        bool m_manual_reset;
        int m_blocked;
    };
}
#endif
