/*********************************************************** 
* Date: 2016-05-04 
* 
* Author: 牟韵 
* 
* Email: mouyun1115@163.com 
* 
* Module: 缓存类 
* 
* Brief: 字节数据管理已起飞，简直不要太好用^_^ 
* 
* Note: 
* 
* CodePage: Pure UTF-8 
************************************************************/ 
#ifndef	__MY_BUFFER_HPP_BY_MOUYUN_2014_11_15__
#define	__MY_BUFFER_HPP_BY_MOUYUN_2014_11_15__

#include <string.h>

namespace my_module_space
{
    /// < 缓存类 
    class Buffer
    {
    public:
        /** 
        * 默认构造函数 
        */ 
        Buffer() : m_p_buffer(nullptr), m_buffer_size(0), m_data_size(0) {}

        /** 
        * 自定义构造函数 
        * 
        * 参数： buffer_size [in] 缓存初始化大小 
        */ 
        explicit Buffer(const int buffer_size) : m_p_buffer(nullptr), m_buffer_size(0), m_data_size(0)
        {
            if (buffer_size > 0)
            {
                try {m_p_buffer = new char[buffer_size];}catch(...){}
                if (m_p_buffer != nullptr)
                {
                    m_buffer_size = buffer_size;
                }
            }
        }

        /** 
        * 自定义构造函数 
        * 
        * 参数： p_str [in] C字符串指针 
        */ 
        explicit Buffer(const char *p_str) : m_p_buffer(nullptr), m_buffer_size(0), m_data_size(0)
        {
            if (p_str != nullptr)
            {
                int str_size = static_cast<int>(strlen(p_str) + 1);
                try {m_p_buffer = new char[str_size];}catch(...){}
                if (m_p_buffer != nullptr)
                {
                    memcpy(m_p_buffer, p_str, str_size);
                    m_buffer_size = str_size;
                    m_data_size = str_size;
                }
            }
        }

        /** 
        * 自定义构造函数 
        * 
        * 参数： 
        *       p_data [in] 外部数据指针 
        *       data_size [in] 外部数据大小 
        */ 
        Buffer(const void *p_data, const int data_size) : m_p_buffer(nullptr), m_buffer_size(0), m_data_size(0)
        {
            if (p_data != nullptr && data_size > 0)
            {
                try {m_p_buffer = new char[data_size];}catch(...){}
                if (m_p_buffer != nullptr)
                {
                    memcpy(m_p_buffer, p_data, data_size);
                    m_buffer_size = data_size;
                    m_data_size = data_size;
                }
            }
        }

        /** 
        * 拷贝构造函数 
        * 
        * 参数： copy_obj [in] 拷贝对象 
        * 
        * 备注： 深拷贝，保持当前对象的缓存大小和数据大小与copy_obj的完全一致 
        */ 
        explicit Buffer(const Buffer &copy_obj) : m_p_buffer(nullptr), m_buffer_size(0), m_data_size(0)
        {
            if (copy_obj.m_p_buffer != nullptr)
            {
                try {m_p_buffer = new char[copy_obj.m_buffer_size];}catch(...){}
                if (m_p_buffer != nullptr)
                {
                    memcpy(m_p_buffer, copy_obj.m_p_buffer, copy_obj.m_buffer_size);
                    m_buffer_size = copy_obj.m_buffer_size;
                    m_data_size = copy_obj.m_data_size;
                }
            }
        }

        /** 
        * 移动构造函数 
        * 
        * 参数： move_obj [in] 移动的对象 
        * 
        * 备注： 移动取值对象的所有资源 
        */ 
        Buffer(Buffer &&move_obj) : m_p_buffer(move_obj.m_p_buffer), m_buffer_size(move_obj.m_buffer_size), m_data_size(move_obj.m_data_size)
        {
            move_obj.m_p_buffer = nullptr;
            move_obj.m_buffer_size = 0;
            move_obj.m_data_size = 0;
        }

        /** 
        * 析构函数 
        */ 
        ~Buffer()
        {
            destroy();
        }

        /** 
        * 赋值 
        * 
        * 参数： right_obj [in] 复制的对象 
        * 
        * 返回值： 当前对象的引用 
        * 
        * 备注： 深拷贝，保持当前对象的缓存大小和数据大小与right_obj的完全一致 
        */ 
        Buffer& operator=(const Buffer &right_obj)
        {
            if (&right_obj == this)
            {
                return *this;
            }

            if (right_obj.m_p_buffer == nullptr)
            {
                destroy();
                return *this;
            }

            if (m_buffer_size != right_obj.m_buffer_size)
            {
                destroy();
            }

            if (m_p_buffer == nullptr)
            {
                try {m_p_buffer = new char[right_obj.m_buffer_size];}catch(...){}
            }

            if (m_p_buffer != nullptr)
            {
                memcpy(m_p_buffer, right_obj.m_p_buffer, right_obj.m_buffer_size);
                m_buffer_size = right_obj.m_buffer_size;
                m_data_size = right_obj.m_data_size;
            }

            return *this;
        }

