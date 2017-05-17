#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "../../common/my_common_header.hpp"
#include "../../common/my_any_type.hpp"
#include "../../common/my_buffer.hpp"
#include "../../common/my_ioc.hpp"
#include "../../common/my_event.hpp"
#include "../../common/my_semphore.hpp"
#include "../../common/my_object_pool.hpp"
#include "../../common/my_processor.hpp"
#include "../../common/my_timer.hpp"

#include "test.h"

using namespace my_module_space;

DEFINE_OBJECT_POOL(TaskPacket, PacketPool)

volatile int g_quit_flag = 0;
void generate(processor<TaskPacket> *p_consumer)
{
    while (1)
    {
        if (g_quit_flag != 0)
        {
            break;
        }

        auto sp_task(PacketPool::pop_sp());
        sp_task->tm.reset();
        int result = p_consumer->put_task(sp_task);
        if (result != PROCESSOR_SUCCESS)
        {
            if (result == PROCESSOR_QUEUE_FULL)
            {
                std::cout << "processor's queue is full" << std::endl;
            }
        }

        //thread_sleep_ms(rand() % 500);
        thread_sleep_ms(500);
    }
}

int main()
{
    srand((unsigned) time(NULL) * 100);

    TaskHandler1 th1;
    TaskHandler2 th2;
    TaskHandler3 th3;
    TaskHandler4 th4;
    TaskHandler5 th5;

    processor<TaskPacket> task_id_processor(std::bind(&TaskHandler1::work, &th1, std::placeholders::_1));
    processor<TaskPacket> task_number_processor(std::bind(&TaskHandler2::work, &th2, std::placeholders::_1));
    processor<TaskPacket> task_text_processor1(std::bind(&TaskHandler3::work, &th3, std::placeholders::_1));
    processor<TaskPacket> task_text_processor2(std::bind(&TaskHandler3::work, &th3, std::placeholders::_1));
    processor<TaskPacket> task_text_processor3(std::bind(&TaskHandler3::work, &th3, std::placeholders::_1));
    processor<TaskPacket> task_print_number_processor(std::bind(&TaskHandler4::work, &th4, std::placeholders::_1));
    processor<TaskPacket> task_print_text_processor(std::bind(&TaskHandler5::work, &th5, std::placeholders::_1));

    task_id_processor.add_next_processor(&task_number_processor);
    task_number_processor.add_next_processor(&task_text_processor1);
    task_text_processor1.add_next_processor(&task_text_processor2);
    task_text_processor2.add_next_processor(&task_text_processor3);
    task_text_processor3.add_next_processor(&task_print_number_processor);
    task_print_number_processor.add_next_processor(&task_print_text_processor);

    task_print_text_processor.begin_thread(1);
    task_print_number_processor.begin_thread(1);
    task_text_processor3.begin_thread(1);
    task_text_processor2.begin_thread(1);
    task_text_processor1.begin_thread(1);
    task_number_processor.begin_thread(1);
    task_id_processor.begin_thread(1);




    std::vector<std::thread> thr_vec;
    for (int i = 0; i < 1; ++i)
    {
        thr_vec.push_back(std::thread(generate, &task_id_processor));
    }

    int cmd = 0;
    while (std::cin >> cmd)
    {
        if (cmd == 1)
        {
            g_quit_flag = 1;
            for (auto &cur_thread : thr_vec)
            {
                cur_thread.join();
            }

            task_id_processor.end_all_threads();
            task_number_processor.end_all_threads();
            task_text_processor1.end_all_threads();
            task_text_processor2.end_all_threads();
            task_text_processor3.end_all_threads();
            task_print_number_processor.end_all_threads();
            task_print_text_processor.end_all_threads();

            break;
        }
    }

    std::cout << "all thread stoped" << std::endl;
    return 0;
}