#ifndef __KEYRECOMMANDER_H__
#define __KEYRECOMMANDER_H__

#include "TcpServer.h"
#include "Configuration.h"
#include "SplitTool.h"
#include "Dictionary.h"
#include <queue>
#include <map>
#include <set>
using std::map;
using std::set;

struct CandidateResult {
    string _word;
    int _freq;
    int _dist;
    bool operator<(const CandidateResult& other) const {
        if (_dist != other._dist) {
            return _dist > other._dist;  // 越小越优先
        }
        if (_freq != other._freq) {
            return _freq < other._freq;  // 越大越优先
        }
        return _word > other._word;  // 字典序越小越优先
    }
};

class KeyRecommander {
public:
    KeyRecommander(const string& query, const TcpConnectionPtr& con);
    void doQuery();

private:
    string _queryWord;
    TcpConnectionPtr _con;
    std::priority_queue<CandidateResult> _result;
};

size_t nBytesCode(const char ch);
size_t length(const string &str);
int editDistance(const string& lhs, const string& rhs);
int triple_min(const int &a, const int &b, const int &c);

#endif
