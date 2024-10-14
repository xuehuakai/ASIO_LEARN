#include"Session.h"
Session::Session(boost::asio::io_context & ioc):_socket(ioc){

}

void Session::print_err(const boost::system::error_code & ec){
    if(ec.value()!=0){
        cout<<"Error occured ! Error Code = "<<ec.value()<<". Message:"<<ec.message();
        return;
    } 
}

void Session::Start(){
    memset(_data,0,max_length);
    _socket.async_read_some(boost::asio::buffer(_data,max_length),std::bind(&Session::handle_read,this,placeholders::_1,placeholders::_2));
}
void Session::handle_read(const boost::system::error_code & ec , size_t bytes_transferred){
    if(!ec)
        cout<<"receive data is "<<_data<<endl;
    boost::asio::async_write(_socket,boost::asio::buffer(_data,bytes_transferred),
    std::bind(&Session::handle_write,this,placeholders::_1)); //bind将成员函数绑定为普通函数
}
void Session::handle_write(const boost::system::error_code & ec){
    if(!ec){
        memset(_data,0,max_length);
        _socket.async_read_some(boost::asio::buffer(_data,max_length),std::bind(&Session::handle_read,this,placeholders::_1,placeholders::_2));
    }
}

Server::Server(boost::asio::io_context & ioc,short port):_ioc(ioc),_acceptor(tcp::acceptor(ioc,tcp::endpoint(tcp::v4(),port))){
    cout<<"Server start success , on port : "<<port<<endl;
    start_accept();
}
void Server::start_accept(){
    Session* new_session = new Session(_ioc);
    _acceptor.async_accept(new_session->Socket(),std::bind(&Server::handle_accept,this,new_session,placeholders::_1));
}
void  Server::handle_accept(Session * new_session,const boost::system::error_code & ec){
    if(!ec){
        new_session->Start();
    }else{
        delete new_session;
    }

    start_accept();
}