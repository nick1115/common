#ifndef __CPP_TEST_H_BY_MOUYUN_2017_05_17__
#define __CPP_TEST_H_BY_MOUYUN_2017_05_17__

#include <string>
#include <mutex>

#include "../../common/my_timer.hpp"
#include "../../common/my_buffer.hpp"
#include "../../common/my_processor.hpp"

class SyncTask
{
public:
    void increase_finished_count()
    {
        
    }
private:
    std::mutex mtx;
    int task_count = 0;
    int task_finished_count = 0;
}

class TaskPacket
{
public:
    int id = 0;
    int number = 0;
    std::string text;
    my_module_space::buffer buf;

    my_module_space::timer tm;
public:
    void clear()
    {
        id = 0;
        number = 0;
        text.clear();
        buf.clear();
        //tm .reset();
    }
};


class TaskHandler1
{
public:
    void work(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        sp_pkt->id = id++;
    }

private:
    int id = 1;
    //std::mutex mtx;
};


class TaskHandler2
{
public:
    void work(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        sp_pkt->number += 1;
    }
};

class TaskHandler3
{
public:
    void work(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        sp_pkt->text += "x";
    }
};


class TaskHandler4
{
public:
    void work(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        std::cout << "task id:" << sp_pkt->id 
                    << ", number: " << sp_pkt->number;
    }
};


class TaskHandler5
{
public:
    void work(std::shared_ptr<TaskPacket> &sp_pkt)
    {
        std::cout << ", text: " << sp_pkt->text << std::endl;
    }
};





#endif