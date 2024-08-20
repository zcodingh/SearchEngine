#ifndef __DICTPRODUCER_H__
#define __DICTPRODUCER_H__

#include "../include/cppjieba/Jieba.hpp"
#include "SplitTool.h"
#include "Configuration.h"
#include <string>
#include <vector>
#include <map>
#include <set>

using std::string;
using std::vector;
using std::pair;
using std::map;
using std::set;

class DictProducer
{
public:
    DictProducer(Configuration& conf);
    DictProducer();
    ~DictProducer() {}

    void buildENDict();
    void buildCNDict();
    void createIndex();
    void store();

private:
    void cleanUpdate(const string& word, map<string, int>& freq, set<string>& stop_words);
    void updateFrequency(const string& word, map<string, int>& freq, set<string>& stop_words);
    void readDir(const string& dir, vector<string>& files);
public:
    vector<string> _files_en;
    vector<string> _files_cn;
    vector<pair<string, int>> _dict_en;
    vector<pair<string, int>> _dict_cn;
    map<string, set<int>> _index;
};

#endif
