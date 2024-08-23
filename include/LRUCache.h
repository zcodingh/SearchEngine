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
    void addElement(int docID, const string& value);
    string getElement(int docID);
    void showResultList() const;            // DEBUG
private:
    unordered_map<int, list<std::pair<int, string>>::iterator> _hash_map; 
    list<std::pair<int, string>> _result_list;      // pair<docID, WebPage>
    size_t _capacity;
    mutex _mutex;
};

#endif