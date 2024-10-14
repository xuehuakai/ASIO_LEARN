#pragma once
#include<boost/asio.hpp>
#include<iostream>
#include<set>
#include<memory>

using namespace boost::asio::ip;
using namespace std;

const int max_length = 1024;
typedef shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
set<shared_ptr<thread>> thread_set;


void session(socket_ptr sock){
    try
    {
        for(;;){
            char data[max_length];
            memset(data,0,max_length);
            boost::system::error_code error;
       //  size_t length = boost::asio::read(sock,boost::asio::buffer(data,max_length),error); //阻塞
            size_t length = sock->read_some(boost::asio::buffer(data,max_length),error);
            if(error == boost::asio::error::eof){
                cerr<<"connection closed by peer"<<endl;
                break;
            }else if(error){
                throw boost::system::system_error(error);
            }

            cout<<"receive from "<<sock->remote_endpoint().address().to_string()<<endl;
            cout<<"receive message is "<<data<<endl;

            //回传
            sock->write_some(boost::asio::buffer(data,length),error);
            

        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << " in tread " <<'\n';
    }
    

}

//connect to client
int server(boost::asio::io_context & ioc,unsigned short port){
    tcp::acceptor a(ioc,tcp::endpoint(tcp::v4(),port)); //上下文 本地要绑定的地址
    //acceptor专门接收客户端连接
    for(;;){
        socket_ptr socket(new tcp::socket(ioc));
        a.accept(*socket); 
        shared_ptr<thread> it = make_shared<thread>(session,socket);
        thread_set.insert(it);
    }
}


int main(){
    try
    {
        boost::asio::io_context ioc;
        server(ioc,10086);
        //主线程等子线程退出才能退出
        for(auto & it  : thread_set){
                it->join();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}

