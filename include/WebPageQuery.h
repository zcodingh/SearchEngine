#ifndef __WEBPAGEQUERY_H__
#define __WEBPAGEQUERY_H__

#include "WebPage.h"
#include "TcpServer.h"
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
    WebPage readWebPage(std::ifstream& ifs, int docID);
    WebPage filterMessage(string& page);
    WebPageQuery();
    void loadOffsetLib();
    void loadInvertIndexTable();
    void loadStopWords(const string& path);
private:
    vector<string> _query_words;
    vector<std::pair<int, int>> _offset_lib;
    unordered_map<string, set<std::pair<int, double>>> _invert_index_table;
    set<string> _stop_words;
};


#endif