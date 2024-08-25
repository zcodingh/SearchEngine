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
    void addElement(const list<std::pair<string, string>>& pendingUpdateList);
    string getElement(const string& query);

    void syncToMasterCache(LRUCache& masterCache);
    void syncFromMasterCache(const LRUCache& masterCache);
    // void clear();

    void showResultList() const;            // DEBUG
private:
    unordered_map<string, list<std::pair<string, string>>::iterator> _hash_map; 
    list<std::pair<string, string>> _result_list;      
    list<std::pair<string, string>> _pending_update_list;
    size_t _capacity;
    mutex _mutex;
};

#endif
