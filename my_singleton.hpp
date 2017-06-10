/*********************************************************** 
* Date: 2016-06-23 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 单例 
* 
* Brief: 
* 
* Note: 无锁 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef __MY_SINGLETON_HPP_BY_MOUYUN_2017_05_31__
#define __MY_SINGLETON_HPP_BY_MOUYUN_2017_05_31__

namespace my_module_space
{
    template<typename T>
    class LockFreeSingleton
    {
    public:
        inline static T* get_instance()
        {
            if (m_p_T == nullptr)
            {
                m_p_T = new T;
            }
            return m_p_T;
        }

        inline static T* peek_instance()
        {
            return m_p_T;
        }

        inline static void destroy_instance()
        {
            if (m_p_T != nullptr)
            {
                delete m_p_T;
                m_p_T = nullptr;
            }
        }
    private:
        static T *m_p_T;
    };

    template<typename T> T* LockFreeSingleton<T>::m_p_T = nullptr;
}

#endif
