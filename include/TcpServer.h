#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Acceptor.h"
#include "EventLoop.h"

class TcpServer
{
public:
    TcpServer(const string& ip, unsigned short port);
    ~TcpServer() {}
    void start();
    void stop();
    void setAllCallback(TcpConnectionCallback&&, TcpConnectionCallback&&, TcpConnectionCallback&&);

private:
    Acceptor _acceptor;
    EventLoop _loop;
};

#endif
