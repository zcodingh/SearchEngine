#ifndef __WEBPAGEQUERY_H__
#define __WEBPAGEQUERY_H__

#include "WebPage.h"
#include "TcpServer.h"
// #include "Redis.h"
#include "LRUCache.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <memory>
using std::set;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::unique_ptr;

class WebPageQuery {
public:
    static WebPageQuery& getInstance();
    static unique_ptr<WebPageQuery> _instance;
    void cut(const string& query);
    void doQuery(TcpConnectionPtr con);
private:
    unordered_map<string, double> calculateWeight(const vector<string>& queryWords);
    double cosSimilarity(unordered_map<string, double>& queryWeights, 
                         unordered_map<string, double>& docWeights);
    bool readWebPage(std::ifstream& ifs, int docID, WebPage& result);
    // bool readRedis(int docID, WebPage& page);
    bool readLRUCache(int docID, WebPage& page);
    bool filterMessage(string& page, int docID, WebPage& result);
    WebPageQuery();
    bool parsePage(const string& page, const string& label, string& result);
    void loadOffsetLib();
    void loadInvertIndexTable();
    void loadStopWords(const string& path);
private:
    vector<string> _query_words;
    vector<std::pair<int, int>> _offset_lib;
    unordered_map<string, set<std::pair<int, double>>> _invert_index_table;
    set<string> _stop_words;
    LRUCache _cache;
};


#endif