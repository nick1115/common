/*********************************************************** 
* Date: 2017-02-10 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 类型擦除类 
* 
* Brief: 可以存放任何类型 
* 
* Note: 因为没有处理new的异常，所以使用时可能会抛出异常 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef __MY_ANY_TYPE_HPP_BY_MOUYUN_2017_02_10__
#define __MY_ANY_TYPE_HPP_BY_MOUYUN_2017_02_10__

#include <typeindex>
#include <memory>

namespace my_module_space
{
    class AnyType
    {
    private:
        class Holder
        {
        public:
            virtual ~Holder() {} /// < must be virtual 
            virtual std::unique_ptr<Holder> clone() const = 0;
            virtual void* get_value_ptr() = 0;
        };

        template<typename T>
        class HolderImpl : public Holder
        {
        public:
            template<typename U>
            HolderImpl(U && _value) : m_value(std::forward<U>(_value)) {}

            virtual std::unique_ptr<Holder> clone() const
            {
                return std::unique_ptr<Holder>(new HolderImpl<T>(m_value));
            }

            virtual void* get_value_ptr()
            {
                return &m_value;
            }

            T m_value;
        };

    public:
        AnyType() : m_type_index(std::type_index(typeid(void))) {}
        AnyType(const AnyType &other) : m_up_holder(other.clone()), m_type_index(other.m_type_index) {}
        AnyType(AnyType &&other) : m_up_holder(std::move(other.m_up_holder)), m_type_index(other.m_type_index) {}

        template<typename U, class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, AnyType>::value, U>::type>
        AnyType(U &&value) :
            m_up_holder(new HolderImpl<typename std::decay<U>::type>(std::forward<U>(value))),
            m_type_index(std::type_index(typeid(typename std::decay<U>::type)))
        {
        }

        AnyType& operator=(const AnyType &rht)
        {
            if (m_up_holder != rht.m_up_holder)
            {
                m_up_holder = rht.clone();
                m_type_index = rht.m_type_index;
            }
            return *this;
        }

    public:
        inline bool empty() const
        {
            return m_up_holder == nullptr;
        }

        template<typename U>
        inline bool is() const
        {
            return m_type_index == std::type_index(typeid(U));
        }

        template<typename U>
        U& get() const
        {
            return *(static_cast<U*>(m_up_holder->get_value_ptr()));
        }

    private:
        std::unique_ptr<Holder> clone() const
        {
            if (m_up_holder != nullptr)
            {
                return m_up_holder->clone();
            }
            else
            {
                return std::unique_ptr<Holder>(nullptr);
            }
        }

    private:
        std::unique_ptr<Holder> m_up_holder;
        std::type_index m_type_index;
    };
}

/* demo 
* 
* using namespace my_module_space; 
* 
* AnyType at1(8); /// < int 
* AnyType at2(1.23);  /// < double 
* AnyType at3("hello"); /// < const char* 
* AnyType at4(std::string("hello")); /// < std::string 
* 
* AnyType at5 = 128; 
* AnyType at6 = 3.14; 
* AnyType at7 = "hello"; 
* AnyType at8 = std::string("hello"); 
*/ 

#endif
