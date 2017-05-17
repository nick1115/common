/*********************************************************** 
* Date: 2016-07-06 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 通用头文件  
* 
* Brief: 
* 
* Note: 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef __MY_COMMON_HEADER_HPP_BY_MOUYUN_2016_07_01__
#define __MY_COMMON_HEADER_HPP_BY_MOUYUN_2016_07_01__

#include <memory>

namespace my_module_space
{
    /// < 禁止类的拷贝构造与赋值 
    #define DISABLE_CLASS_COPY(CLASS) CLASS(const CLASS&)=delete;CLASS& operator=(const CLASS&)=delete;

    /// < 禁止类的拷贝构造、移动语义与赋值 
    #define DISABLE_CLASS_COPY_AND_MOVE(CLASS) CLASS(const CLASS&)=delete;CLASS(CLASS&&)=delete;CLASS& operator=(const CLASS&)=delete;CLASS& operator=(CLASS&&)=delete;

    /// < 无锁单例模式的声明（写在类中） 
    #define DECLARE_NO_LOCK_SINGLETON(OBJ) private:static std::shared_ptr<OBJ> m_sp_single_instance;\
    public:static OBJ* get_instance();\
    static OBJ* peek_instance();\
    static void destroy_instance();

    /// < 无锁单例模式的实现（写在类外） 
    #define IMPLEMENT_NO_LOCK_SINGLETON(OBJ) std::shared_ptr<OBJ> OBJ::m_sp_single_instance;\
    OBJ* OBJ::get_instance() {if(m_sp_single_instance==nullptr){try{m_sp_single_instance = std::make_shared<OBJ>();}catch (...){}}return m_sp_single_instance.get();}\
    OBJ* OBJ::peek_instance() {return m_sp_single_instance.get();}\
    void OBJ::destroy_instance() {if(m_sp_single_instance!=nullptr){m_sp_single_instance.reset();}}

    /// < 创建对象（屏蔽异常） 
    template<typename T, typename... ARGS>
    inline T* create_obj(ARGS &&... _args)
    {
        T *p_obj = nullptr;
        try {p_obj = new T(std::forward<ARGS>(_args)...);} catch (...){}
        return p_obj;
    }

    /// < 销毁对象 
    template<typename T>
    inline void destroy_obj(T *p_obj)
    {
        delete p_obj;
    }

    /// < 创建数组（屏蔽异常） 
    template<typename T>
    inline T* create_array(unsigned int _size)
    {
        T *p_array = nullptr;
        try{p_array = new T[_size];} catch (...){}
        return p_array;
    }

    /// < 销毁数组 
    template<typename T>
    inline void destroy_array(T *p_array)
    {
        delete [] p_array;
    }

    /// < 创建shared_ptr对象（屏蔽异常） 
    template<typename T, typename... ARGS>
    inline std::shared_ptr<T> create_sp_obj(ARGS &&... _args)
    {
        try{return std::make_shared<T>(std::forward<ARGS>(_args)...);} catch (...){}
        return std::shared_ptr<T>(nullptr);
    }

    /// < 创建shared_ptr数组（屏蔽异常） 
    template<typename T>
    inline std::shared_ptr<T> create_sp_array(unsigned int _size)
    {
        try{return std::shared_ptr<T>(create_array<T>(_size), destroy_array<T>);} catch (...){}
        return std::shared_ptr<T>(nullptr);
    }
}

using namespace my_module_space;

#endif
