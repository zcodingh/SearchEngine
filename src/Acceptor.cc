#include "../include/Acceptor.h"

Acceptor::Acceptor(const string& ip, unsigned short port)
: _sock()
, _addr(ip, port)
{}

Acceptor::~Acceptor() {

}

void Acceptor::ready() {
    setReuseAddr();
    setReusePort();
    bind();
    listen();
}

void Acceptor::setReuseAddr() {
    int on = 1;
    if (setsockopt(_sock.fd(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        perror("setsockopt");
        return;
    }
}

void Acceptor::setReusePort() {
    int on = 1;
    if (setsockopt(_sock.fd(), SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) {
        perror("setsockopt");
        return;
    }
}

void Acceptor::bind() {
    if (::bind(_sock.fd()
               , (const struct sockaddr*)_addr.getInetAddrPtr()
               , sizeof(struct sockaddr))) {
        perror("bind");
        return;
    }
}

void Acceptor::listen() {
    if (::listen(_sock.fd(), 1000)) {
        perror("listen");
        return;
    }
}

int Acceptor::accept() {
    int connfd = ::accept(fd(), nullptr, nullptr);
    if (connfd == -1) {
        perror("accept");
        return -1;
    }
    return connfd;
}

int Acceptor::fd() const {
    return _sock.fd();
}
