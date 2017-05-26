#ifndef __TEST_OBJECT_POOL_H_BY_MOUYUN_2017_05_18__
#define __TEST_OBJECT_POOL_H_BY_MOUYUN_2017_05_18__

#include <iostream>
#include "../my_object_pool.hpp"

using namespace my_module_space;

class Packet
{
public:
    int id = 0;
    float conf = 0.00f;
};

class ObjectPoolTester
{
public:
    void test()
    {
        auto sp = ObjectPool<Packet>::pop_sp();
        sp->id = 1024;
        sp->conf = 3.14;

        std::cout << "Test ObjectPool: SUCCESS" << std::endl;
    }
};



#endif
