#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include "../include/ThreadPool.h"
#include "../include/TcpServer.h"

class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con);

    void process();

private:
    string _msg;
    TcpConnectionPtr _con;
};


class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize, const string& ip, unsigned short port);
    ~EchoServer();
    void start();
    void stop();
    void onNewConnection(const TcpConnectionPtr &);
    void onMessage(const TcpConnectionPtr &);
    void onClose(const TcpConnectionPtr &);

private:
    ThreadPool _pool;
    TcpServer _server;
};

#endif
