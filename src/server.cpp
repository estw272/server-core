#include "pch.h"
#include "../include/servercore/server.h"

server_core::Server::Server(std::shared_ptr<Worker> worker, unsigned int thread_num) : 
    thread_count_(thread_num), 
    acceptor_(ios_), 
    worker_(std::move(worker)) {

}

void server_core::Server::start(uint16_t port) {

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();

    for (size_t i = 0; i < thread_count_; ++i) {
        thread_pool_.emplace_back([=](){ios_.run();});
    }

    ios_.run();

}

void server_core::Server::start_accept() {
    auto new_connection = std::make_shared<Connection>(ios_, connection_manager_, worker_);

    acceptor_.async_accept(new_connection->socket(), [=](auto ec){
        handle_new_connection(new_connection, ec);
    });
}

void server_core::Server::handle_new_connection(std::shared_ptr<Connection> cptr, const boost::system::error_code& ec) {
    if (!acceptor_.is_open()) {
        LOG->error("acceptor is not open, returning");
        return;
    }

    if (!ec) {
        connection_manager_.start(cptr);
    }

    start_accept();

}
