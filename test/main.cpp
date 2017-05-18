#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "test_buffer.h"
#include "test_any_type.h"
#include "test_ioc.h"
#include "test_event.h"
#include "test_semphore.h"
#include "test_object_pool.h"
#include "test_processor.h"

using namespace my_module_space;

void print_menu()
{
    std::cout << "menu:" << std::endl 
        << "    1: begin processor test" << std::endl 
        << "    2: stop processor test" << std::endl 
        << "    3: buffer test" << std::endl 
        << "    0: print menu" << std::endl 
        << "    -1: quit" << std::endl;
}

int main()
{
    my_module_space::AnyType at(3.14);
    
    srand((unsigned) time(NULL) * 100);

    print_menu();

    ProcessorTester processor_tester;
    BufferTester buffer_tester;

    int cmd = 0;
    while (std::cin >> cmd)
    {
        switch (cmd)
        {
        case -1:
            break;
        case 0:
            print_menu();
            break;
        case 1:
            processor_tester.begin_test();
            break;
        case 2:
            processor_tester.stop_test();
            break;
        case 3:
            buffer_tester.test();
            break;
        default:
        print_menu();
            break;
        }

        if (cmd == -1)
        {
            break;
        }
    }

    std::cout << "test quit" << std::endl;
    return 0;
}
