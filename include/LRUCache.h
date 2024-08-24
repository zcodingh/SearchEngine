#ifndef __LRUCACHE_H__
#define __LRUCACHE_H__

#include <string>
#include <unordered_map>
#include <list>
#include <mutex>
using std::mutex;
using std::string;
using std::unordered_map;
using std::list;

class LRUCache {
public:
    LRUCache(size_t num = 100);
    void addElement(const string& query, const string& value);
    string getElement(const string& query);
    void showResultList() const;            // DEBUG
private:
    unordered_map<string, list<std::pair<string, string>>::iterator> _hash_map; 
    list<std::pair<string, string>> _result_list;      
    size_t _capacity;
    mutex _mutex;
};

#endif