/*********************************************************** 
* Date: 2017-02-16 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 控制反转 
* 
* Brief: DI构造函数实现IoC 
* 
* Note: 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef __MY_IOC_HPP_BY_MOUYUN_2017_02_16__
#define __MY_IOC_HPP_BY_MOUYUN_2017_02_16__

#include <string>
#include <map>
#include <memory>
#include <functional>

#include "my_any_type.hpp"

namespace my_module_space
{
    class ioc
    {
    public:
        ioc() {}
        ~ioc() {}

    private:
        ioc(const ioc&) = delete;
        ioc(ioc&&) = delete;
        ioc& operator=(const ioc&) = delete;

    public:
        /// < 注册代理关系 
        template<class DELEGATE, typename T, typename ... ARGS>
        typename std::enable_if<!std::is_base_of<DELEGATE, T>::value, bool>::type register_type(const std::string &name_key)
        {
            std::function<DELEGATE* (ARGS ...)> obj_constructor = [](ARGS ... args) 
            {
                T *p_t = nullptr;
                try {p_t = new T(std::forward<ARGS>(args)...);} catch (...){}
                if (p_t != nullptr)
                {
                    DELEGATE *p_d = nullptr;
                    try {p_d = new DELEGATE(p_t);} catch (...){}
                    if (p_d != nullptr)
                    {
                        return p_d;
                    }
                    else
                    {
                        my_destroy_obj(p_t);
                    }
                }

                return static_cast<T*>(nullptr);
            };

            return register_type(name_key, obj_constructor);
        }

        /// < 注册继承关系 
        template<class BASE, typename DERIVED, typename ... ARGS>
        typename std::enable_if<std::is_base_of<BASE, DERIVED>::value, bool>::type register_type(const std::string &name_key)
        {
            std::function<BASE* (ARGS ...)> obj_constructor = [](ARGS ... args)
            {
                DERIVED *p_d = nullptr;
                try {p_d = new DERIVED(std::forward<ARGS>(args)...);} catch (...){}
                return p_d;
            };

            return register_type(name_key, obj_constructor);
        }

        /// < 注册单一类型 
        template<class T, typename ... ARGS>
        bool register_single(const std::string &name_key)
        {
            std::function<T* (ARGS ...)> obj_constructor = [](ARGS ... args)
            {
                T *p_t = nullptr;
                try {p_t = new T(std::forward<ARGS>(args)...);} catch (...){}
                return p_t;
            };

            return register_type(name_key, obj_constructor);
        }

        /// < 创建普通对象 
        template<class T, typename ... ARGS>
        T* create(const std::string &name_key, ARGS ... args)
        {
            auto c_itr = m_constructor_map.find(name_key);
            if (c_itr == m_constructor_map.end())
            {
                return nullptr;
            }

            /// < 判断类型是否匹配 
            if (!c_itr->second.is<std::function<T*(ARGS ...)> >())
            {
                return nullptr;
            }

            auto obj_constructor = c_itr->second.get<std::function<T*(ARGS ...)> >();
            if (obj_constructor != nullptr)
            {
                return obj_constructor(args ...);
            }
            else
            {
                return nullptr;
            }
        }

        /// < 创建智能指针对象 
        template<class T, typename ... ARGS>
        std::shared_ptr<T> create_sp(const std::string &name_key, ARGS ... args)
        {
            return std::shared_ptr<T>(create<T>(name_key, args ...));
        }

    private:
        bool register_type(const std::string &name_key, any_type _constructor)
        {
            if (m_constructor_map.find(name_key) != m_constructor_map.end())
            {
                return false;
            }

            m_constructor_map.emplace(name_key, _constructor);

            return true;
        }

    private:
        std::map<std::string, any_type> m_constructor_map;
    };
}


#endif
