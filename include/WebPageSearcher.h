#ifndef __WEBPAGESEARCHER_H__
#define __WEBPAGESEARCHER_H__

#include "TcpServer.h"
#include "WebPage.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
using std::set;
using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;



class WebPageSearcher {
public:
    WebPageSearcher(const string& query, const TcpConnectionPtr& con);
    void doQuery();
private:
    string _query;
    TcpConnectionPtr _con;
};

#endif