#pragma once
#ifndef _CONNECTION_H_82382938
#define _CONNECTION_H_82382938

class ConnectionManager;
class Worker;

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    explicit Connection(boost::asio::io_service& ios, ConnectionManager& cm, std::shared_ptr<Worker> worker);

    void start();
    void stop();
    void close_connection();
    void send(std::string message);
    void broadcast(unsigned int program, std::string message);
    boost::asio::ip::tcp::socket& socket();

    unsigned int get_program() const;
    void set_program(unsigned int p);
    std::string get_username() const;
    void set_username(std::string username);
    std::string get_address() const;

    void set_started_auth(bool s);
    bool started_auth() const;
    void set_authorized(bool auth);

private:

	void start_read();
	void handle_read(const boost::system::error_code &, const size_t);
	void start_write();
    void queue_message(std::string message);
	void handle_write(const boost::system::error_code &, const size_t);
    void process_buffer();

private:
    boost::asio::io_service&        ios_;
    boost::asio::ip::tcp::socket    socket_;
    boost::asio::io_service::strand write_strand_;
    boost::asio::streambuf          buffer_in_;
    std::deque<std::string>         buffer_out_queue_;
    ConnectionManager&              connection_manager_;
    std::shared_ptr<Worker>         worker_;

    bool message_ongoing_;
    std::string message_buffer_str_;
    std::string prev_message_;

    std::string  username_;
    unsigned int program_;
    bool         authorized_;

    const std::string msg_begin_str_ = "__MSG_STS__";
    const std::string msg_end_str_   = "__MSG_STE__";

    std::atomic<bool> started_auth_;

};

#endif