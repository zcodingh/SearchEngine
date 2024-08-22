#include "../include/Configuration.h"
#include "../include/SearchEngineServer.h"
#include "../include/TcpConnection.h"
#include "../include/KeyRecommander.h"
#include "../include/WebPageSearcher.h"
#include <iostream>

using std::cout;


SearchEngineServer::SearchEngineServer() 
: _threadpool(std::stoi(Configuration::getInstance().getValue("server", "thread_number")), 
              std::stoi(Configuration::getInstance().getValue("server", "taskque_size")))
, _tcpserver(Configuration::getInstance().getValue("server", "server_ip"), 
             std::stoi(Configuration::getInstance().getValue("server", "server_port")))
{}


void SearchEngineServer::start() {
    _threadpool.start();
    using namespace std::placeholders;
    _tcpserver.setAllCallback(std::bind(&SearchEngineServer::onNewConnection, this, _1), 
                           std::bind(&SearchEngineServer::onMessage, this, _1),
                           std::bind(&SearchEngineServer::onClose, this, _1));
    _tcpserver.start();
}

void SearchEngineServer::stop() {
    _threadpool.stop();
    _tcpserver.stop();
}

void SearchEngineServer::onNewConnection(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has connected!!!\n";
}

void SearchEngineServer::onMessage(const TcpConnectionPtr &con) //消息格式x:msg 0=关键字，1=网页查询
{
    //接收客户端的数据
    string msg = con->receive();
    if (msg.size() < 2) return;
    char type = msg[0];
    switch (type) {
    case '0': {
        KeyRecommander task(msg.substr(2), con);
        _threadpool.addTask(std::bind(&KeyRecommander::doQuery, task));
        break;
    }
    case '1':{
        WebPageSearcher task(msg.substr(2), con);
        _threadpool.addTask(std::bind(&WebPageSearcher::doQuery, task));
        break;
    }
    default:
        break;
    }
}

void SearchEngineServer::onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has closed!!!\n";
}
