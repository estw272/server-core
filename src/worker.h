#pragma once
#ifndef _WORKER_H_389823
#define _WORKER_H_389823

#include "connection.h"

class Worker {
public:
    explicit Worker(unsigned int thread_count = 1); 

    virtual void queue_task(std::shared_ptr<Connection> cptr, std::string request);

    virtual ~Worker() = default;

private:
    virtual void process_request(std::shared_ptr<Connection> cptr, std::string request) = 0;

private:
    unsigned int                  thread_count_;
    std::vector<std::thread>      thread_pool_;
    boost::asio::io_service       ios_;
    boost::asio::io_service::work work_;

};

#endif