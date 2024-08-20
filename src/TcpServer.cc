#include "../include/TcpServer.h"

TcpServer::TcpServer(const string& ip, unsigned short port)
: _acceptor(ip, port)
, _loop(_acceptor)
{}

void TcpServer::start() {
    _acceptor.ready();
    _loop.loop();
}

void TcpServer::stop() {
    _loop.unloop();
}

void TcpServer::setAllCallback(TcpConnectionCallback&& con_cb, TcpConnectionCallback&& msg_cb, TcpConnectionCallback&& clo_cb) {
    _loop.setNewConnectionCallback(std::move(con_cb));
    _loop.setMessageCallback(std::move(msg_cb));
    _loop.setCloseCallback(std::move(clo_cb));
}