        /** 
        * 移动赋值 
        * 
        * 参数： right_obj [in] 取值对象 
        * 
        * 返回值： 当前对象的引用 
        * 
        * 备注： 移动取值对象的所有资源 
        */ 
        Buffer& operator=(Buffer &&right_obj)
        {
            if (&right_obj != this)
            {
                if (m_p_buffer != nullptr)
                {
                    delete [] m_p_buffer;
                }

                m_p_buffer = right_obj.m_p_buffer;
                m_buffer_size = right_obj.m_buffer_size;
                m_data_size = right_obj.m_data_size;

                right_obj.m_p_buffer = nullptr;
                right_obj.m_buffer_size = 0;
                right_obj.m_data_size = 0;
            }

            return *this;
        }

        /** 
        * 重载 
        * 
        * 参数： p_data [in] C字符串指针 
        * 
        * 备注： 
        *     1. 若缓存能容纳字符串，则直接复制而不改变缓存大小，若不能则扩容复制 
        *     2. 除非非常了解源码，否则不建议使用此赋值函数 
        */ 
        Buffer& operator=(const char *p_str)
        {
            if (p_str != nullptr)
            {
                int str_size = static_cast<int>(strlen(p_str) + 1);
                if (str_size <= m_buffer_size)
                {
                    memcpy(m_p_buffer, p_str, str_size);
                    m_data_size = str_size;
                }
                else
                {
                    if (resize(str_size))
                    {
                        memcpy(m_p_buffer, p_str, str_size);
                        m_data_size = str_size;
                    }
                }
            }
            else
            {
                destroy();
            }

            return *this;
        }

    private:
        char *m_p_buffer; /// < 缓存指针 
        int m_buffer_size;	/// < 缓存容量 
        int m_data_size; /// < 数据大小 

    public:
        /** 
        * 获取缓存指针 
        * 
        * 参数： 空 
        * 
        * 返回值： void*格式的缓存指针 
        * 
        * 备注： 
        *     1. 写入数据时，若有需要，须通过set_data_size()接口设置数据大小 
        *     2. 外部需自己防止越界问题 
        */ 
        inline void* buf() const
        {
            return m_p_buffer;
        }

        /** 
        * 获取缓存指针 
        * 
        * 参数： 空 
        * 
        * 返回值： char*格式的缓存指针 
        * 
        * 备注： 
        *     1. char*格式的缓存指针可以对缓存进行读写，兼容C形式的接口 
        *     2. 写入数据时，若有需要，须通过set_data_size()接口设置数据大小 
        *     3. 外部需自己防止越界问题 
        */ 
        inline char* buf_char() const
        {
            return m_p_buffer;
        }

        /** 
        * 获取缓存指针 
        * 
        * 参数： 空 
        * 
        * 返回值： unsigned char*格式的缓存指针 
        * 
        * 备注： 
        *     1. unsigned char*格式的缓存指针可以对缓存进行读写，兼容C形式的接口 
        *     2. 写入数据时，若有需要，须通过set_data_size()接口设置数据大小 
        *     3. 外部需自己防止越界问题 
        */ 
        inline unsigned char* buf_uchar() const
        {
            return reinterpret_cast<unsigned char*>(m_p_buffer);
        }

        /** 
        * 检查是否存在数据 
        * 
        * 参数： 空 
        * 
        * 备注： 仿照STL的容器的判空接口命名 
        */ 
        inline bool empty() const
        {
            return m_data_size <= 0;
        }

        /** 
        * 获取缓存容量 
        * 
        * 参数： 空 
        * 
        * 返回值： 缓存容量 
        */ 
        inline int buffer_size() const
        {
            return m_buffer_size;
        }

        /** 
        * 获取数据大小 
        * 
        * 参数： 
        * 
        * 返回值： 数据大小 
        */ 
        inline int data_size() const
        {
            return m_data_size;
        }

        /** 
        * 设置数据大小 
        * 
        * 参数： new_size [in] 新的数据大小 
        * 
        * 返回值： 空 
        * 
        * 备注： 
        *     1. 当通过指针直接写入数据时，一般应设置数据大小，若使用copy()、push()、insert()等接口写入数据时，这些接口内部会设置数据大小，无需再调用此接口 
        *     2. 当new_size传递了负值或大于缓存容量的值时，不会进行设置 
        */ 
        void data_size(const int new_size)
        {
            if (new_size >= 0 && new_size <= m_buffer_size)
            {
                m_data_size = new_size;
            }
        }

