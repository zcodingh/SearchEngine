#ifndef _SEARCHENGINESERVER_H
#define _SEARCHENGINESERVER_H

#include "../include/ThreadPool.h"
#include "../include/TcpServer.h"

class SearchEngineServer {
public: 
    SearchEngineServer();
    void start();
    void stop();

    void onNewConnection(const TcpConnectionPtr &con);
    void onMessage(const TcpConnectionPtr &con);
    void onClose(const TcpConnectionPtr &con);

    void doTaskThread(const TcpConnectionPtr& tcpConPtr, string& msg);
private: 
    ThreadPool _threadpool;
    TcpServer _tcpserver;
};

#endif 
