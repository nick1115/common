/*********************************************************** 
* Date: 2017-05-16 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 流水线单元 
* 
* Brief: 
* 
* Note: 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef __MY_PROCESSOR_HPP_BY_MOUYUN_2017_05_16__
#define __MY_PROCESSOR_HPP_BY_MOUYUN_2017_05_16__

#include <list>
#include <memory>
#include <thread>
#include <chrono>
#include <functional>

#include "my_semphore.hpp"

namespace my_module_space
{
    #define thread_sleep_s(s) std::this_thread::sleep_for(std::chrono::seconds(s))
    #define thread_sleep_ms(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
    #define thread_sleep_us(us) std::this_thread::sleep_for(std::chrono::microseconds(us))
    #define thread_sleep_ns(ns) std::this_thread::sleep_for(std::chrono::nanoseconds(ns))

    class ThreadWrapper
    {
        template<typename T> friend class Processor;
    public:
        ThreadWrapper() : m_sp_thread(nullptr), m_init_flag(0), m_quit_flag(0) {}
        ~ThreadWrapper()
        {
            /// < 线程信息类析构前，用户应停止线程，这里的代码是为异常情况准备的 
            if (m_sp_thread != nullptr && m_sp_thread->joinable())
            {
                set_quit_flag();
                thread_sleep_ms(0);
                m_sp_thread->detach(); /// < 不要使用 join() 
                m_sp_thread.reset();
            }
        }

    private:
        ThreadWrapper(const ThreadWrapper&) = delete;
        ThreadWrapper& operator=(const ThreadWrapper&) = delete;
    
    private:
        std::shared_ptr<std::thread> m_sp_thread; /// < C++11线程类 
        volatile int m_init_flag; /// < 在创建者与线程之间同步线程信息 
        volatile int m_quit_flag; /// < 线程退出标志 

    public:
        inline void set_quit_flag()
        {
            m_quit_flag = 1;
        }
        inline void reset_quit_flag()
        {
            m_quit_flag = 0;
        }
        inline bool is_thread_quit() const
        {
            return m_quit_flag != 0;
        }
    };

    using SP_THREAD_WRAPPER = std::shared_ptr<ThreadWrapper>;

    enum
    {
        PROCESSOR_FAIL = -1,
        PROCESSOR_SUCCESS = 0,
        PROCESSOR_TIME_OUT = 1,
        PROCESSOR_QUEUE_FULL = 2,
        PROCESSOR_QUEUE_EMPTY = 3
    };

    template<typename T>
    class Processor
    {
        using TASK_FUNCTION = std::function<void(std::shared_ptr<T>&)>;
		using TIMEOUT_FUNCTION = std::function<void()>;

    public:
        Processor(TASK_FUNCTION task_f = nullptr, 
				TIMEOUT_FUNCTION timeout_f = nullptr, 
				const int thread_time_out_ms = 200, 
				const int task_max_count = 1024, 
				const int thread_max_count = 1024) : 
            m_task_function(task_f), 
			m_timeout_function(timeout_f), 
            m_task_max_count(task_max_count),
            m_thread_max_count(thread_max_count)
        {
        }

        ~Processor() {end_all_threads();}

    private:
        Processor(const Processor&) = delete;
        Processor& operator=(const Processor&) = delete;

    private:
        std::list<std::shared_ptr<T> > m_task_list;
        std::mutex m_task_lock;
        Semphore m_task_semphore;
        volatile int m_task_max_count = 1024;

        std::list<SP_THREAD_WRAPPER> m_thread_list;
        std::mutex m_thread_lock;
        volatile int m_thread_max_count = 1024;
		volatile int m_thread_timeout_ms = 200;

        TASK_FUNCTION m_task_function;
		TIMEOUT_FUNCTION m_timeout_function;

        std::list<Processor<T>*> m_next_processors;

    private:
        void remove_thread_wrapper(const SP_THREAD_WRAPPER &sp_thread_wrapper)
        {
            std::lock_guard<std::mutex> auto_lock(m_thread_lock);

            for (auto itr = m_thread_list.begin(); itr != m_thread_list.end(); ++itr)
            {
                if ((*itr) == sp_thread_wrapper)
                {
                    m_thread_list.erase(itr);
                    return;
                }
            }
        }

        void end_one_thread(SP_THREAD_WRAPPER &sp_target, bool sync = true)
        {
            if (sp_target != nullptr && sp_target->m_sp_thread != nullptr && sp_target->m_sp_thread->joinable())
            {
                sp_target->set_quit_flag();
                thread_sleep_ms(0);

                if (sync)
                {
                    sp_target->m_sp_thread->join();
                }
                else
                {
                    sp_target->m_sp_thread->detach();
                }

                sp_target->m_sp_thread.reset();
            }
        }

    public:
		inline void set_thread_timeout(const int timeout_ms)
		{
			m_thread_timeout_ms = timeout_ms;
		}

		inline void set_queue_max_count(const int max_count)
		{
			m_task_max_count = max_count;
		}
		inline int get_queue_max_count() const
		{
			return m_task_max_count;
		}

        inline void set_task_function(TASK_FUNCTION f)
        {
            m_task_function = f;
        }
		inline void set_timeout_function(TIMEOUT_FUNCTION f)
		{
			m_timeout_function = f;
		}

        int add_next_processor(Processor<T> *p_processor) /// < current, it's not thread safe 
        {
            if (p_processor == nullptr)
            {
                return PROCESSOR_FAIL;
            }

            /// <  keep unique 
            for (auto &cur_processor : m_next_processors)
            {
                if (cur_processor == p_processor)
                {
                    return PROCESSOR_FAIL;
                }
            }

            m_next_processors.push_back(p_processor);

            return PROCESSOR_SUCCESS;
        }
        int remove_next_processor(Processor<T> *p_processor) /// < current, it's not thread safe 
        {
            for (auto itr = m_next_processors.begin(); itr != m_next_processors.end(); ++itr)
            {
                if (*itr == p_processor)
                {
                    m_next_processors.erase(itr);
                    return PROCESSOR_SUCCESS;
                }
            }

            return PROCESSOR_FAIL;
        }

    public:
        int begin_thread(const int count = 1)
        {
            if (count <= 0)
            {
                return 0;
            }

            std::lock_guard<std::mutex> auto_lock(m_thread_lock);

            int cur_count = static_cast<int>(m_thread_list.size());
            int create_count = 0;

            for (; create_count < count;)
            {
                if (cur_count >= m_thread_max_count)
                {
                    break;
                }

                try
                {
                    SP_THREAD_WRAPPER sp_thread_wrapper(std::make_shared<ThreadWrapper>());
                    sp_thread_wrapper->m_sp_thread = std::make_shared<std::thread>([this](SP_THREAD_WRAPPER sp_thread_wrapper)->void
                    {
                        if (sp_thread_wrapper == nullptr)
                        {
                            return;
                        }

                        /// < wait for thread object complete 
                        while (sp_thread_wrapper->m_init_flag == 0)
                        {
                            std::this_thread::yield();
                        }

                        std::shared_ptr<T> sp_task(nullptr);
                        int result = 0;

                        while (true)
                        {
                            result = get_task(sp_task, this->m_thread_timeout_ms);
                            if (sp_thread_wrapper->is_thread_quit())
                            {
                                break;
                            }

                            if (result == PROCESSOR_SUCCESS)
                            {
								if (m_task_function != nullptr)
								{
									m_task_function(sp_task);
								}

                                /// < push task to next processors 
                                for (auto &cur_processor : this->m_next_processors)
                                {
                                    cur_processor->put_task(sp_task);
                                }
                            }
							else if (result == PROCESSOR_TIME_OUT)
							{
								if (m_timeout_function != nullptr)
								{
									m_timeout_function();
								}
							}
                            else
                            {
                                /// < queue empty, ignore it 
                            }
                        }

                        if (!sp_thread_wrapper->is_thread_quit())
                        {
                            /// < thread not notified to quit, clean thread wrapper 
                            if (sp_thread_wrapper->m_sp_thread != nullptr)
                            {
                                sp_thread_wrapper->m_sp_thread->detach();
                            }

                            this->remove_thread_wrapper(sp_thread_wrapper);
                        }
                    }, sp_thread_wrapper);

                    sp_thread_wrapper->m_init_flag = 1024; /// < notify thread, yes, we love 1024 ^_^  
                    thread_sleep_ms(0);

                    m_thread_list.emplace_back(sp_thread_wrapper);

                    ++cur_count;
                    ++create_count;
                }
                catch (...)
                {
                    return create_count;
                }
            }

            return create_count;
        }

        int end_thread(const int count = 1, bool sync = true)
        {
            if (count <= 0)
            {
                return 0;
            }

            int tar_end_count = count;
            int real_ended_count = 0;

            while (tar_end_count-- > 0)
            {
                m_thread_lock.lock();

                if (m_thread_list.empty())
                {
                    m_thread_lock.unlock();
                    break;
                }

                auto sp_target(std::move(m_thread_list.front()));
                m_thread_list.pop_front();

                m_thread_lock.unlock();

                end_one_thread(sp_target, sync);

                ++real_ended_count;
            }

            return real_ended_count;
        }

        void end_all_threads(bool sync = true)
        {
            std::unique_lock<std::mutex> auto_lock(m_thread_lock);

            if (m_thread_list.empty())
            {
                return;
            }

            auto temp_thread_list(std::move(m_thread_list)); /// < 取出所有线程 

            auto_lock.unlock(); /// < 手动解锁，防止join()时死锁 

            /// < 停止这些线程 
            for (auto & sp_target : temp_thread_list)
            {
                end_one_thread(sp_target, sync);
            }
        }

        int put_task(std::shared_ptr<T> &sp_task, int *p_new_size = nullptr)
        {
            std::lock_guard<std::mutex> auto_lock(m_task_lock);

            int cur_size = static_cast<int>(m_task_list.size());
            if (cur_size >= m_task_max_count)
            {
                if (p_new_size != nullptr)
                {
                    *p_new_size = cur_size;
                }
                return PROCESSOR_QUEUE_FULL;
            }

            m_task_list.emplace_back(sp_task);
            m_task_semphore.signal();

            if (p_new_size != nullptr)
            {
                *p_new_size = cur_size + 1;
            }

            return PROCESSOR_SUCCESS;
        }

        int get_task(std::shared_ptr<T> &sp_task, const int ms = (-1), int *p_new_size = nullptr)
        {
            if (m_task_semphore.wait(ms) != SEMPHORE_SUCCESS)
            {
                return PROCESSOR_TIME_OUT;
            }

            std::lock_guard<std::mutex> auto_lock(m_task_lock);

            /*
            * 元素入队时信号与元素是1对1关系，由于可以一次取全部队列元素，
            * 所以可能存在等待信号成功但队列中已无元素的情况，故在此判断
            */
            if (m_task_list.empty())
            {
                if (p_new_size != nullptr)
                {
                    *p_new_size = 0;
                }
                return PROCESSOR_QUEUE_EMPTY;
            }

            sp_task = std::move(m_task_list.front());
            m_task_list.pop_front();

            if (p_new_size != nullptr)
            {
                *p_new_size = static_cast<int>(m_task_list.size());
            }

            return PROCESSOR_SUCCESS;
        }


    };
}


#endif
