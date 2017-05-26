/*********************************************************** 
* Date: 2016-07-08 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 对象池 
* 
* Brief: 
* 
* Note: 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef	__MY_OBJECT_POOL_HPP_BY_MOUYUN_2014_10_27__
#define	__MY_OBJECT_POOL_HPP_BY_MOUYUN_2014_10_27__

#include <type_traits>
#include <list>
#include <mutex>

namespace my_module_space
{
    template<typename T>
    class ClearFunctionSensor
    {
    public:
        template<typename U, void (U::*)()> class Matcher;
        template<typename U> static char helper(Matcher<U, &U::clear>*);
        template<typename U> static int helper(...);
        enum { value = sizeof(helper<T>(nullptr)) == sizeof(char) };
    };

    template <bool> class ClearImpl;
    template<> class ClearImpl<true>
    {
    public:
        template<typename T> inline static void clear(T *p_t) {p_t->clear();}
    };
    template<> class ClearImpl<false>
    {
    public:
        template<typename T> inline static void clear(T *p_t) {}
    };

    template<typename T> inline void clear_object(T *p_t)
    {
        ClearImpl<ClearFunctionSensor<T>::value>::clear(p_t);
    }

    template <typename T>
    class ObjectPool
    {
    public:
        ObjectPool() : m_max_holding_count(8) {} /// < default max holding count is 8

        ~ObjectPool()
        {
            m_lock.lock();
            typename std::list<T*>::iterator itr = m_obj_list.begin();
            while (itr != m_obj_list.end())
            {
                delete *itr;
                ++itr;
            }
            m_obj_list.clear();
            m_lock.unlock();
        }

    private:
        ObjectPool(const ObjectPool&) = delete;
        ObjectPool(ObjectPool&&) = delete;
        ObjectPool& operator=(const ObjectPool&) = delete;

    private:
        std::list<T*> m_obj_list;
        std::mutex m_lock;
        int m_max_holding_count; /// < when push(), delete or hold, determined by this var

    public:
        T* pop()
        {
            T *p_obj = nullptr;

            std::lock_guard<std::mutex> lg(m_lock);

            if (m_obj_list.empty())
            {
                try {p_obj = new T();} catch(...){} /// < pool is empty, new object
            }
            else
            {
                p_obj = m_obj_list.front();
                m_obj_list.pop_front();
            }

            return p_obj;
        }

        void push(T *p_obj)
        {
            if (p_obj == nullptr)
            {
                return;
            }

            std::lock_guard<std::mutex> lg(m_lock);

            if (static_cast<int>(m_obj_list.size()) < m_max_holding_count)
            {
                clear_object(p_obj); /// < using the fucking shit template meta programming
                m_obj_list.push_back(p_obj);
            }
            else
            {
                delete p_obj; /// < pool is full, delete object
            }
        }

        inline int get_max_holding_count() const
        {
            return m_max_holding_count;
        }

        void set_max_holding_count(const int count)
        {
            std::lock_guard<std::mutex> lg(m_lock);

            m_max_holding_count = count;

            int current_size = static_cast<int>(m_obj_list.size()); 
            while (current_size-- > m_max_holding_count)
            {
                delete m_obj_list.front();
                m_obj_list.pop_front();
            }
        }
    };

    template<typename T>
    class ObjectCreator
    {
    public:
        static inline T* pop()
        {
            return m_pool.pop();
        }

        static inline void push(T *p)
        {
            m_pool.push(p);
        }

        static inline std::shared_ptr<T> pop_sp()
        {
            return std::shared_ptr<T>(pop(), push);
        }

        static inline int get_max_holding_count()
        { 
            return m_pool.get_max_holding_count();
        }
        static inline void set_max_holding_count(const int count)
        {
            m_pool.set_max_holding_count(count);
        }

    private:
        static ObjectPool<T> m_pool;
    };

    template<typename T> ObjectPool<T> ObjectCreator<T>::m_pool; //this is wonderful 
}

/*********************************************MESSAGE*********************************************
For a long time, even now, memory leak in C/C++ code is fucking most of the C/C++ programers, 
however, smart pointers in C++11 have ended this! But many of them don't know the usage, and still
programming with the raw pointer. In fact, to master their usage only cost a few hours.

In this hpp file, I have implemented the ObjectPool<T> and the ObjectCreator<T>. These two facilities
make it eaier to use the std::shared_ptr<T> and provide a few profit of Memory Pool. Now, I'll explain 
how to used them, it's easy use is unbelievable ^_^

Now, we assume there is a class, it's name is A. Then, in your functions, usage is like this:

#include "my_object_pool.h"

... function(...)
{
    ...// other code

    auto a = ObjectCreator<A>::pop_sp(); //directly use, not any explicit declare or define is required

    //the type a is std::shared_ptr<A>, 
    //if use pop() instead of pop_sp() then a's type is A*
    
    if (sp_a == nullptr) //nullptr check is recommended
    {
        //handle memory error
    }
    else
    {
        //now, in this function you have a object of A on the heap, you can program with it without
        //any care of when and how to delete it

        //if you want to deliver this object to other modules, you should be clear about std::shared_ptr<T>
    }

    ...//other code
}

At last, it's recommended to implement a funciton in class A to clear all it's data members, like this:

class A
{
    ...//other code

public:
    void clear()
    {
        data1 = 0; //data1's type is integer
        data2.clear(); //data2 maybe std::vector, std::string...even a custom type who has a void clear()
        ... //other clear of data members
    }
}

Whatever, even there has no clear function in A, ObjectCreator<A> is also available, the clear function 
just makes the cache mechanism in ObjectPool<T> more safer.

***************************************************************************************************/

#endif
