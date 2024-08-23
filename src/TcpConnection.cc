#include "../include/TcpConnection.h"
#include "../include/EventLoop.h"
#include <iostream>
#include <sstream>

using std::ostringstream;
using std::cout;

TcpConnection::TcpConnection(int fd, EventLoop* loop)
: _loop(loop)
, _sockIO(fd)
, _sock(fd)
, _localAddr(getLocalAddr())
, _peerAddr(getPeerAddr())
{}

TcpConnection::~TcpConnection() {}

void TcpConnection::send(const string& msg) {
    _sockIO.writeTrain(msg.c_str(), msg.size());
}

string TcpConnection::receive() {
    char buff[65535] = { 0 };
    _sockIO.readTrain(buff, sizeof(buff));
    return string(buff);
}

bool TcpConnection::isClosed() const {
    char buf[8] = { 0 };
    return ::recv(_sock.fd(), buf, sizeof(buf), MSG_PEEK) == 0;
}

void TcpConnection::sendInLoop(const string& msg) {
    if (_loop) {
        _loop->runInLoop(std::bind(&TcpConnection::send, this, msg));
    }
}

string TcpConnection::toString() {
    ostringstream oss;
    oss << _localAddr.ip() << ":"
       << _localAddr.port() << "---->"
       << _peerAddr.ip() << ":"
       << _peerAddr.port();
    return oss.str();
}

InetAddress TcpConnection::getLocalAddr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    if (getsockname(_sock.fd(), (struct sockaddr*)&addr, &len)) {
        perror("getsockname");
    }

    return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    if (getpeername(_sock.fd(), (struct sockaddr*)&addr, &len)) {
        perror("getpeername");
    }

    return InetAddress(addr);
}

void TcpConnection::setNewConnectionCallback(const TcpConnectionCallback& cb) {
    _onNewConnection = cb;
}

void TcpConnection::setMessageCallback(const TcpConnectionCallback& cb) {
    _onMessage = cb;
}

void TcpConnection::setCloseCallback(const TcpConnectionCallback& cb) {
    _onClose = cb;
}

void TcpConnection::handleNewConnectionCallback() {
    if (_onNewConnection) {
        _onNewConnection(shared_from_this());
    } else {
        cout << "_onNewConnection == nullptr\n";
    }
}

void TcpConnection::handleMessageCallback() {
    if (_onMessage) {
        _onMessage(shared_from_this());
    } else {
        cout << "_onMessage == nullptr\n";
    }
}

void TcpConnection::handleCloseCallback() {
    if (_onClose) {
        _onClose(shared_from_this());
    } else {
        cout << "_onClose == nullptr\n";
    }
}
