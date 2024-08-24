#include "../include/KeyRecommander.h"
#include "../include/nlohmann/json.hpp"
#include "../include/TcpConnection.h"
#include <fstream>
#include <sstream>
using nlohmann::json;

//TODO 先假设一次就查一个词
KeyRecommander::KeyRecommander(const string& query, const TcpConnectionPtr& con)
: _queryWord(query)
, _con(con)
{}

void KeyRecommander::doQuery() {
    const map<string, set<int>>& index = Dictionary::getInstance().getIndex();
    const vector<std::pair<string, int>>& dictEN = Dictionary::getInstance().getDictEN();
    const vector<std::pair<string, int>>& dictCN = Dictionary::getInstance().getDictCN();
    set<CandidateResult> candidate;                 
    vector<string> words = SplitTool::getInstance()->cut(_queryWord);
    for (auto word : words) {   //召回       word是一个词，还需要分出字符
        size_t i = 0;
        while (i < word.size()) {
            size_t nBytes = nBytesCode(word[i]);
            if (i + nBytes > word.size()) {
                break; 
            }
            string character = word.substr(i, nBytes);
            auto it = index.find(character);
            if (it != index.end()) {
                if (nBytes == 1) {
                    for (int idx : it->second) {
                        string candidateString = dictEN[idx].first;
                        int dist = editDistance(candidateString, _queryWord);
                        candidate.insert({candidateString, dictEN[idx].second, dist});
                    }
                } else {
                    for (int idx : it->second) {
                        string candidateString = dictCN[idx].first;
                        int dist = editDistance(candidateString, _queryWord);
                        candidate.insert({candidateString, dictCN[idx].second, dist});
                    }
                }
            }
            i += nBytes; 
        }
    }
    if (candidate.empty())  std::cout << "candidate is empty\n";    // TODO rm
    for (auto result : candidate) {
        _result.push(result);
    }
    json msg;
    int count = 0;      // 默认发出前5个词
    while (!_result.empty() && count < 5) {
        msg.push_back(_result.top()._word);
        _result.pop();
        ++count;
    }
    _con->sendInLoop(msg.dump());
}

//1. 求取一个字符占据的字节数
size_t nBytesCode(const char ch) {
    if(ch & (1 << 7))
    {
        int nBytes = 1;
        for(int idx = 0; idx != 6; ++idx)
        {
            if(ch & (1 << (6 - idx)))
            {
                ++nBytes;
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}

//2. 求取一个字符串的字符长度
size_t length(const string &str) {
    size_t ilen = 0;
    for(size_t idx = 0; idx != str.size(); ++idx)
    {
        int nBytes = nBytesCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }
    return ilen;
}

int triple_min(const int &a, const int &b, const int &c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

int editDistance(const string& lhs, const string& rhs) {
    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);
    int editDist[lhs_len + 1][rhs_len + 1];
    for (size_t idx = 0; idx <= lhs_len; ++idx) {
        editDist[idx][0] = idx;
    }
    for (size_t idx = 0; idx <= rhs_len; ++idx) {
        editDist[0][idx] = idx;
    }

    string sublhs, subrhs;
    for (size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i, ++lhs_idx) {
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);
        for (size_t dist_j = 1, rhs_idx = 0; dist_j <= rhs_len; ++dist_j, ++rhs_idx) {
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);
            if (sublhs == subrhs) {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
            } else {
                editDist[dist_i][dist_j] = triple_min(
                    editDist[dist_i][dist_j - 1] + 1,
                    editDist[dist_i - 1][dist_j] + 1,
                    editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    }
    return editDist[lhs_len][rhs_len];
}
