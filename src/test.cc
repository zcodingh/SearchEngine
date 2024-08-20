#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "TcpServer.h"
#include "EchoServer.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

ThreadPool *gPool = nullptr;


void test()
{
    EchoServer server(4, 10, "127.0.0.1", 8888);
    server.start();
}

int main(int argc, char **argv)
{
    test();
    return 0;
}
