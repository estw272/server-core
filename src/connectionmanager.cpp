#include "pch.h"
#include "connectionmanager.h"
#include "coreutility.h"

ConnectionManager::ConnectionManager() {

}

void ConnectionManager::start(std::shared_ptr<Connection> cptr) {
    std::lock_guard<std::mutex> lk(conn_mutex_);
    connections_.insert(cptr);
    cptr->start();
    print();
}

void ConnectionManager::stop(std::shared_ptr<Connection> cptr) {
    {
		std::lock_guard<std::mutex> lk(conn_mutex_);
		cptr->stop();
		connections_.erase(cptr);
    }
    print();
}

void ConnectionManager::stop_all() {
    std::for_each(std::cbegin(connections_), std::cend(connections_), [](auto cptr){
        cptr->stop();
    });
    connections_.clear();
	print();
}

void ConnectionManager::print() const {
    coreutility::winapi::cls();
	std::cout << "Total connections: " << connections_.size() << "\n\n";
	if (!connections_.empty()) {
		std::cout << "Active connections:\n";
		std::for_each(connections_.cbegin(), connections_.cend(), [](std::shared_ptr<Connection> c){
			std::cout << c->socket().remote_endpoint().address().to_string() << ":" << c->socket().remote_endpoint().port() << "\n";
		});
	}
}

void ConnectionManager::broadcast_msg(unsigned int program, std::string msg) {
    for_each(connections_.cbegin(), connections_.cend(), [&program, msg](auto conn){
		if (conn->get_program() == program) {
			conn->send(msg);
		}
	});
}