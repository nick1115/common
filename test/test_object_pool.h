#ifndef __TEST_OBJECT_POOL_H_BY_MOUYUN_2017_05_18__
#define __TEST_OBJECT_POOL_H_BY_MOUYUN_2017_05_18__

#include <iostream>
#include <string>
#include "../my_object_pool.hpp"

using namespace my_module_space;

class PacketA : public ObjectPool<PacketA>
{
public:
    int id = 0;
    std::string text;
};

class PacketB : public ObjectPool<PacketB>
{
public:
    int id = 999;
    std::string text{"interesting"};
    int clear_flag = 0;
public:
    void clear()
    {
        id = 0;
        text.clear();
        clear_flag = 1;
    }
};

class ObjectPoolTester
{
public:
    void test_object_with_no_clear()
    {
        int id = 233333;
        std::string text("hello world");

        auto sp1 = ObjectPool<PacketA>::pop_sp(); //get packet from pool
        auto p1 = sp1.get(); //get the real address of packet
        sp1->id = id;
        sp1->text = text;
        sp1.reset(); //return packet to pool

        auto sp2 = PacketA::pop_sp();
        auto p2 = sp2.get();
        if (p2 == p1 && sp2->id == id && sp2->text == text) //check
        {
            std::cout << "Test ObjectPool for object without clear() : SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "Test ObjectPool for object without clear() : FAIL" << std::endl;
        }
    }

    void test_object_with_clear()
    {
        int id = 666;
        std::string text("driving");

        auto sp1 = ObjectPool<PacketB>::pop_sp(); //get packet from pool
        auto p1 = sp1.get(); //get the real address of packet
        sp1->id = id;
        sp1->text = text;
        sp1.reset(); //return packet to pool

        auto sp2 = PacketB::pop_sp();
        auto p2 = sp2.get();
        if (p2 == p1 && sp2->id == 0 && sp2->text.empty() && sp2->clear_flag == 1) //check
        {
            std::cout << "Test ObjectPool for object with clear() : SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "Test ObjectPool for object with clear() : FAIL" << std::endl;
        }
    }

    void test()
    {
        std::cout << "=====================ObjectPool test begin=====================" << std::endl;
        test_object_with_no_clear();
        test_object_with_clear();
        std::cout << "=====================ObjectPool test end=======================" << std::endl;
    }
};



#endif
