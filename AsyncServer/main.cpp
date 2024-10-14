#include"Session.h"
#include<iostream>
int main(){
    try
    {
        boost::asio::io_context ioc;
        Server s(ioc,10086);
        ioc.run(); //轮询
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}