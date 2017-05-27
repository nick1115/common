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

/*********************************************序********************************************************* 

丛远古时代到现在，C/C++的内存泄漏问题干翻了一大票C/C++程序猿，而今C++11的智能指针彻底终结了这种疯狂的杀戮。  
尽管掌握智能指针的用法并不难，可他们中还是有一大批人仍在犯2裸奔。  

这个文件实现了ObjectPool<T>，这个设施极大简化了std::shared_ptr<T>的使用以及对象的复用，下面将展示如何使用， 
简单得不要不要的...... 

现在我们假设有一个类A（一般应该是纯数据类），在某个函数中我们需要用A的对象实现业务逻辑，那么应该这样使用： 

#include "my_object_pool.hpp" //在函数所在的源文件中包含这个头文件 

using namespace my_module_space; //使用空间 

... function(...)
{
    ...// 其他代码 

    auto a = ObjectPool<A>::pop_sp(); //直接使用，不需要任何显式的声明或定义，6的飞起！ 

    //像上面这样使用了pop_sp()来生成对象,那么a的类型是std::shared_ptr<A>， 
    //如果不是使用pop_sp()而是使用pop()，那么a的类型是A*,而且这时的对象在用完后需要调用push()还回去 
    //强烈建议使用pop_sp()而不要使用pop() 
    
    if (a == nullptr) //强烈建议检查指针有效性，这样可以避免内存不足时的程序崩溃问题 
    {
        //处理内存错误 
    }
    else
    {
        //现在用a来实现业务逻辑吧，根本不用管它什么时间以及怎样释放，爱咋咋地......^_^

        //如果你想把a传递到其他函数，那么最好还是要清楚std::shared_ptr<T>的特性 
    }

    ...//其他代码 
}

如果在此函数的其他地方或任何其他函数（不限源码文件）中再次使用了ObjectPool<A>::pop_sp()， 
那么这里还提供了一个非常牛B的特性，那就是所有的这些使用仅会产生一个A的对象池，大家共享之。 

最后，强烈建议在类A中实现一个清理所有成员数据的函数，格式限定为 void clear()，举个栗子： 

class A
{
    ...//其他代码 
public://最好是public 
    void clear()
    {
        data1 = 0; //data1的类型应该是整型 
        memset(p, 0, size); //p应该指向一个大小为size字节的缓存 
        data2.clear(); //data2的类型应该是std::vector, std::string或者自定义类型，只要它有void clear()方法 
        ...//其他清理成员数据的代码 
    }
}

当然，即使A不提供这个void clear()方法也可以照常使用ObjectPool<A>，可以飞上天的模板元编程在编译期会自动 
判定这个方法是否存在，存在则调用，不存在则忽略。这个方法主要是为了保证复用对象时的安全性（清除垃圾数据）。 


****************************my english is poor, so it's only a rough translation ^_^ **********************************

For a long time, even now, memory leak in C/C++ code is fucking most of the C/C++ programers, 
however, smart pointers in C++11 have ended this! But many of them don't know the usage, and still
programming with the raw pointer. Although, to master their usage only cost a few hours.

This file has implemented the ObjectPool<T>. This facility makes it eaier to use the std::shared_ptr<T> and object reuse, 
now, I'll explain how to used them, it's easy use is unbelievable ^_^

Now, we assume there is a class, it's name is A. Then, in your functions, usage is like this:

#include "my_object_pool.hpp"

using namespace my_module_space;

... function(...)
{
    ...// other code

    auto a = ObjectPool<A>::pop_sp(); //directly use it, not any requirements for explicit declare or define

    //the type of a is std::shared_ptr<A>, 
    //if use pop() instead of pop_sp() then a's type is A*
    
    if (a == nullptr) //nullptr check is recommended
    {
        //handle memory error
    }
    else
    {
        //now, in this function you have a object of A on the heap, you can program with it without
        //any care of when and how to delete it

        //if you want to deliver this object to any other functions, you should be clear about std::shared_ptr<T>
    }

    ...//other code
}

This function is in one of the source files, if we use ObjectrPool<A>::pop_sp() in other functions of this 
source file or any others, these functions will share only one object pool of A, and this is we want to get.

At last, it's recommended to implement a funciton in class A to clear all it's data members, like this:

class A
{
    ...//other code

public: //public is recommended
    void clear()
    {
        data1 = 0; //data1's type is integer
        memset(p, 0, size) //p is a buffer with size bytes
        data2.clear(); //data2 maybe std::vector, std::string...even a custom type who has a void clear()
        ... //other clear of data members
    }
}

Whatever, even there has no clear function in A, ObjectPool<A> is also available, the clear function 
just makes the cache mechanism more safer(clean garbage data in object).

***************************************HETE WE GO**********************************************************************/
#ifndef	__MY_OBJECT_POOL_HPP_BY_MOUYUN_2014_10_27__
#define	__MY_OBJECT_POOL_HPP_BY_MOUYUN_2014_10_27__

#include <type_traits>
#include <list>
#include <mutex>
#include <memory>

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

    template<typename T>
    class ObjectPool
    {
    private:
        class ObjectPoolImpl
        {
        public:
            ObjectPoolImpl() : m_max_holding_count(8) {} /// < default max holding count is 8

            ~ObjectPoolImpl()
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
            ObjectPoolImpl(const ObjectPoolImpl&) = delete;
            ObjectPoolImpl(ObjectPoolImpl&&) = delete;
            ObjectPoolImpl& operator=(const ObjectPoolImpl&) = delete;

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
        static ObjectPoolImpl m_pool;
    };

    template<typename T> typename ObjectPool<T>::ObjectPoolImpl ObjectPool<T>::m_pool; //this is wonderful 
}


#endif
