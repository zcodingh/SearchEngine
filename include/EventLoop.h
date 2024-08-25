#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <sys/timerfd.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>

using std::vector;
using std::unordered_map;
using std::shared_ptr;
using std::function;
using std::mutex;

class Acceptor;
class TcpConnection;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr&)>;
using Functor = function<void()>;
class EventLoop
{
public:
    EventLoop(Acceptor& acceptor);
    ~EventLoop();

    void loop();
    void unloop();

    void setTimer(int initialSec, int intervalSec);

    int createEpollFd();
    void waitEpollFd();

    void handleNewConnection();
    void handleMessage(int fd);

    void addEpollReadFd(int fd);
    void delEpollReadFd(int fd);

    void setNewConnectionCallback(TcpConnectionCallback&& cb);
    void setMessageCallback(TcpConnectionCallback&& cb);
    void setCloseCallback(TcpConnectionCallback&& cb);

    int createEventFd();
    int createTimerFd();
    void handleTimer();
    void handleRead();
    void wakeup();
    void doPendingFunctors();
    void runInLoop(Functor&& cb);

private:
    int _epfd;
    vector<struct epoll_event> _evtList;
    bool _isLooping;
    Acceptor& _acceptor;
    unordered_map<int, TcpConnectionPtr> _conns;

    int _evtfd;
    int _timerfd;
    vector<Functor> _pendings;
    mutex _mutex;

    TcpConnectionCallback _onNewConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif
