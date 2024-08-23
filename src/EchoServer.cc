#include "../include/EchoServer.h"
#include "../include/TcpConnection.h"
#include <iostream>

using std::cout;

MyTask::MyTask(const string &msg, const TcpConnectionPtr &con)
: _msg(msg)
, _con(con)
{}

void MyTask::process()
{
    /*
     *
     * bussiness process
     *
     */
    _con->sendInLoop(_msg);
}


EchoServer::EchoServer(size_t threadNum, size_t queSize, const string& ip, unsigned short port) 
: _pool(threadNum, queSize)
, _server(ip, port)
{}

EchoServer::~EchoServer() {}

void EchoServer::start() {
    _pool.start();
    using namespace std::placeholders;
    _server.setAllCallback(std::bind(&EchoServer::onNewConnection, this, _1), 
                           std::bind(&EchoServer::onMessage, this, _1),
                           std::bind(&EchoServer::onClose, this, _1));
    _server.start();
}

void EchoServer::stop() {
    _pool.stop();
    _server.stop();
}

void EchoServer::onNewConnection(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has connected!!!\n";
}

void EchoServer::onMessage(const TcpConnectionPtr &con)
{
    //接收客户端的数据
    string msg = con->receive();

    MyTask task(msg, con);

    _pool.addTask(std::bind(&MyTask::process, task));
}

void EchoServer::onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has closed!!!\n";
}