        /** 
        * 调整缓存 
        * 
        * 参数： new_size [in] 新的缓存大小 
        *        retain_former_data [in] 是否保留原来缓存中数据 
        * 
        * 返回值： true:调整成功，false：调整失败 
        * 
        * 备注： 
        *     1. new_size为正值时，不论原缓存容量为多少，都会调整缓存至指定大小 
        *     2. new_size为非正值时，销毁缓存 
        *     3. 当retain_former_data为真，如果new_size小于原来数据的大小，则只截取new_size那部分数据进行保存 
        */ 
        bool resize(const int new_size, bool retain_former_data = false)
        {
            if (new_size <= 0)
            {
                destroy();
                return true;
            }

            char *p_new_buffer = nullptr;
            try {p_new_buffer = new char[new_size];}catch(...){}
            if (p_new_buffer == nullptr)
            {
                return false;
            }

            int new_data_size = 0;
            if (m_p_buffer != nullptr)
            {
                if (retain_former_data && m_data_size > 0)
                {
                    if (new_size > m_data_size)
                    {
                        memcpy(p_new_buffer, m_p_buffer, m_data_size);
                        new_data_size = m_data_size;
                    }
                    else
                    {
                        memcpy(p_new_buffer, m_p_buffer, new_size);
                        new_data_size = new_size;
                    }
                }

                destroy();
            }

            m_p_buffer = p_new_buffer;
            m_buffer_size = new_size;
            m_data_size = new_data_size;

            return true;
        }

        /** 
        * 确保缓存大小 
        * 
        * 参数： 
        *     size [in] 确保的缓存容量 
        *     retain_former_data [in] 是否保留原来缓存中数据 
        * 
        * 返回值： true：缓存容量正常，false:缓存容量不足 
        * 
        * 备注： 
        *     1. 若缓存容量小于size，则接口内部通过调用resize()调整缓存 
        *     2. 若缓存容量不小于size，则直接返回真 
        */ 
        bool ensure(const int size, bool retain_former_data = false)
        {
            if (m_buffer_size < size)
            {
                return resize(size, retain_former_data);
            }
            return true;
        }

        /**清空缓存 
        * 
        * 简介： 将缓存所有字节的值设为0 
        * 参数： 
        * 返回值： 
        * 
        * 备注： 仿照STL的容器的清空接口命名 
        */ 
        void clear()
        {
            if (m_p_buffer != nullptr)
            {
                memset(m_p_buffer, 0, m_buffer_size);
                m_data_size = 0;
            }
        }

        /** 
        * 分离缓存 
        * 
        * 参数： 
        *     p_buffer_size [out] 缓存的容量 
        *     p_data_size [out]  数据的大小 
        * 
        * 返回值： 缓存的指针 
        * 
        * 备注： 
        *     1. 调用此接口后，当前Buffer对象的缓存指针、缓存容量、数据大小均为0 
        *     2. 调用此接口后，缓存的拥有权析出，缓存新的拥有者需要自己维护缓存，释放时使用 my_destroy_array<char>(p)  （p为detach返回的指针） 
        */ 
        char* detach(int *p_buffer_size, int *p_data_size)
        {
            char *p_buffer = m_p_buffer;
            if (p_buffer_size != nullptr)
            {
                *p_buffer_size = m_buffer_size;
            }
            if (p_data_size != nullptr)
            {
                *p_data_size = m_data_size;
            }

            m_p_buffer = nullptr;
            m_buffer_size = 0;
            m_data_size = 0;

            return p_buffer;
        }

        /** 
        * 销毁缓存 
        * 
        * 参数： 空 
        * 
        * 返回值： 空 
        * 
        * 备注： 调用此接口后，当前Buffer对象的缓存指针、缓存容量、数据大小均为0 
        */ 
        void destroy()
        {
            if (m_p_buffer != nullptr)
            {
                delete [] m_p_buffer;
                m_p_buffer = nullptr;
                m_buffer_size = 0;
                m_data_size = 0;
            }
        }

        /** 
        * 向缓存拷入数据 
        * 
        * 参数： 
        *       p_data [in] 外部数据指针 
        *       data_size [in] 外部数据大小 
        *       auto_resize [in] 自动扩容标志 
        * 
        * 返回值： true:拷贝成功，false：拷贝失败 
        * 
        * 备注： 
        *     1. 此接口从缓存的0位置写入数据 
        *     2. 若data_size大于缓存容量且auto_resize为真则扩容并复制，否则返回失败，原缓存数据不变 
        *     3. 接口会设置数据的大小，外部无需维护 
        */ 
        bool copy(const void *p_data, const int data_size, bool auto_resize = true)
        {
            if (p_data == nullptr || data_size < 0)
            {
                return false;
            }

            if (data_size == 0)
            {
                clear();
                return true;
            }

            if (data_size > m_buffer_size && (!auto_resize || !resize(data_size)))
            {
                return false;
            }

            memcpy(m_p_buffer, p_data, data_size);
            m_data_size = data_size;

            return true;
        }

