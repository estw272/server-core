#include "pch.h"
#include "connection.h"
#include "connectionmanager.h"
#include "worker.h"

Connection::Connection(boost::asio::io_service& ios, ConnectionManager& cm, std::shared_ptr<Worker> worker) : 
    ios_(ios), 
    socket_(ios_), 
    write_strand_(ios_), 
    connection_manager_(cm), 
    worker_(worker),
    message_ongoing_(false), 
    message_buffer_str_(""), 
    prev_message_(""), 
    username_(""), 
    program_(0), 
    authorized_(false), 
    started_auth_(false) {
}

void Connection::start() {
    LOG->info("connection opened by peer {}:{}", 
        socket_.remote_endpoint().address().to_string(), 
        socket_.remote_endpoint().port()
    );

    start_read();
}

void Connection::stop() {
    try {
		boost::system::error_code ec;
		LOG->info("closing socket: {}:{}", 
			socket_.remote_endpoint().address().to_string(), 
			socket_.remote_endpoint().port());
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (ec) {
			LOG->error("error while closing socket: {}", 
				ec.message());
			socket_.close();
			return;
		}
		socket_.close();
    } catch (const std::exception& e) {
        LOG->error("error closing socket: {}", e.what());
    }
}

void Connection::close_connection() {
    connection_manager_.stop(shared_from_this());
}

void Connection::start_read() {
    boost::asio::streambuf::mutable_buffers_type mut_buffer_in = buffer_in_.prepare(512);
    socket_.async_read_some(mut_buffer_in, [me=shared_from_this()](const boost::system::error_code& ec, std::size_t bytes_trans){
        me->handle_read(ec, bytes_trans);
    });
}

void Connection::handle_read(const boost::system::error_code& ec, std::size_t bytes_trans) {
    if (!ec) {
        auto read_buffer = [this, &bytes_trans]() -> std::string {
            buffer_in_.commit(bytes_trans);
            std::string temp_msg_str((std::istreambuf_iterator<char>(&buffer_in_)), std::istreambuf_iterator<char>());
            return temp_msg_str;
        };

        message_buffer_str_ += read_buffer();
        process_buffer();

        start_read();
    } else if (boost::asio::error::eof == ec) {
        LOG->info("connection closed by peer: {}:{}", 
			socket_.remote_endpoint().address().to_string(), 
			socket_.remote_endpoint().port());
        connection_manager_.stop(shared_from_this());
    } else if (boost::asio::error::operation_aborted != ec) {
        LOG->error("error: {}:{}, {}", 
			socket_.remote_endpoint().address().to_string(), 
			socket_.remote_endpoint().port(), 
			ec.message());
        connection_manager_.stop(shared_from_this());
    }
}

void Connection::send(std::string message) {
    ios_.post(write_strand_.wrap([me=shared_from_this(), message](){
        me->queue_message(message);
    }));
}

void Connection::queue_message(std::string message) {
    bool writing = !buffer_out_queue_.empty();
    buffer_out_queue_.push_back(std::move(message));

    if (!writing) {
        start_write();
    }
}

void Connection::start_write() {
    buffer_out_queue_.front() = msg_begin_str_ + buffer_out_queue_.front() + msg_end_str_;
    boost::asio::async_write(socket_, 
                            boost::asio::buffer(buffer_out_queue_.front()), 
                            write_strand_.wrap([me=shared_from_this()](const boost::system::error_code& ec, std::size_t bytes_sent) {
                                me->handle_write(ec, bytes_sent);
                            }));

}

void Connection::handle_write(const boost::system::error_code& ec, [[maybe_unused]] const size_t bytes_sent) {
    if (!ec) {
        buffer_out_queue_.pop_front();

        if (!buffer_out_queue_.empty()) {
            start_write();
        }
    }
}

boost::asio::ip::tcp::socket& Connection::socket() {
    return socket_;
}

void Connection::process_buffer() {
    auto msg_start = message_buffer_str_.find(msg_begin_str_);
    if (msg_start == std::string::npos) {
        if (message_buffer_str_.size() > 4096) {
            message_buffer_str_ = message_buffer_str_.erase(0, message_buffer_str_.size() - msg_begin_str_.size());
        }
        return;
    }

    auto msg_end = message_buffer_str_.find(msg_end_str_);
    if (msg_end == std::string::npos) {
        return;
    }

    std::string complete_message = message_buffer_str_.substr(msg_start + msg_begin_str_.size(), msg_end - msg_start - msg_begin_str_.size());
    worker_->queue_task(shared_from_this(), std::move(complete_message));

    message_buffer_str_ = message_buffer_str_.substr(msg_end + msg_end_str_.size());
    process_buffer();
}

unsigned int Connection::get_program() const {
    return this->program_;
}

std::string Connection::get_username() const {
    return this->username_;
}

void Connection::set_program(unsigned int p) {
    this->program_ = p;
}

void Connection::set_username(std::string username) {
    this->username_ = username;
}

std::string Connection::get_address() const {
    return this->socket_.remote_endpoint().address().to_string();
}

void Connection::broadcast(unsigned int program, std::string message) {
    this->connection_manager_.broadcast_msg(program, std::move(message));
}

void Connection::set_started_auth(bool s) {
    this->started_auth_ = s;
}

bool Connection::started_auth() const {
    return this->started_auth_;
}

void Connection::set_authorized(bool auth) {
    authorized_ = auth;
}
