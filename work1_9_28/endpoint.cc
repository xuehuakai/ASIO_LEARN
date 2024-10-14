#include"endpoint.h"
#include<iostream>
#include<string>
#include<boost/asio.hpp>
using namespace boost;
using namespace std;
void use_const_buffer(){
    string buf = "heelo";
    asio::const_buffer asio_buf(buf.c_str(),buf.length());
    vector<asio::const_buffer> buffers_sequence;
    buffers_sequence.push_back(asio_buf);
}//麻烦

void use_buffer_str(){
    asio::const_buffers_1 output_buf = asio::buffer("hello");
}//简便

void use_buffer_array(){
    const size_t BUF_SIZE_BYTES = 20;
    unique_ptr<char[]> buf(new char[BUF_SIZE_BYTES]); //数组的内存交给unique_ptr管理
    auto input_buf = asio::buffer(static_cast<void*>(buf.get()),BUF_SIZE_BYTES);
}

void write_to_socket(asio::ip::tcp::socket& sock){
    string buf = "jzy";
    size_t total_bytes_written = 0;//控制已经发送的数据的长度
    
    //循环发送
    //write_some 返回每次写入的字节数
    while(total_bytes_written != buf.length()){
        total_bytes_written += sock.write_some(asio::buffer(
            buf.c_str()+total_bytes_written,buf.length()-total_bytes_written
        ));
    }
}


int send_data_by_write_some(){
    string raw_ip_address = "192.168.3.11";
    unsigned short port_num = 3333;
    try{
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address),port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc,ep.protocol());
        sock.connect(ep);
        write_to_socket(sock);
    }catch(system::system_error& e){
        return e.code().value();
    }
}

//每次发送多少主要取决于内核态，write_some会返回实际发送的长度
//sock.send() 是你中途没有发完没必要告诉我，发完再告诉我

int send_data_by_send(){
     string raw_ip_address= "192.168.3.11";
       string buf = "jzy";
    unsigned short port_num = 3333;
    try{
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address),port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc,ep.protocol());
        sock.connect(ep);
        //send等待阻塞 直到发送完毕
        int send_length = sock.send(asio::buffer(buf.c_str(),buf.length()));
        //<0 socket系统级错误 =0 对端关闭  >0 一定是buf.length()
        if(send_length<=0){
            cerr<<"send failed"<<endl;
            return 0;
        }
    }catch(system::system_error& e){
        return e.code().value();
    }
}

int send_data_by_write(){
     string raw_ip_address= "192.168.3.11";
       string buf = "jzy";
    unsigned short port_num = 3333;
    try{
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address),port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc,ep.protocol());
        sock.connect(ep);
        
        int send_length = asio::write(sock,asio::buffer(buf.c_str(),buf.length()));
        if(send_length<=0){
            return 0;
        }
       
    }catch(system::system_error& e){
        return e.code().value();
    }
}


string read_from_socket(asio::ip::tcp::socket& sock){
    const unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    size_t total_bytes_read = 0;
    while(total_bytes_read != MESSAGE_SIZE){
        total_bytes_read += sock.read_some(asio::buffer(buf+total_bytes_read,MESSAGE_SIZE-total_bytes_read));
    }
    return string{buf,total_bytes_read};
}

int read_data_by_read_some(){
    string raw_address = "";
    unsigned short port = 3333;
    asio::io_context ioc;
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_address),port);
    asio::ip::tcp::socket sock(ioc,ep.protocol());
    sock.connect(ep);
    string ans = read_from_socket(sock);
    return ans.length();
}



int send_data_by_receive(){
     string raw_ip_address= "192.168.3.11";
       string buf = "jzy";
    unsigned short port_num = 3333;
    try{
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address),port_num);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc,ep.protocol());
        sock.connect(ep);
        const unsigned char BUF_SIZE = 7;
        char buffer_receive[BUF_SIZE];
        int receive_length = sock.receive(asio::buffer(buffer_receive,BUF_SIZE));
        //int receive_length = asio::read(sock,asio::buffer(buffer_receive,BUF_SIZE));
        if(receive_length<=0){
            //error
        }
    }catch(system::system_error& e){
        return e.code().value();
    }
}