        /** 
        * 向缓存尾部拷入数据 
        * 
        * 参数： 
        *     p_data [in] 外部数据指针 
        *     data_size [in] 外部数据大小 
        *     auto_resize [in] 自动扩容标志 
        * 
        * 返回值： true:拷贝成功，false：拷贝失败 
        * 
        * 备注： 
        *     1. 此接口从缓存目前已有数据的尾部的下一字节开始写入数据 
        *     2. 若data_size大于缓存的剩余容量且auto_resize为真则扩容并复制，否则返回失败，原缓存数据不变 
        *     3. 接口会设置新数据的大小，外部无需维护 
        */ 
        bool push(const void *p_data, const int data_size, bool auto_resize = true)
        {
            if (p_data == nullptr || data_size < 0)
            {
                return false; /// < 参数无效 
            }

            if (data_size == 0)
            {
                return true;
            }

            int new_data_size = m_data_size + data_size;
            if (new_data_size > m_buffer_size && (!auto_resize || !resize(new_data_size, true)))
            {
                return false;
            }

            memcpy(m_p_buffer + m_data_size, p_data, data_size);
            m_data_size = new_data_size;

            return true;
        }

        /** 
        * 插入数据 
        * 
        * 参数： 
        *     p_data [in] 外部数据指针 
        *     data_size [in] 外部数据大小 
        *     start_pos [in] 插入的起始位置 
        *     auto_resize [in] 自动扩容标志 
        * 
        * 返回值： true:插入成功，false：插入失败 
        * 
        * 备注： 
        *     1. 遵循C的风格，start_pos从0而非1开始计算
        *     2. 指定的插入位置不可超过现有数据大小（等于现有数据大小时，即为从尾部下一字节写入数据，与push()功能相同） 
        *     3. 接口会设置新数据的大小，外部无需维护 
        */ 
        bool insert(const void *p_data, const int data_size, const int start_pos, bool auto_resize = true)
        {
            if (p_data == nullptr || data_size < 0 || start_pos < 0 || start_pos > m_data_size)
            {
                return false; /// < 参数错误 
            }

            if (data_size == 0)
            {
                return true;
            }

            int new_data_size = m_data_size + data_size;
            if (new_data_size <= m_buffer_size)
            {
                memmove(m_p_buffer + start_pos + data_size, m_p_buffer + start_pos, m_data_size - start_pos);
            }
            else
            {
                if (!auto_resize)
                {
                    return false;
                }

                char *p_new_buffer = nullptr;
                try {p_new_buffer = new char[new_data_size];}catch(...){}
                if (p_new_buffer == nullptr)
                {
                    return false;
                }
 
                memcpy(p_new_buffer, m_p_buffer, start_pos);
                memcpy(p_new_buffer + start_pos + data_size, m_p_buffer + start_pos, m_data_size - start_pos);

                delete []m_p_buffer;

                m_p_buffer = p_new_buffer;
                m_buffer_size = new_data_size;
            }

            memcpy(m_p_buffer + start_pos, p_data, data_size); /// < 数据插入 
            m_data_size = new_data_size;

            return true;
        }

        /** 
        * 设置指定位置的值 
        * 
        * 参数： 
        *     _pos [in] 待设值的位置 
        *     _value [in] 待设置的值 
        * 
        * 返回值： true:设置成功，false：设置失败 
        * 
        * 备注： 遵循C风格，缓存位置从0开始计算 
        */ 
        bool set(const int _pos, const char _value)
        {
            if (_pos < 0 || _pos >= m_buffer_size)
            {
                return false; /// < 位置越界 
            }

            m_p_buffer[_pos] = _value;

            return true;
        }

        /** 
        * 设置指定区间的值 
        * 
        * 参数： _begin [in] 区间开始位置 
        *        _end [in] 区间结束位置 
        *        _value [in] 待设置的值 
        * 
        * 返回值： true:设置成功，false：设置失败 
        * 
        * 备注： 
        *     1. 遵循C风格，缓存位置从0开始计算 
        *     2. 调用此接口后，外部需自己维护缓存的数据大小 
        *     3. 区间为[_begin, _end] 
        */ 
        bool set(const int _begin, const int _end, const char _value)
        {
            if (_begin > _end)
            {
                return false; /// < 参数错误 
            }

            if (_begin < 0 || _end >= m_buffer_size)
            {
                return false; /// < 位置越界 
            }

            memset(m_p_buffer + _begin, _value, _end - _begin + 1); /// < 因为[_begin, _end]，所以 + 1 

            return true;
        }
    };
}

#endif
