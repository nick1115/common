#include <iostream>
#include "../my_singleton.hpp"

using namespace my_module_space;

class SingletonClass
{
public:
    void func()
    {
        ++m_id;
        std::cout << "Test::func() print:" << "id is " << m_id <<std::endl;
    }

private:
    int m_id = 0;
};

class SingletonTester
{
public:
    void test(const SingletonClass *p)
    {
        std::cout << "=====================Singleton test begin=====================" << std::endl;
        if (p == LockFreeSingleton<SingletonClass>::get_instance())
        {
            std::cout << "test Singleton : SUCCESS" << std::endl;
        }
        else
        {
            std::cout << "test Singleton : FAIL" << std::endl;
        }
        std::cout << "=====================Singleton test end=====================" << std::endl;
    }
};
