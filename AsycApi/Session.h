#include<memory>
#include<boost/asio.hpp>
#include<iostream>
#include<queue>
using namespace std;
using namespace boost::asio;

const int RECVSIZE = 1024;

//封装一个Node结构，用来管理要发送和接收的数据
class MsgNode{
    public:
        MsgNode(const char * msg , int total_len):_total_len(total_len),_cur_len(0){
            _msg = new char[_total_len];
            memcpy(_msg,msg,total_len);
        }

        //作为接收节点的构造
        MsgNode(int total_len):_total_len(total_len),_cur_len(0){
            _msg = new char[_total_len];

        }

        ~MsgNode(){
            delete [] _msg;
        }
    public:
        int _total_len;
        int _cur_len;
        char * _msg;
};

//服务器处理客户端连接的管理类
class Session{
    public:
        Session(std::shared_ptr<ip::tcp::socket>  socket);
        void Connect(const ip::tcp::endpoint & ep);
        void print_err(const boost::system::error_code & ec);

        void WriteCallBackErr(const boost::system::error_code & ec,
                              size_t bytes_transferred,
                              shared_ptr<MsgNode> msg_node);
        void WriteToSocketErr(const string  buf);


        void WriteToSocket(const string & buf);
        void WriteCallBack(const boost::system::error_code & ec , 
                           size_t bytes_transferred);

        void WriteAllToSocket(const string & buf);
        void WriteAllCallBack(const boost::system::error_code & ec ,
                              std::size_t bytes_transferred);

        void ReadFromSocket();
        void ReadCallBack(const boost::system::error_code & ec,
                          size_t bytes_transferred);

        void ReadAllFromSocket();
        void ReadAllCallBack(const boost::system::error_code & ec,
                          size_t bytes_transferred);
    private:
        queue<shared_ptr<MsgNode>> _send_queue;//发送队列 缓存要发送的消息节点，实现异步发送功能
        shared_ptr<ip::tcp::socket> _socket;
        shared_ptr<MsgNode> _send_node;
        bool _send_pending;//True-节点未发送完毕 
        
        shared_ptr<MsgNode> _recv_node;
        bool _recv_pending;
};