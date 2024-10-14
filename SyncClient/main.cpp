#include<boost/asio.hpp>
#include<iostream>
using namespace boost::asio::ip;
using namespace std;
const int MAX_LENGTH = 1024;
int main(){
    try
    {
        //创建上下文服务
        boost::asio::io_context ioc;
        //构造endpoint 
        tcp::endpoint remote_ep(address::from_string("127.0.0.1"),10086);
        tcp::socket sock(ioc);
        boost::system::error_code error = boost::asio::error::host_not_found;
        sock.connect(remote_ep,error);
        if(error){
            cout<<"connect failed,code is "<<error.value()<<
            "error msg is "<<error.message()<<endl;
            return 0;
        }

        cout<<"Enter message:";
        char request[MAX_LENGTH];
        std::cin.getline(request,MAX_LENGTH);
        size_t request_lengrh = strlen(request);
        boost::asio::write(sock,boost::asio::buffer(request,request_lengrh));

        char reply[MAX_LENGTH];
        size_t reply_length  = boost::asio::read(sock,boost::asio::buffer(reply,request_lengrh));
        cout<<"Reply is : ";
        std::cout.write(reply,reply_length);
        cout<<endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}