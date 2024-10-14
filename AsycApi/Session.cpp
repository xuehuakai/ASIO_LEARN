#include"Session.h"
Session::Session(std::shared_ptr<ip::tcp::socket>  socket):_socket(socket),
_send_pending(false),_recv_pending(false){

}
void Session::Connect(const ip::tcp::endpoint & ep){
    _socket->connect(ep);
}

void Session::print_err(const boost::system::error_code & ec){
    if(ec.value()!=0){
        cout<<"Error occured ! Error Code = "<<ec.value()<<". Message:"<<ec.message();
        return;
    } 
}

void Session::WriteCallBackErr(  const boost::system::error_code & ec,size_t bytes_transferred,shared_ptr<MsgNode> msg_node){
    if(bytes_transferred + msg_node->_cur_len < msg_node->_total_len){
        _send_node->_cur_len += bytes_transferred;
        this->_socket->async_write_some(boost::asio::buffer(_send_node->_msg + _send_node->_cur_len,_send_node->_total_len-_send_node->_cur_len),
        std::bind(&Session::WriteCallBackErr,this,std::placeholders::_1,
        std::placeholders::_2,_send_node));
    }
}

void Session::WriteToSocketErr(const string  buf){
    _send_node = make_shared<MsgNode>(buf.c_str(),buf.length());
    this->_socket->async_write_some(boost::asio::buffer(_send_node->_msg,_send_node->_total_len),bind(&Session::WriteCallBackErr,this,std::placeholders::_1,std::placeholders::_2,_send_node));
}

void Session::WriteToSocket(const string & buf){
    //插入发送队列
    _send_queue.emplace(new MsgNode(buf.c_str(),buf.length()));
    //pending状态说明上一次有未发送完的数据
    if(_send_pending){
        return;
    }
    //异步发送数据，因为异步状态所以不会一下子发送完
    this->_socket->async_write_some(boost::asio::buffer(buf),std::bind(&Session::WriteCallBack,this,std::placeholders::_1,std::placeholders::_2
    ));
    _send_pending = true;
}

void Session::WriteCallBack(const boost::system::error_code & ec , size_t bytes_transferred){
     if (ec.value() != 0) {
        std::cout << "Error , code is " << ec.value() << " . Message is " << ec.message();
        return;
    }

    auto & _send_node = _send_queue.front();
    _send_node->_cur_len += bytes_transferred;
    if(_send_node->_cur_len < _send_node->_total_len){
        this->_socket->async_write_some(boost::asio::buffer(_send_node->_msg+_send_node->_cur_len,_send_node->_total_len-_send_node->_cur_len),std::bind(&Session::WriteCallBack,std::placeholders::_1,std::placeholders::_2));
        return;        
    }

    //发送完毕 pop
    _send_queue.pop();

    if(_send_queue.empty()){
        _send_pending = false;
    }

    if(!_send_queue.empty()){
        _send_node  = _send_queue.front();
        this->_socket->async_write_some(boost::asio::buffer(_send_node->_msg + _send_node->_cur_len,_send_node->_total_len - _send_node->_cur_len),std::bind(&Session::WriteCallBack,std::placeholders::_1,std::placeholders::_2));

    }

}


void Session::WriteAllToSocket(const string & buf){
      _send_queue.emplace(new MsgNode(buf.c_str(),buf.length()));
      if(_send_pending){
        return;
      }
      auto & _send_node = _send_queue.front();
      this->_socket->async_send(boost::asio::buffer(buf.c_str(),buf.length()),std::bind(&Session::WriteAllCallBack,std::placeholders::_1,std::placeholders::_2));
      _send_pending = true;
}
void Session::WriteAllCallBack(const boost::system::error_code & ec ,
                      std::size_t bytes_transferred){
      print_err(ec);
      _send_queue.pop();
      if(_send_queue.empty()){
            _send_pending=false;
      }
      if(!_send_queue.empty()){
        auto & send_data = _send_queue.front();
        this->_socket->async_read_some(boost::asio::buffer(send_data->_msg+send_data->_cur_len,send_data->_total_len-send_data->_cur_len),
                std::bind(&Session::WriteAllCallBack,
        this,std::placeholders::_1,std::placeholders::_2));
      }
}




void Session::ReadFromSocket(){
    //当前正在接收
    if(_recv_pending){
        return;
    }


    _recv_node = make_shared<MsgNode>(RECVSIZE);
    this->_socket->async_read_some(boost::asio::buffer(_recv_node->_msg,_recv_node->_total_len),std::bind(&Session::ReadCallBack,this,std::placeholders::_1,std::placeholders::_2)); //异步读

    _recv_pending=true;                          
}
void Session::ReadCallBack(const boost::system::error_code & ec,
                  size_t bytes_transferred){
        print_err(ec);
        _recv_node->_cur_len+=bytes_transferred;
        if(_recv_node->_cur_len < _recv_node->_total_len){
            this->_socket->async_receive(boost::asio::buffer(_recv_node->_msg+_recv_node->_cur_len,_recv_node->_total_len-_recv_node->_cur_len),std::bind(&Session::ReadCallBack,this,std::placeholders::_1,std::placeholders::_2)); 
            return;
        }

        _recv_pending = false; //收完了
        _recv_node = nullptr;
}

void Session::ReadAllFromSocket(){
    if(_recv_pending){
        return;
    }

    _recv_node = make_shared<MsgNode>(RECVSIZE);
    this->_socket->async_receive(boost::asio::buffer(_recv_node->_msg,_recv_node->_total_len),
    bind(&Session::ReadAllCallBack,std::placeholders::_1,std::placeholders::_2));//只触发一次回调函数，触发的时候已经接收完了

    _recv_pending = true;
}
void Session::ReadAllCallBack(const boost::system::error_code & ec,
                    size_t bytes_transferred){
        print_err(ec);
        _recv_node->_cur_len += bytes_transferred;
        _recv_node = nullptr;
        _recv_pending = false;
}

        