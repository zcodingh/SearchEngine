#include "../include/InetAddress.h"
#include <arpa/inet.h>
#include <strings.h>

InetAddress::InetAddress(const string& ip, unsigned short port) {
    ::bzero(&_addr, sizeof(struct sockaddr_in));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr);
}

InetAddress::InetAddress(const struct sockaddr_in& addr)
: _addr(addr)
{}

InetAddress::~InetAddress() {}

string InetAddress::ip() const {
    return string(inet_ntoa(_addr.sin_addr));
}

unsigned short InetAddress::port() const {
    return ntohs(_addr.sin_port);
}

const struct sockaddr_in *InetAddress::getInetAddrPtr() const {
    return &_addr;
}
