#pragma once
#ifndef _CONNECTIONMANAGER_H_8922398
#define _CONNECTIONMANAGER_H_8922398

#include "connection.h"

class ConnectionManager {
public:
    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

    ConnectionManager();

    void start(std::shared_ptr<Connection> cptr);
    void stop(std::shared_ptr<Connection> cptr);
    void stop_all();
    void print() const;

    void broadcast_msg(unsigned int program, std::string msg);

private:
    std::set<std::shared_ptr<Connection>> connections_;
    std::mutex conn_mutex_;

};


#endif