#pragma once
#ifndef _SERVER_H_089238
#define _SERVER_H_089238

#include "connection.h"
#include "connectionmanager.h"
#include "worker.h"

namespace server_core {
    class Server {

    public:
        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;

        explicit Server(std::shared_ptr<Worker> worker, unsigned int thread_num = 1);

        void start(uint16_t port);

    private:
        void start_accept();
        void handle_new_connection(std::shared_ptr<Connection> cptr, const boost::system::error_code& ec);

    private:
        unsigned int                   thread_count_;
        std::vector<std::thread>       thread_pool_;
        boost::asio::io_service        ios_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::shared_ptr<Worker>        worker_;

        ConnectionManager connection_manager_;

    };
}

#endif