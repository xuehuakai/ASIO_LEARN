#pragma once
#include<iostream>
#include<boost\asio.hpp>
using namespace std;
using boost::asio::ip::tcp;


class Session{
public:
    Session(boost::asio::io_context & ioc);
    tcp::socket & Socket(){return _socket;}
    void Start();
    void print_err(const boost::system::error_code & ec);
private:
    void handle_read(const boost::system::error_code & ec , size_t bytes_transferred);
    void handle_write(const boost::system::error_code & ec);
    tcp::socket _socket;
    enum{
        max_length = 1024
    };

    char _data[max_length];
};

class Server{
public:
    Server(boost::asio::io_context & ioc,short port);
private:
    void start_accept();//启动专门监听的描述符
    void handle_accept(Session * new_session,const boost::system::error_code & ec);//当有链接进来触发的回调函数
    boost::asio::io_context & _ioc;//io_context不允许有拷贝构造
    tcp::acceptor _acceptor;
};