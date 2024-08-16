#ifndef __DICTPRODUCER_H__
#define __DICTPRODUCER_H__

#include "../include/cppjieba/Jieba.hpp"
#include "SplitTool.h"
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
    DictProducer(const string& dir);
    DictProducer(const string& dir, SplitTool* splitTool);
    ~DictProducer() {}

    void buildEnDict();
    void buildCnDict();
    void createIndex();
    void store();

private:
    void cleanUpdateFrequency(const string& word, map<string, int>& freq);

public:
    vector<string> _files;
    vector<pair<string, int>> _dict;
    map<string, set<int>> _index;
    SplitTool* _cuttor;
};

#endif
