#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"
#include <memory>
#include <functional>

using std::shared_ptr;
using std::function;

class EventLoop;

class TcpConnection
: public std::enable_shared_from_this<TcpConnection>
{
    using TcpConnectionPtr = shared_ptr<TcpConnection>;
    using TcpConnectionCallback = function<void(TcpConnectionPtr)>;
public:
    explicit TcpConnection(int fd, EventLoop* loop);
    ~TcpConnection();
    void send(const string& msg);
    string receive();
    bool isClosed() const;

    void sendInLoop(const string& msg);

    //为了调试加的函数
    string toString();

private:
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();

public:
    void setNewConnectionCallback(const TcpConnectionCallback& cb);
    void setMessageCallback(const TcpConnectionCallback& cb);
    void setCloseCallback(const TcpConnectionCallback& cb);
    
    void handleNewConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();

private:
    EventLoop* _loop;
    SocketIO _sockIO;

    TcpConnectionCallback _onNewConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;

    // 为了调试加的三个数据成员
    Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;
};

#endif
