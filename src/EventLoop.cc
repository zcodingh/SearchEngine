#include "../include/EventLoop.h"
#include "../include/Acceptor.h"
#include "../include/TcpConnection.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

EventLoop::EventLoop(Acceptor& acceptor)
: _epfd(createEpollFd())
, _evtList(1024)
, _isLooping(false)
, _acceptor(acceptor)
, _evtfd(createEventFd())
, _mutex()
{
    int listenfd = _acceptor.fd();
    addEpollReadFd(listenfd);
    addEpollReadFd(_evtfd);
}

EventLoop::~EventLoop() {
    close(_epfd);
    close(_evtfd);
}

void EventLoop::loop() {
    _isLooping = true;
    while (_isLooping) {
        waitEpollFd();
    }
}

void EventLoop::unloop() {
    _isLooping = false;
}

int EventLoop::createEpollFd() {
    int fd = ::epoll_create(1);
    if (fd < 0) {
        perror("createEpollFd");
        return -1;
    }
    return fd;
}

void EventLoop::waitEpollFd() {
    int nready = 0;
    do {
        nready = ::epoll_wait(_epfd, &_evtList[0], _evtList.size(), 5000);
    } while (nready == -1 && errno == EINTR);

    if (-1 == nready) {
        cerr << "nready = -1" << endl;
        return;
    } else if (0 == nready) {
        cout << ">> TIMEOUT!" << endl;
    } else {
        if (nready == (int)_evtList.size()) {
            _evtList.resize(nready * 2);
        }

        for (int i = 0; i < nready; i++) {
            int fd = _evtList[i].data.fd;
            int listenfd = _acceptor.fd();

            if (fd == listenfd) {
                handleNewConnection();
            } else if (fd == _evtfd) {
                handleRead();
                doPendingFunctors();
            } else {
                handleMessage(fd);
            }
        }
    }
}

void EventLoop::handleNewConnection() {
    int connfd = _acceptor.accept();
    if (connfd < 0) {
        perror("handleNewConnection");
        return;
    }
    addEpollReadFd(connfd);
    TcpConnectionPtr con(new TcpConnection(connfd, this));
    _conns[connfd] = con;
    con->setNewConnectionCallback(_onNewConnection);
    con->setMessageCallback(_onMessage);
    con->setCloseCallback(_onClose);
    con->handleNewConnectionCallback();
}

void EventLoop::handleMessage(int fd) {
    auto it = _conns.find(fd);
    if (it != _conns.end()) {
        if (it->second->isClosed()) {
            it->second->handleCloseCallback();
            delEpollReadFd(it->first);
            _conns.erase(fd);
        } else {
            it->second->handleMessageCallback();
        }
    } else {
        cout << "connection not exist" << endl;
        return;
    }
}

void EventLoop::addEpollReadFd(int fd) {
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    if (::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &evt) < 0) {
        perror("addEpollReadFd");
    }
}

void EventLoop::delEpollReadFd(int fd) {
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    if (::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &evt) < 0) {
        perror("delEpollReadFd");
    }
}

void EventLoop::setNewConnectionCallback(TcpConnectionCallback&& cb) {
    _onNewConnection = std::move(cb);
}
void EventLoop::setMessageCallback(TcpConnectionCallback&& cb) {
    _onMessage = std::move(cb);
}
void EventLoop::setCloseCallback(TcpConnectionCallback&& cb) {
    _onClose = std::move(cb);
}

int EventLoop::createEventFd() {
    int fd = ::eventfd(0, 0);
    if (fd == -1) {
        perror("createEventFd");
        return -1;
    }
    return fd;
}

void EventLoop::handleRead() {
    uint64_t one = 0;
    if (::read(_evtfd, &one, sizeof(uint64_t)) != sizeof(uint64_t)) {
        perror("handleRead");
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t ret = ::write(_evtfd, &one, sizeof(uint64_t));
    if (ret != sizeof(uint64_t)) {
        perror("handleRead");
    }
}

void EventLoop::doPendingFunctors() {
    vector<Functor> tmp;
    {
        std::lock_guard<mutex> lg(_mutex);
        tmp.swap(_pendings);
    }

    for (auto& cb : tmp) {
        cb();
    }
}

void EventLoop::runInLoop(Functor&& cb) {
    {
        std::lock_guard<mutex> lg(_mutex);
        _pendings.push_back(std::move(cb));
    }

    wakeup();
}
