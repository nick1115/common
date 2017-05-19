#ifndef __TEST_PROCESSOR_H_BY_MOUYUN_2017_05_17__
#define __TEST_PROCESSOR_H_BY_MOUYUN_2017_05_17__

#include <iostream>
#include <string>
#include <mutex>
#include <memory>
#include <functional>

#include "../my_timer.hpp"
#include "../my_processor.hpp"

using namespace my_module_space;

class TaskSync
{
public:
    void increase_finished_count()
    {
        m_mtx.lock();
        ++m_task_finished_count;
        m_mtx.unlock();
    }

private:
    std::mutex m_mtx;
    int m_task_count = 0;
    int m_task_finished_count = 0; //atomic_int may be better
};

class TaskPacket
{
public:
    int m_id = 0;
    int m_number = 0;
    std::string m_text;

    my_module_space::Timer m_tm;
public:
    void clear()
    {
        m_id = 0;
        m_number = 0;
        m_text.clear();
        //m_tm .reset();
    }
};

class TaskHandler
{
public:
    void generate_id(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        sp_pkt->m_id = m_task_id++;
    }

    void handle_number(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        sp_pkt->m_number += 1;
    }

    void handle_text(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        sp_pkt->m_text += "x";
    }

    void print_id(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        std::cout << "task id:" << sp_pkt->m_id;
    }

    void print_number(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        std::cout << ", number: " << sp_pkt->m_number;
    }

    void print_text(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        std::cout << ", text: " << sp_pkt->m_text << std::endl;
    }

private:
    int m_task_id = 1;
    //std::mutex mtx; //currently, we use only one thread to gennerate task id 
};

/****************************************************************************************************
 there is a pipeline with some processors to simulate the pipeline of real product
    one processor to generate task id
    one processor to handle number in task packet
    three processor to handle text in task packet
    three processor to print result

we have a thread to generate task packet, note, only task packet, not any task data
****************************************************************************************************/
class ProcessorTester
{
public:
    ProcessorTester()
    {
        //assign task function
        m_id_processor.set_task_function(std::bind(&TaskHandler::generate_id, &m_task_handler, std::placeholders::_1));
        m_number_processor.set_task_function(std::bind(&TaskHandler::handle_number, &m_task_handler, std::placeholders::_1));
        m_text_processor1.set_task_function(std::bind(&TaskHandler::handle_text, &m_task_handler, std::placeholders::_1));
        m_text_processor2.set_task_function(std::bind(&TaskHandler::handle_text, &m_task_handler, std::placeholders::_1));
        m_text_processor3.set_task_function(std::bind(&TaskHandler::handle_text, &m_task_handler, std::placeholders::_1));
        m_print_processor1.set_task_function(std::bind(&TaskHandler::print_id, &m_task_handler, std::placeholders::_1));
        m_print_processor2.set_task_function(std::bind(&TaskHandler::print_number, &m_task_handler, std::placeholders::_1));
        m_print_processor3.set_task_function(std::bind(&TaskHandler::print_text, &m_task_handler, std::placeholders::_1));

        //reversely assemble pipeline 
        m_print_processor2.add_next_processor(&m_print_processor3); //print_processor3 is the last processor in pipeline 
        m_print_processor1.add_next_processor(&m_print_processor2);
        m_text_processor3.add_next_processor(&m_print_processor1);
        m_text_processor2.add_next_processor(&m_text_processor3);
        m_text_processor1.add_next_processor(&m_text_processor2);
        m_number_processor.add_next_processor(&m_text_processor1);
        m_id_processor.add_next_processor(&m_number_processor);
        m_p_first_processor = &m_id_processor; //id_processor is the first processor in pipeline
    }

    ~ProcessorTester()
    {
        stop_test();
    }

public:
    void begin_test()
    {
        stop_test();

        //reversely start all processors
        m_print_processor3.begin_thread(1);
        m_print_processor2.begin_thread(1);
        m_print_processor1.begin_thread(1);
        m_text_processor3.begin_thread(1);
        m_text_processor2.begin_thread(1);
        m_text_processor1.begin_thread(1);
        m_number_processor.begin_thread(1);
        m_id_processor.begin_thread(1);

        //start task gennerator thread
        m_sp_task_generator = std::make_shared<std::thread>([this](Processor<TaskPacket> *pfp)->void
        {
            while (true)
            {
                if (this->m_quit_flag != 0)
                {
                    break;
                }

                auto sp_task(std::make_shared<TaskPacket>());
                pfp->put_task(sp_task);

                thread_sleep_ms(500);
            }
        }, m_p_first_processor);

        std::cout << "processor test begin" << std::endl;
    }

    void stop_test()
    {
        if (m_sp_task_generator != nullptr)
        {
            if (m_sp_task_generator->joinable())
            {
                m_quit_flag = 1;
                thread_sleep_ms(0);
                m_sp_task_generator->join();
                m_quit_flag = 0;
            }
            m_sp_task_generator.reset();

            m_id_processor.end_all_threads();
            m_number_processor.end_all_threads();
            m_text_processor1.end_all_threads();
            m_text_processor2.end_all_threads();
            m_text_processor3.end_all_threads();
            m_print_processor1.end_all_threads();
            m_print_processor2.end_all_threads();
            m_print_processor3.end_all_threads();

            std::cout << "processor test stop" << std::endl;
        }
    }

private:
    TaskHandler m_task_handler;

    Processor<TaskPacket> *m_p_first_processor;

    Processor<TaskPacket> m_id_processor;
    Processor<TaskPacket> m_number_processor;
    Processor<TaskPacket> m_text_processor1;
    Processor<TaskPacket> m_text_processor2;
    Processor<TaskPacket> m_text_processor3;
    Processor<TaskPacket> m_print_processor1;
    Processor<TaskPacket> m_print_processor2;
    Processor<TaskPacket> m_print_processor3;

    std::shared_ptr<std::thread> m_sp_task_generator;
    volatile int m_quit_flag = 0;
};




#endif
