#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__
#include <string>
#include <map>
#include <vector>
#include <set>
#include <memory>
using std::string;
using std::vector;
using std::map;
using std::set;

class Dictionary {
public:
    ~Dictionary(){}
    static Dictionary& getInstance();
    const vector<std::pair<string, int>>& getDictEN() const { return _dict_en; }
    const vector<std::pair<string, int>>& getDictCN() const { return _dict_cn; }
    const map<string, set<int>>& getIndex() const { return _index; }
    Dictionary(const Dictionary&) = delete;
    Dictionary& operator=(Dictionary&) = delete;
private:
    Dictionary();
    vector<std::pair<string, int>> _dict_en;
    vector<std::pair<string, int>> _dict_cn;
    map<string, set<int>> _index;
    static std::unique_ptr<Dictionary> _instance;
};

#endif 

