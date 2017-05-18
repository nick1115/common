#ifndef __TEST_BUFFER_H_BY_MOUYUN_2017_05_18__
#define __TEST_BUFFER_H_BY_MOUYUN_2017_05_18__

#include <iostream>
#include <string>
#include "../my_buffer.hpp"

class BufferTester
{
public:
    void test()
    {
        std::cout << "=====================Buffer test begin=====================" << std::endl;
        test_normal_constructor();
        test_int_constructor();
        test_str_constructor();
        test_data_constructor();
        test_resize();
        test_ensure();
        test_copy();
        test_push();
        test_insert();
        std::cout << "=====================Buffer test end=======================" << std::endl;
    }

public:
    void test_normal_constructor()
    {
        my_module_space::Buffer buf;

        if (buf.buf() == nullptr && 
            buf.buffer_size() == 0 && 
            buf.data_size() == 0)
        {
            std::cout << "  test_normal_constructor: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_normal_constructor: FAIL" << std::endl;
        }
    }

    void test_int_constructor()
    {
        my_module_space::Buffer buf(666);

        if (buf.buf() != nullptr && 
            buf.buffer_size() == 666 && 
            buf.data_size() == 0)
        {
            std::cout << "  test_int_constructor: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_int_constructor: FAIL" << std::endl;
        }
    }

    void test_str_constructor()
    {
        std::string str("double_kill, triple_kill, ultra_kill, rampage");
        my_module_space::Buffer buf(str.c_str());

        if (buf.buf() != nullptr && 
            buf.buffer_size() >= str.size() + 1 && 
            buf.data_size() == str.size() + 1 && 
            str == buf.buf_char())
        {
            std::cout << "  test_str_constructor: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_str_constructor: FAIL" << std::endl;
        }
    }

    void test_data_constructor()
    {
        std::string str("C++ template meta programming is fucking shit");
        my_module_space::Buffer buf(str.c_str(), str.size() + 1);

        if (buf.buf() != nullptr && 
            buf.buffer_size() >= str.size() + 1 && 
            buf.data_size() == str.size() + 1 && 
            str == buf.buf_char())
        {
            std::cout << "  test_data_constructor: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_data_constructor: FAIL" << std::endl;
        }
    }

    void test_resize()
    {
        my_module_space::Buffer buf;

        if (buf.resize(999) && buf.buffer_size() == 999 && buf.data_size() == 0 && 
            buf.resize(666) && buf.buffer_size() == 666 && buf.data_size() == 0 && 
            buf.resize(2333333) && buf.buffer_size() == 2333333 && buf.data_size() == 0)
        {
            std::cout << "  test_resize: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_resize: FAIL" << std::endl;
        }
    }

    void test_ensure()
    {
        my_module_space::Buffer buf;

        if (buf.ensure(666) && 
            buf.buf() != nullptr && 
            buf.buffer_size() >= 666)
        {
            std::cout << "  test_ensure: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_ensure: SUCCESS" << std::endl;
        }
    }

    void test_copy()
    {
        std::string str("aoeiuvbpmfdtnlgkhjqx");
        my_module_space::Buffer buf;

        if (buf.copy(str.c_str(), str.size() + 1) && 
            buf.buf() != nullptr && 
            buf.buffer_size() >= str.size() + 1 && 
            buf.data_size() == str.size() + 1 && 
            str == buf.buf_char())
        {
            std::cout << "  test_copy: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_copy: FAIL" << std::endl;
        }
    }

    void test_push()
    {
        std::string str1("ni_shi_da_ben_dan");
        std::string str2("hahahaha,hahahaha");

        my_module_space::Buffer buf;
        
        if (buf.push(str1.c_str(), str1.size()) && 
            buf.push(str2.c_str(), str2.size() + 1) && 
            buf.buf() != nullptr && 
            buf.buffer_size() >= str1.size() + str2.size() + 1 && 
            buf.data_size() == str1.size() + str2.size() + 1 && 
            (str1 + str2) == buf.buf_char())
        {
            std::cout << "  test_push: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_push: FAIL" << std::endl;
        }
    }

    void test_insert()
    {
        std::string str1("an old driver ");
        std::string str2("is driving ");
        std::string str3("on qiuming moutain");

        my_module_space::Buffer buf(str1.c_str());

        buf.data_size(buf.data_size() - 1); //discard the '\0' in string's tail

        if (buf.push(str3.c_str(), str3.size() + 1) && 
            buf.insert(str2.c_str(), str2.size(), str1.size()) && 
            buf.buf() != nullptr && 
            buf.buffer_size() >= (str1 + str2 + str3).size() + 1 && 
            buf.data_size() == (str1 + str2 + str3).size() + 1 && 
            (str1 + str2 + str3) == buf.buf_char())
        {
            std::cout << "  test_insert: SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "  test_insert: FAIL" << std::endl;
        }
    }
};


#endif